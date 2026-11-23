#include "AndroidEngine.h"

#include "appdraw/AdornG3D.h"
#include "gui/GUI.h"
#include "humanoid/Humanoid.h"
#include "network/API.h"
#include "network/Player.h"
#include "network/Players.h"
#include "rbxview/View.h"
#include "renderlib/Mesh.h"
#include "script/ScriptContext.h"
#include "security/SecurityContext.h"
#include "util/ContentProvider.h"
#include "util/FileSystem.h"
#include "util/FrameInterpolator.h"
#include "util/Profiling.h"
#include "util/RunStateOwner.h"
#include "util/Timer.h"
#include "v8datamodel/Accoutrement.h"
#include "v8datamodel/Backpack.h"
#include "v8datamodel/BrickColor.h"
#include "v8datamodel/Camera.h"
#include "v8datamodel/CharacterAppearance.h"
#include "v8datamodel/DataModel.h"
#include "v8datamodel/Decal.h"
#include "v8datamodel/GameSettings.h"
#include "v8datamodel/GlobalSettings.h"
#include "v8datamodel/Lighting.h"
#include "v8datamodel/LocalBackpack.h"
#include "v8datamodel/ModelInstance.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/TimeState.h"
#include "v8datamodel/UserController.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/factoryregistration.h"
#include "v8tree/Service.h"
#include "v8xml/SerializerV2.h"

#include <GLG3D/GLCaps.h>
#include <GLG3D/gl_es_compat.h>
#include <android/log.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "OpenRBX-Engine", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "OpenRBX-Engine", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "OpenRBX-Engine", __VA_ARGS__)

namespace RBX::Android {

AndroidEngine& AndroidEngine::instance()
{
	static AndroidEngine s_instance;
	return s_instance;
}

void AndroidEngine::initGlobal(
	const std::string& filesDir, const std::string& cacheDir, const std::string& contentDir
)
{
	std::scoped_lock lock(m_engineMutex);
	m_filesDir = filesDir;
	m_cacheDir = cacheDir;
	m_contentDir = contentDir;

	RBX::FileSystem::setAndroidBasePaths(filesDir, cacheDir, contentDir);

	if (m_globalInitialized) {
		if (!contentDir.empty()) {
			RBX::ContentProvider::singleton().setAssetFolder(contentDir.c_str());
		}
		return;
	}

	RBX::Profiling::init(false);
	static RBX::FactoryRegistrator s_registrator;
	RBX::Network::API::init("0.9.0-android");
	RBX::GameSettings::singleton();
	RBX::GlobalSettings::singleton();

	if (!contentDir.empty()) {
		RBX::ContentProvider::singleton().setAssetFolder(contentDir.c_str());
	}

	m_userInput = std::make_unique<AndroidUserInput>();
	m_globalInitialized = true;
	LOGI("Global engine initialized (content=%s)", contentDir.c_str());
}

void AndroidEngine::attachSurface(ANativeWindow* window, int width, int height)
{
	std::scoped_lock lock(m_engineMutex);
	m_width = std::max(1, width);
	m_height = std::max(1, height);

	if (!m_window) {
		m_window = std::make_unique<AndroidWindow>(window, m_width, m_height);
		m_renderDevice = std::make_unique<G3D::RenderDevice>();
		m_renderDevice->init(m_window.get(), nullptr);
		G3D::GLCaps::init();
		m_renderDevice->notifyResize(m_width, m_height);
		m_renderDevice->setViewport(G3D::Rect2D::xywh(0, 0, m_width, m_height));
	} else {
		m_window->attachSurface(window, m_width, m_height);
		m_window->makeCurrent();
		if (m_renderDevice) {
			m_renderDevice->notifyResize(m_width, m_height);
			m_renderDevice->setViewport(G3D::Rect2D::xywh(0, 0, m_width, m_height));
		}
	}

	if (m_userInput) {
		m_userInput->setViewportSize(m_width, m_height);
	}

	if (m_dataModel && !m_view) {
		RBX::DataModel::Lock dmLock(m_dataModel);
		setupViewAndGuiLocked();
	}
}

void AndroidEngine::resizeSurface(int width, int height)
{
	std::scoped_lock lock(m_engineMutex);
	if (width <= 0 || height <= 0) {
		return;
	}
	m_width = width;
	m_height = height;
	if (m_userInput) {
		m_userInput->setViewportSize(width, height);
	}
	if (m_window && m_window->hasValidSurface() && m_renderDevice) {
		m_window->makeCurrent();
		m_renderDevice->notifyResize(width, height);
		m_renderDevice->setViewport(G3D::Rect2D::xywh(0, 0, width, height));
	}
}

void AndroidEngine::detachSurface()
{
	std::scoped_lock lock(m_engineMutex);
	if (m_window) {
		m_window->detachSurface();
	}
}

void AndroidEngine::setupViewAndGuiLocked()
{
	if (!m_dataModel || !m_renderDevice || !m_window || !m_window->hasValidSurface()) {
		return;
	}

	m_window->makeCurrent();
	m_view = std::make_unique<RBX::View::View>(m_dataModel);
	m_adorn = std::make_unique<RBX::AdornG3D>(m_renderDevice.get());

	if (m_userInput) {
		m_userInput->init(m_adorn.get());
		m_dataModel->create<RBX::ControllerService>()->setHardwareDevice(m_userInput.get());
	}

	m_dataModel->buildGui(m_adorn.get());

	// Hide desktop menus that collide with mobile HUD, keep HealthHud/ScoreHud/ChatHud/LocalBackpack
	if (auto* guiRoot = m_dataModel->getGuiRoot()) {
		if (auto* mainMenu = guiRoot->findFirstChildByName("MainMenu")) {
			if (auto* bar = dynamic_cast<RBX::TopMenuBar*>(mainMenu)) bar->setVisible(false);
		}
		if (auto* controlHud = guiRoot->findFirstChildByName("ControlHud")) {
			if (auto* bar = dynamic_cast<RBX::TopMenuBar*>(controlHud)) bar->setVisible(false);
		}
		if (auto* rightPalette = guiRoot->findFirstChildByName("RightPalette")) {
			if (auto* bar = dynamic_cast<RBX::TopMenuBar*>(rightPalette)) bar->setVisible(false);
		}
	}

	m_viewValid = true;
	m_dataModel->create<RBX::RunService>()->addRunView(this);
	RBX::Render::Mesh::makeVAR();
	RBX::Render::Mesh::varDirty = true;
}

void AndroidEngine::closePlace()
{
	std::scoped_lock lock(m_engineMutex);
	if (m_dataModel) {
		{
			RBX::DataModel::Lock dmLock(m_dataModel);
			if (auto* controllers = m_dataModel->find<RBX::ControllerService>()) {
				controllers->setHardwareDevice(nullptr);
			}
			if (auto* runService = m_dataModel->find<RBX::RunService>()) {
				runService->removeRunView(this);
			}
			m_view.reset();
			m_adorn.reset();
		}
		RBX::DataModel::closeDataModel(m_dataModel, true);
		m_dataModel.reset();
	}
}

bool AndroidEngine::loadPlace(const std::string& rbxlPath)
{
	closePlace();

	std::scoped_lock lock(m_engineMutex);
	m_currentPlacePath = rbxlPath;
	m_dataModel = RBX::DataModel::createDataModel();

	try {
		RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
		RBX::DataModel::Lock dmLock(m_dataModel);

		if (!rbxlPath.empty()) {
			std::ifstream stream(rbxlPath, std::ios_base::in | std::ios_base::binary);
			if (!stream.is_open()) {
				LOGE("Unable to open place file: %s", rbxlPath.c_str());
				return false;
			}
			SerializerV2 serializer;
			serializer.load(stream, m_dataModel.get());
		}

		m_dataModel->getWorkspace()->joinAllHack();

		auto* players = m_dataModel->create<RBX::Network::Players>();
		RBX::Network::Player* localPlayer = RBX::Network::Players::findLocalPlayer(m_dataModel.get());
		if (!localPlayer) {
			auto created = players->createLocalPlayer(1);
			localPlayer = dynamic_cast<RBX::Network::Player*>(created.get());
		}
		if (localPlayer) {
			localPlayer->setName(m_avatar.playerName.empty() ? "Player" : m_avatar.playerName);
			localPlayer->loadCharacter();
			applyAvatarToCharacterLocked();
		}

		setupViewAndGuiLocked();
		m_dataModel->create<RBX::RunService>()->run();
		m_lastStepTime = RBX::getRealTime();
		LOGI("Loaded place successfully: %s", rbxlPath.c_str());
		return true;
	}
	catch (const std::exception& e) {
		LOGE("loadPlace exception: %s", e.what());
		return false;
	}
}

void AndroidEngine::respawnCharacter()
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return;

	try {
		RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
		RBX::DataModel::Lock dmLock(m_dataModel);
		if (auto* player = RBX::Network::Players::findLocalPlayer(m_dataModel.get())) {
			player->loadCharacter();
			applyAvatarToCharacterLocked();
		}
	}
	catch (const std::exception& e) {
		LOGE("respawnCharacter failed: %s", e.what());
	}
}

void AndroidEngine::setAvatarConfig(const AvatarConfig& config)
{
	std::scoped_lock lock(m_engineMutex);
	m_avatar = config;
	if (m_dataModel) {
		try {
			RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
			RBX::DataModel::Lock dmLock(m_dataModel);
			if (auto* player = RBX::Network::Players::findLocalPlayer(m_dataModel.get())) {
				if (!m_avatar.playerName.empty()) {
					player->setName(m_avatar.playerName);
				}
			}
			applyAvatarToCharacterLocked();
		}
		catch (const std::exception& e) {
			LOGW("Live avatar update skipped: %s", e.what());
		}
	}
}

AvatarConfig AndroidEngine::getAvatarConfig() const
{
	std::scoped_lock lock(m_engineMutex);
	return m_avatar;
}

void AndroidEngine::applyAvatarToCharacterLocked()
{
	if (!m_dataModel) return;
	auto* player = RBX::Network::Players::findLocalPlayer(m_dataModel.get());
	if (!player) return;

	RBX::ModelInstance* character = player->getCharacter();
	if (!character) return;

	// Remove prior CharacterAppearance / Accoutrement items
	std::vector<std::shared_ptr<RBX::Instance>> toRemove;
	for (unsigned int i = 0; i < character->numChildren(); ++i) {
		RBX::Instance* child = character->getChild(i);
		if (dynamic_cast<RBX::CharacterAppearance*>(child) || dynamic_cast<RBX::Accoutrement*>(child)) {
			toRemove.push_back(RBX::shared_from(child));
		}
	}
	for (auto& inst : toRemove) {
		inst->setParent(nullptr);
	}

	// Apply BodyColors via Reflection BoundProp descriptors
	auto bodyColors = RBX::Creatable<RBX::Instance>::create<RBX::BodyColors>();
	if (auto* bc = dynamic_cast<RBX::BodyColors*>(bodyColors.get())) {
		RBX::BodyColors::prop_HeadColor.setValue(bc, RBX::BrickColor(m_avatar.headColor));
		RBX::BodyColors::prop_TorsoColor.setValue(bc, RBX::BrickColor(m_avatar.torsoColor));
		RBX::BodyColors::prop_LeftArmColor.setValue(bc, RBX::BrickColor(m_avatar.leftArmColor));
		RBX::BodyColors::prop_RightArmColor.setValue(bc, RBX::BrickColor(m_avatar.rightArmColor));
		RBX::BodyColors::prop_LeftLegColor.setValue(bc, RBX::BrickColor(m_avatar.leftLegColor));
		RBX::BodyColors::prop_RightLegColor.setValue(bc, RBX::BrickColor(m_avatar.rightLegColor));
	}
	bodyColors->setParent(character);

	// Apply ShirtGraphic (Classic T-Shirt / Shirt graphic on Torso)
	if (!m_avatar.shirtAsset.empty()) {
		auto sg = RBX::Creatable<RBX::Instance>::create<RBX::ShirtGraphic>();
		if (auto* graphic = dynamic_cast<RBX::ShirtGraphic*>(sg.get())) {
			RBX::ShirtGraphic::prop_Graphic.setValue(graphic, RBX::TextureId(m_avatar.shirtAsset));
		}
		sg->setParent(character);
	}

	// Apply Face decal on Head
	if (!m_avatar.faceAsset.empty()) {
		if (auto* headPart = dynamic_cast<RBX::PartInstance*>(character->findFirstChildByName("Head"))) {
			if (auto* face = headPart->findFirstChildOfType<RBX::Decal>()) {
				face->setTexture(RBX::TextureId(m_avatar.faceAsset));
			} else {
				auto decal = RBX::Creatable<RBX::Instance>::create<RBX::Decal>();
				if (auto* d = dynamic_cast<RBX::Decal*>(decal.get())) {
					d->setName("face");
					d->setFace(RBX::NORM_Z_NEG);
					d->setTexture(RBX::TextureId(m_avatar.faceAsset));
				}
				decal->setParent(headPart);
			}
		}
	}

	// Apply Hat (.rbxm)
	if (!m_avatar.hatAsset.empty()) {
		try {
			auto hatItems = m_dataModel->get(RBX::ContentId(m_avatar.hatAsset));
			if (hatItems) {
				for (const auto& item : *hatItems) {
					if (dynamic_cast<RBX::Accoutrement*>(item.get()) ||
						dynamic_cast<RBX::CharacterAppearance*>(item.get())) {
						item->setParent(character);
					}
				}
			}
		}
		catch (const std::exception& e) {
			LOGW("Could not load hat '%s': %s", m_avatar.hatAsset.c_str(), e.what());
		}
	}
}

void AndroidEngine::setVirtualJoystick(float x, float y)
{
	if (m_userInput) {
		m_userInput->setVirtualJoystick(x, y);
	}
}

void AndroidEngine::setVirtualJump(bool down)
{
	if (m_userInput) {
		m_userInput->setVirtualJump(down);
	}
}

void AndroidEngine::touchDown(float x, float y)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel || !m_userInput) return;
	RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
	RBX::DataModel::Lock dmLock(m_dataModel);
	m_userInput->dispatchMouseEvent(m_dataModel.get(), RBX::UIEvent::MOUSE_LEFT_BUTTON_DOWN, x, y);
}

void AndroidEngine::touchMove(float x, float y)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel || !m_userInput) return;
	RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
	RBX::DataModel::Lock dmLock(m_dataModel);
	m_userInput->dispatchMouseEvent(m_dataModel.get(), RBX::UIEvent::MOUSE_MOVE, x, y);
}

void AndroidEngine::touchUp(float x, float y)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel || !m_userInput) return;
	RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
	RBX::DataModel::Lock dmLock(m_dataModel);
	m_userInput->dispatchMouseEvent(m_dataModel.get(), RBX::UIEvent::MOUSE_LEFT_BUTTON_UP, x, y);
}

void AndroidEngine::panAndTiltCamera(float deltaXPixels, float deltaYPixels)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return;
	RBX::DataModel::Lock dmLock(m_dataModel);
	if (auto* ws = m_dataModel->getWorkspace()) {
		if (auto* cam = ws->getCamera()) {
			constexpr float kPanSensitivity = 0.0065f;
			constexpr float kTiltSensitivity = 0.0055f;
			if (deltaXPixels != 0.0f) {
				cam->panRadians(-deltaXPixels * kPanSensitivity);
			}
			if (deltaYPixels != 0.0f) {
				cam->tiltRadians(deltaYPixels * kTiltSensitivity);
			}
		}
	}
}

void AndroidEngine::zoomCamera(float zoomDelta)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return;
	RBX::DataModel::Lock dmLock(m_dataModel);
	if (auto* ws = m_dataModel->getWorkspace()) {
		if (auto* cam = ws->getCamera()) {
			cam->zoom(zoomDelta);
		}
	}
}

void AndroidEngine::equipBackpackSlot(int slotIndex)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return;

	RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
	RBX::DataModel::Lock dmLock(m_dataModel);

	if (auto* lb = m_dataModel->find<RBX::LocalBackpack>()) {
		if (slotIndex >= 0 && static_cast<unsigned int>(slotIndex) < lb->numChildren()) {
			if (auto* item = dynamic_cast<RBX::LocalBackpackItem*>(lb->getChild(slotIndex))) {
				lb->onClick(item);
			}
		}
	}
}

std::string AndroidEngine::getBackpackItemsJson()
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return "[]";
	RBX::DataModel::Lock dmLock(m_dataModel);

	auto* lb = m_dataModel->find<RBX::LocalBackpack>();
	if (!lb) return "[]";

	std::ostringstream out;
	out << "[";
	bool first = true;
	for (unsigned int i = 0; i < lb->numChildren(); ++i) {
		auto* localItem = dynamic_cast<RBX::LocalBackpackItem*>(lb->getChild(i));
		if (!localItem || !localItem->getItem()) continue;
		RBX::BackpackItem* bpItem = localItem->getItem();
		if (!first) out << ",";
		first = false;
		std::string name = bpItem->getName();
		std::string escaped;
		for (char c : name) {
			if (c == '"' || c == '\\') escaped += '\\';
			escaped += c;
		}
		out << "{\"index\":" << i << ",\"name\":\"" << escaped << "\",\"selected\":"
			<< (bpItem->drawSelected() ? "true" : "false") << "}";
	}
	out << "]";
	return out.str();
}

float AndroidEngine::getLocalHealthRatio()
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return 1.0f;
	RBX::DataModel::Lock dmLock(m_dataModel);
	if (auto* ch = RBX::Network::Players::findLocalCharacter(m_dataModel.get())) {
		if (auto* hum = RBX::Humanoid::modelIsCharacter(ch)) {
			float maxH = hum->getMaxHealth();
			return maxH > 0.0f ? std::clamp(hum->getHealth() / maxH, 0.0f, 1.0f) : 0.0f;
		}
	}
	return 1.0f;
}

void AndroidEngine::runLuaCommand(const std::string& code)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel || code.empty()) return;
	try {
		RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
		RBX::DataModel::Lock dmLock(m_dataModel);
		std::vector<RBX::Reflection::Value> args;
		m_dataModel->create<RBX::ScriptContext>()->executeInNewThread(
			RBX::Security::CmdLine, code.c_str(), "MobileCmd", args
		);
	}
	catch (const std::exception& e) {
		LOGW("Lua error: %s", e.what());
	}
}

void AndroidEngine::setVisualStyle(int mode)
{
	rbx_gl_es_set_visual_style(mode);
}

void AndroidEngine::setTimeOfDay(float hours)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return;
	RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
	RBX::DataModel::Lock dmLock(m_dataModel);
	if (auto* lighting = m_dataModel->create<RBX::Lighting>()) {
		float clamped = std::clamp(hours, 0.0f, 23.95f);
		lighting->setMinutesAfterMidnight(static_cast<double>(clamped) * 60.0);
	}
}

bool AndroidEngine::toggleFirstPerson()
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return false;
	RBX::DataModel::Lock dmLock(m_dataModel);
	if (auto* ws = m_dataModel->getWorkspace()) {
		if (auto* cam = ws->getCamera()) {
			if (cam->isFirstPersonCamera()) {
				cam->setDistanceFromTarget(13.0f);
				cam->zoom(-1.0f);
				return false;
			} else {
				cam->setDistanceFromTarget(0.0f);
				cam->zoom(1.0f);
				return true;
			}
		}
	}
	return false;
}

void AndroidEngine::spawnQuickBrick(int brickColorId, bool anchored)
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) return;
	RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
	RBX::DataModel::Lock dmLock(m_dataModel);

	auto* ws = m_dataModel->getWorkspace();
	if (!ws) return;

	G3D::Vector3 spawnPos(0.0f, 10.0f, 0.0f);
	if (auto* ch = RBX::Network::Players::findLocalCharacter(m_dataModel.get())) {
		if (auto* hum = RBX::Humanoid::modelIsCharacter(ch)) {
			if (auto* torso = hum->getTorso()) {
				const G3D::CoordinateFrame& cf = torso->getCoordinateFrame();
				spawnPos = cf.translation + cf.lookVector() * 6.5f + G3D::Vector3(0.0f, 2.5f, 0.0f);
			}
		}
	}

	auto partInst = RBX::Creatable<RBX::Instance>::create<RBX::PartInstance>();
	if (auto* part = dynamic_cast<RBX::PartInstance*>(partInst.get())) {
		part->setName("MobileBrick");
		part->setPartSizeXml(G3D::Vector3(4.0f, 2.0f, 2.0f));
		part->setColor(RBX::BrickColor(brickColorId));
		part->setAnchored(anchored);
		part->setCoordinateFrame(G3D::CoordinateFrame(spawnPos));
		part->setParent(ws);
	}
}

std::string AndroidEngine::getStatsJson()
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_dataModel) {
		return "{\"fps\":0,\"parts\":\"0\",\"timeHours\":14.0,\"firstPerson\":false,\"health\":1.0}";
	}
	RBX::DataModel::Lock dmLock(m_dataModel);

	double fps = m_dataModel->timeState ? m_dataModel->timeState->fpsStopwatch.smoothFPS() : 0.0;
	if (!std::isfinite(fps) || fps < 0.0) fps = 0.0;

	std::string parts = m_dataModel->evaluate("numPrimitives");
	double timeHours = 14.0;
	if (auto* lighting = m_dataModel->find<RBX::Lighting>()) {
		timeHours = lighting->getMinutesAfterMidnight() / 60.0;
	}
	bool fp = false;
	if (auto* ws = m_dataModel->getWorkspace()) {
		if (auto* cam = ws->getCamera()) {
			fp = cam->isFirstPersonCamera();
		}
	}
	float health = 1.0f;
	if (auto* ch = RBX::Network::Players::findLocalCharacter(m_dataModel.get())) {
		if (auto* hum = RBX::Humanoid::modelIsCharacter(ch)) {
			float maxH = hum->getMaxHealth();
			health = maxH > 0.0f ? std::clamp(hum->getHealth() / maxH, 0.0f, 1.0f) : 0.0f;
		}
	}

	std::ostringstream out;
	out << "{\"fps\":" << static_cast<int>(fps + 0.5)
		<< ",\"parts\":\"" << parts << "\""
		<< ",\"timeHours\":" << timeHours
		<< ",\"firstPerson\":" << (fp ? "true" : "false")
		<< ",\"health\":" << health << "}";
	return out.str();
}

void AndroidEngine::stepAndRender()
{
	std::scoped_lock lock(m_engineMutex);
	if (!m_window || !m_window->hasValidSurface() || !m_renderDevice) {
		return;
	}

	m_window->makeCurrent();

	if (!m_dataModel || !m_view) {
		m_renderDevice->beginFrame();
		m_renderDevice->setColorClearValue(G3D::Color4(0.11f, 0.13f, 0.16f, 1.0f));
		m_renderDevice->clear();
		m_renderDevice->endFrame();
		m_window->swapGLBuffers();
		return;
	}

	m_renderDevice->beginFrame();
	m_renderDevice->pushState();
	try {
		RBX::Security::Impersonator impersonate(RBX::Security::LocalGUI);
		RBX::DataModel::Lock dmLock(m_dataModel);

		m_dataModel->timeState->fpsStopwatch.tick();
		m_dataModel->timeState->fpsStopwatch.tock();

		if (auto* runService = m_dataModel->find<RBX::RunService>()) {
			RBX::FrameInterpolator::beginRender(
				runService->getStepCount(), runService->getStepTime(), runService->getPeriod()
			);
		}

		m_view->render(m_renderDevice.get());
		m_dataModel->renderPass3dAdorn(m_adorn.get());

		m_renderDevice->push2D();
		m_dataModel->renderPass2d(m_adorn.get(), this);
		m_renderDevice->pop2D();

		RBX::FrameInterpolator::endRender();
		m_viewValid = true;
		if (auto* runService = m_dataModel->find<RBX::RunService>()) {
			runService->runViewValid(this);
		}
	}
	catch (const std::exception& e) {
		LOGE("Render exception: %s", e.what());
	}
	m_renderDevice->popState();
	m_renderDevice->endFrame();
	m_window->swapGLBuffers();
}

std::string AndroidEngine::getMetric(const std::string& metric) const
{
	if (metric == "graphicsMode") {
		return "OpenGLES2";
	}
	return "";
}

} // namespace RBX::Android

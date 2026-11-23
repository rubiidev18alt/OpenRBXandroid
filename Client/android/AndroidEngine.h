#ifndef OPENRBX_ANDROID_ENGINE_H
#define OPENRBX_ANDROID_ENGINE_H

#include "AndroidUserInput.h"
#include "AndroidWindow.h"

#include "util/IMetric.h"
#include "util/RunStateOwner.h"
#include "util/UIEvent.h"

#include <G3D/Stopwatch.h>
#include <GLG3D/RenderDevice.h>
#include <android/native_window.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace RBX {
class AdornG3D;
class DataModel;
namespace View {
class View;
}
} // namespace RBX

namespace RBX::Android {

struct AvatarConfig {
	std::string playerName = "Player";
	int headColor = 24;      // Bright yellow
	int torsoColor = 23;     // Bright blue
	int leftArmColor = 24;   // Bright yellow
	int rightArmColor = 24;  // Bright yellow
	int leftLegColor = 119;  // Br. yellowish green
	int rightLegColor = 119; // Br. yellowish green
	std::string shirtAsset;
	std::string faceAsset;
	std::string hatAsset;
};

class AndroidEngine final : public RBX::IRunView, public RBX::IMetric
{
public:
	static AndroidEngine& instance();

	void initGlobal(const std::string& filesDir, const std::string& cacheDir, const std::string& contentDir);

	void attachSurface(ANativeWindow* window, int width, int height);
	void resizeSurface(int width, int height);
	void detachSurface();

	bool loadPlace(const std::string& rbxlPath);
	void closePlace();
	void respawnCharacter();
	void setAvatarConfig(const AvatarConfig& config);
	AvatarConfig getAvatarConfig() const;

	// Mobile controls
	void setVirtualJoystick(float x, float y);
	void setVirtualJump(bool down);
	void touchDown(float x, float y);
	void touchMove(float x, float y);
	void touchUp(float x, float y);
	void panAndTiltCamera(float deltaXPixels, float deltaYPixels);
	void zoomCamera(float zoomDelta);
	void equipBackpackSlot(int slotIndex);
	std::string getBackpackItemsJson();
	float getLocalHealthRatio();
	void runLuaCommand(const std::string& code);
	void setVisualStyle(int mode);
	void setTimeOfDay(float hours);
	bool toggleFirstPerson();
	void spawnQuickBrick(int brickColorId, bool anchored);
	std::string getStatsJson();

	// Called periodically from render loop / Choreographer frame callback
	void stepAndRender();

	// IRunView
	void InvalidateRunView() override { m_viewValid = false; }
	void AdjustThrottle(double frameRate) override {}

	// IMetric
	std::string getMetric(const std::string& metric) const override;

private:
	AndroidEngine() = default;
	~AndroidEngine() override = default;

	void setupViewAndGuiLocked();
	void applyAvatarToCharacterLocked();

	mutable std::mutex m_engineMutex;
	bool m_globalInitialized = false;
	bool m_viewValid = true;
	bool m_audioInitialized = false;

	std::string m_filesDir;
	std::string m_cacheDir;
	std::string m_contentDir;
	std::string m_currentPlacePath;
	AvatarConfig m_avatar;

	std::unique_ptr<AndroidWindow> m_window;
	std::unique_ptr<G3D::RenderDevice> m_renderDevice;
	std::shared_ptr<RBX::DataModel> m_dataModel;
	std::unique_ptr<RBX::View::View> m_view;
	std::unique_ptr<RBX::AdornG3D> m_adorn;
	std::unique_ptr<AndroidUserInput> m_userInput;

	double m_lastStepTime = 0.0;
	int m_width = 1280;
	int m_height = 720;
};

} // namespace RBX::Android

#endif // OPENRBX_ANDROID_ENGINE_H

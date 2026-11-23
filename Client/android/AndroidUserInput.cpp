#include "AndroidUserInput.h"

#include "rbxgraphics/TextureProxyBase.h"
#include "v8datamodel/DataModel.h"

#include <algorithm>
#include <cmath>

namespace RBX::Android {

AndroidUserInput::AndroidUserInput()
{
	m_keys.fill(false);
}

void AndroidUserInput::setViewportSize(int width, int height)
{
	std::scoped_lock lock(m_mutex);
	m_viewportWidth = std::max(1, width);
	m_viewportHeight = std::max(1, height);
}

void AndroidUserInput::setVirtualJoystick(float x, float y)
{
	std::scoped_lock lock(m_mutex);
	float lenSq = x * x + y * y;
	if (lenSq > 1.0f) {
		float inv = 1.0f / std::sqrt(lenSq);
		x *= inv;
		y *= inv;
	}
	m_virtualJoy.x = x;
	m_virtualJoy.y = y;
}

void AndroidUserInput::setVirtualJump(bool down)
{
	std::scoped_lock lock(m_mutex);
	m_virtualJump = down;
}

void AndroidUserInput::setTouchPosition(float x, float y)
{
	std::scoped_lock lock(m_mutex);
	m_cursorPos.x = x;
	m_cursorPos.y = y;
}

G3D::Vector2 AndroidUserInput::getVirtualJoystick() const
{
	std::scoped_lock lock(m_mutex);
	return m_virtualJoy;
}

bool AndroidUserInput::isVirtualJumpDown() const
{
	std::scoped_lock lock(m_mutex);
	return m_virtualJump;
}

void AndroidUserInput::centerCursor()
{
	std::scoped_lock lock(m_mutex);
	m_cursorPos.x = m_viewportWidth * 0.5f;
	m_cursorPos.y = m_viewportHeight * 0.5f;
}

bool AndroidUserInput::keyDown(SDLKey key) const
{
	std::scoped_lock lock(m_mutex);
	int idx = static_cast<int>(key);
	if (idx >= 0 && idx < static_cast<int>(m_keys.size())) {
		return m_keys[static_cast<size_t>(idx)];
	}
	return false;
}

void AndroidUserInput::setKeyState(SDLKey key, bool down)
{
	std::scoped_lock lock(m_mutex);
	int idx = static_cast<int>(key);
	if (idx >= 0 && idx < static_cast<int>(m_keys.size())) {
		m_keys[static_cast<size_t>(idx)] = down;
	}
}

G3D::Vector2 AndroidUserInput::getCursorPosition()
{
	std::scoped_lock lock(m_mutex);
	return m_cursorPos;
}

G3D::ReferenceCountedPointer<RBX::TextureProxyBase> AndroidUserInput::getGameCursor(RBX::Adorn*)
{
	// No floating cursor icon on mobile touchscreens
	return nullptr;
}

void AndroidUserInput::dispatchMouseEvent(
	RBX::DataModel* dataModel, RBX::UIEvent::EventType type, float x, float y
)
{
	if (!dataModel) {
		return;
	}

	G3D::Vector2int16 winSize;
	{
		std::scoped_lock lock(m_mutex);
		m_cursorPos.x = x;
		m_cursorPos.y = y;
		winSize.x = static_cast<G3D::int16>(m_viewportWidth);
		winSize.y = static_cast<G3D::int16>(m_viewportHeight);
	}

	G3D::Vector2int16 mousePos(static_cast<G3D::int16>(x), static_cast<G3D::int16>(y));
	RBX::UIEvent event(this, type, mousePos, winSize);
	dataModel->processGui(event);
}

void AndroidUserInput::dispatchKeyEvent(RBX::DataModel* dataModel, SDLKey key, bool down)
{
	setKeyState(key, down);
	if (!dataModel) {
		return;
	}

	RBX::UIEvent event(this, down ? RBX::UIEvent::KEY_DOWN : RBX::UIEvent::KEY_UP, key, KMOD_NONE);
	dataModel->processGui(event);
}

} // namespace RBX::Android

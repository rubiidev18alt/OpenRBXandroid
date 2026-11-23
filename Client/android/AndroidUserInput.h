#ifndef OPENRBX_ANDROID_USERINPUT_H
#define OPENRBX_ANDROID_USERINPUT_H

#include "util/UIEvent.h"
#include "util/UserInputBase.h"

#include <G3D/Vector2.h>
#include <array>
#include <mutex>

namespace RBX {
class DataModel;
}

namespace RBX::Android {

class AndroidUserInput final : public RBX::UserInputBase
{
public:
	AndroidUserInput();
	~AndroidUserInput() = default;

	void setViewportSize(int width, int height);
	void setVirtualJoystick(float x, float y);
	void setVirtualJump(bool down);
	void setTouchPosition(float x, float y);

	G3D::Vector2 getVirtualJoystick() const override;
	bool isVirtualJumpDown() const override;

	void setWrapMode(WrapMode wrapMode) override { m_wrapMode = wrapMode; }
	WrapMode getWrapMode() const override { return m_wrapMode; }
	void centerCursor() override;
	bool keyDown(SDLKey key) const override;
	void setKeyState(SDLKey key, bool down) override;

	void dispatchMouseEvent(RBX::DataModel* dataModel, RBX::UIEvent::EventType type, float x, float y);
	void dispatchKeyEvent(RBX::DataModel* dataModel, SDLKey key, bool down);

protected:
	G3D::Vector2 getCursorPosition() override;
	G3D::ReferenceCountedPointer<RBX::TextureProxyBase> getGameCursor(RBX::Adorn* adorn) override;

private:
	mutable std::mutex m_mutex;
	WrapMode m_wrapMode = WRAP_AUTO;
	int m_viewportWidth = 1280;
	int m_viewportHeight = 720;
	G3D::Vector2 m_cursorPos{640.0f, 360.0f};
	G3D::Vector2 m_virtualJoy{0.0f, 0.0f};
	bool m_virtualJump = false;
	std::array<bool, 512> m_keys{};
};

} // namespace RBX::Android

#endif // OPENRBX_ANDROID_USERINPUT_H

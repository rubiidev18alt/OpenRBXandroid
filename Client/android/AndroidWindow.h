#ifndef OPENRBX_ANDROID_WINDOW_H
#define OPENRBX_ANDROID_WINDOW_H

#include <EGL/egl.h>
#include <G3D/Rect2D.h>
#include <GLG3D/GWindow.h>
#include <android/native_window.h>
#include <string>

namespace RBX::Android {

class AndroidWindow final : public G3D::GWindow
{
public:
	AndroidWindow(ANativeWindow* window, int width, int height);
	~AndroidWindow() override;

	bool attachSurface(ANativeWindow* window, int width, int height);
	void detachSurface();
	bool hasValidSurface() const { return m_surface != EGL_NO_SURFACE && m_context != EGL_NO_CONTEXT; }

	void getSettings(GWindow::Settings& settings) const override;
	int width() const override { return m_settings.width; }
	int height() const override { return m_settings.height; }
	G3D::Rect2D dimensions() const override;
	void setDimensions(const G3D::Rect2D& dims) override;
	void setPosition(int x, int y) override;
	bool hasFocus() const override { return m_focused; }
	void setFocused(bool focused) { m_focused = focused; }
	std::string getAPIVersion() const override { return "EGL 1.4 / GLES 2.0"; }
	std::string getAPIName() const override { return "Android EGL"; }
	void setGammaRamp(const G3D::Array<G3D::uint16>& gammaRamp) override;
	void setCaption(const std::string& caption) override { m_caption = caption; }
	int numJoysticks() const override { return 0; }
	std::string joystickName(unsigned int sticknum) override { return ""; }
	std::string caption() override { return m_caption; }
	void swapGLBuffers() override;
	void notifyResize(int w, int h) override;
	void setRelativeMousePosition(double x, double y) override;
	void setRelativeMousePosition(const G3D::Vector2& p) override;
	void getRelativeMouseState(G3D::Vector2& position, G3D::uint8& mouseButtons) const override;
	void getRelativeMouseState(int& x, int& y, G3D::uint8& mouseButtons) const override;
	void getRelativeMouseState(double& x, double& y, G3D::uint8& mouseButtons) const override;
	void getJoystickState(unsigned int stickNum, G3D::Array<float>& axis, G3D::Array<bool>& button) override;
	void setInputCapture(bool c) override { m_inputCapture = c; }
	bool inputCapture() const override { return m_inputCapture; }
	void setMouseVisible(bool b) override { m_mouseVisible = b; }
	bool mouseVisible() const override { return m_mouseVisible; }

	void setCursorPos(float x, float y, G3D::uint8 buttons)
	{
		m_cursorX = x;
		m_cursorY = y;
		m_mouseButtons = buttons;
	}

protected:
	void reallyMakeCurrent() const override;

private:
	bool initDisplayAndContext();

	EGLDisplay m_display = EGL_NO_DISPLAY;
	EGLConfig m_config = nullptr;
	EGLContext m_context = EGL_NO_CONTEXT;
	EGLSurface m_surface = EGL_NO_SURFACE;
	ANativeWindow* m_nativeWindow = nullptr;

	GWindow::Settings m_settings;
	std::string m_caption = "OpenRBX";
	bool m_focused = true;
	bool m_inputCapture = false;
	bool m_mouseVisible = false;
	float m_cursorX = 0.0f;
	float m_cursorY = 0.0f;
	G3D::uint8 m_mouseButtons = 0;
};

} // namespace RBX::Android

#endif // OPENRBX_ANDROID_WINDOW_H

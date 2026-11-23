#include "AndroidWindow.h"

#include <android/log.h>
#include <stdexcept>

extern "C" void rbx_gl_es_reset_state(void);

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "OpenRBX-Window", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "OpenRBX-Window", __VA_ARGS__)

namespace RBX::Android {

AndroidWindow::AndroidWindow(ANativeWindow* window, int width, int height)
{
	m_settings.width = width > 0 ? width : 1280;
	m_settings.height = height > 0 ? height : 720;
	m_settings.rgbBits = 8;
	m_settings.alphaBits = 8;
	m_settings.depthBits = 24;
	m_settings.stencilBits = 8;
	m_settings.fullScreen = true;
	m_settings.framed = false;
	m_settings.resizable = true;

	if (!initDisplayAndContext()) {
		throw std::runtime_error("Failed to initialize Android EGL display/context");
	}
	if (window) {
		if (!attachSurface(window, m_settings.width, m_settings.height)) {
			throw std::runtime_error("Failed to attach Android EGL window surface");
		}
	}
}

AndroidWindow::~AndroidWindow()
{
	detachSurface();
	if (m_display != EGL_NO_DISPLAY) {
		if (m_context != EGL_NO_CONTEXT) {
			eglDestroyContext(m_display, m_context);
			m_context = EGL_NO_CONTEXT;
		}
		eglTerminate(m_display);
		m_display = EGL_NO_DISPLAY;
	}
}

bool AndroidWindow::initDisplayAndContext()
{
	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (m_display == EGL_NO_DISPLAY) {
		LOGE("eglGetDisplay failed");
		return false;
	}
	EGLint major = 0, minor = 0;
	if (!eglInitialize(m_display, &major, &minor)) {
		LOGE("eglInitialize failed: 0x%x", eglGetError());
		return false;
	}
	eglBindAPI(EGL_OPENGL_ES_API);

	const EGLint candidates[][19] = {
		{EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
		 EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_STENCIL_SIZE, 8, EGL_NONE},
		{EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
		 EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_STENCIL_SIZE, 8, EGL_NONE},
		{EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
		 EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_NONE},
	};

	bool chosen = false;
	for (const auto& attribs : candidates) {
		EGLint numConfigs = 0;
		if (eglChooseConfig(m_display, attribs, &m_config, 1, &numConfigs) && numConfigs > 0) {
			chosen = true;
			break;
		}
	}
	if (!chosen) {
		LOGE("eglChooseConfig failed");
		return false;
	}

	EGLint depth = 16, stencil = 0;
	eglGetConfigAttrib(m_display, m_config, EGL_DEPTH_SIZE, &depth);
	eglGetConfigAttrib(m_display, m_config, EGL_STENCIL_SIZE, &stencil);
	m_settings.depthBits = depth;
	m_settings.stencilBits = stencil;

	const EGLint ctxAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
	m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, ctxAttribs);
	if (m_context == EGL_NO_CONTEXT) {
		LOGE("eglCreateContext failed: 0x%x", eglGetError());
		return false;
	}
	rbx_gl_es_reset_state();
	LOGI("EGL initialized (depth=%d stencil=%d)", depth, stencil);
	return true;
}

bool AndroidWindow::attachSurface(ANativeWindow* window, int width, int height)
{
	detachSurface();
	m_nativeWindow = window;
	if (!m_nativeWindow) {
		return false;
	}

	EGLint format = 0;
	if (eglGetConfigAttrib(m_display, m_config, EGL_NATIVE_VISUAL_ID, &format)) {
		ANativeWindow_setBuffersGeometry(m_nativeWindow, 0, 0, format);
	}

	m_surface = eglCreateWindowSurface(m_display, m_config, m_nativeWindow, nullptr);
	if (m_surface == EGL_NO_SURFACE) {
		LOGE("eglCreateWindowSurface failed: 0x%x", eglGetError());
		return false;
	}

	m_settings.width = width > 0 ? width : ANativeWindow_getWidth(m_nativeWindow);
	m_settings.height = height > 0 ? height : ANativeWindow_getHeight(m_nativeWindow);
	reallyMakeCurrent();
	eglSwapInterval(m_display, 1);
	return true;
}

void AndroidWindow::detachSurface()
{
	if (m_display != EGL_NO_DISPLAY) {
		if (eglGetCurrentContext() == m_context || eglGetCurrentSurface(EGL_DRAW) == m_surface) {
			eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		}
		if (m_surface != EGL_NO_SURFACE) {
			eglDestroySurface(m_display, m_surface);
			m_surface = EGL_NO_SURFACE;
		}
	}
	m_nativeWindow = nullptr;
}

void AndroidWindow::reallyMakeCurrent() const
{
	if (m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE && m_context != EGL_NO_CONTEXT) {
		if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context)) {
			LOGE("eglMakeCurrent failed: 0x%x", eglGetError());
		}
	}
}

void AndroidWindow::getSettings(GWindow::Settings& settings) const
{
	settings = m_settings;
}

G3D::Rect2D AndroidWindow::dimensions() const
{
	return G3D::Rect2D::xywh(0.0f, 0.0f, static_cast<float>(m_settings.width), static_cast<float>(m_settings.height));
}

void AndroidWindow::setDimensions(const G3D::Rect2D& dims)
{
	m_settings.width = static_cast<int>(dims.width());
	m_settings.height = static_cast<int>(dims.height());
}

void AndroidWindow::setPosition(int x, int y)
{
	m_settings.x = x;
	m_settings.y = y;
}

void AndroidWindow::setGammaRamp(const G3D::Array<G3D::uint16>&) {}

void AndroidWindow::swapGLBuffers()
{
	if (m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE) {
		eglSwapBuffers(m_display, m_surface);
	}
}

void AndroidWindow::notifyResize(int w, int h)
{
	if (w > 0 && h > 0) {
		m_settings.width = w;
		m_settings.height = h;
	}
}

void AndroidWindow::setRelativeMousePosition(double x, double y)
{
	m_cursorX = static_cast<float>(x);
	m_cursorY = static_cast<float>(y);
}

void AndroidWindow::setRelativeMousePosition(const G3D::Vector2& p)
{
	m_cursorX = p.x;
	m_cursorY = p.y;
}

void AndroidWindow::getRelativeMouseState(G3D::Vector2& position, G3D::uint8& mouseButtons) const
{
	position.x = m_cursorX;
	position.y = m_cursorY;
	mouseButtons = m_mouseButtons;
}

void AndroidWindow::getRelativeMouseState(int& x, int& y, G3D::uint8& mouseButtons) const
{
	x = static_cast<int>(m_cursorX);
	y = static_cast<int>(m_cursorY);
	mouseButtons = m_mouseButtons;
}

void AndroidWindow::getRelativeMouseState(double& x, double& y, G3D::uint8& mouseButtons) const
{
	x = m_cursorX;
	y = m_cursorY;
	mouseButtons = m_mouseButtons;
}

void AndroidWindow::getJoystickState(unsigned int, G3D::Array<float>& axis, G3D::Array<bool>& button)
{
	axis.clear();
	button.clear();
}

} // namespace RBX::Android

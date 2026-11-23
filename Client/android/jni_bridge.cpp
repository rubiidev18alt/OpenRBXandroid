#include "AndroidEngine.h"

#include <android/native_window_jni.h>
#include <jni.h>
#include <string>

namespace {

std::string jstringToStdString(JNIEnv* env, jstring jstr)
{
	if (!jstr) {
		return {};
	}
	const char* utf = env->GetStringUTFChars(jstr, nullptr);
	if (!utf) {
		return {};
	}
	std::string out(utf);
	env->ReleaseStringUTFChars(jstr, utf);
	return out;
}

} // namespace

extern "C" {

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeInitGlobal(
	JNIEnv* env, jobject /*thiz*/, jstring filesDir, jstring cacheDir, jstring contentDir
)
{
	RBX::Android::AndroidEngine::instance().initGlobal(
		jstringToStdString(env, filesDir),
		jstringToStdString(env, cacheDir),
		jstringToStdString(env, contentDir)
	);
}

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeOnSurfaceCreated(
	JNIEnv* env, jobject /*thiz*/, jobject surface, jint width, jint height
)
{
	ANativeWindow* window = surface ? ANativeWindow_fromSurface(env, surface) : nullptr;
	if (!window) {
		return;
	}
	RBX::Android::AndroidEngine::instance().attachSurface(window, width, height);
	ANativeWindow_release(window);
}

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeOnSurfaceChanged(
	JNIEnv* /*env*/, jobject /*thiz*/, jint width, jint height
)
{
	RBX::Android::AndroidEngine::instance().resizeSurface(width, height);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeOnSurfaceDestroyed(JNIEnv* /*env*/, jobject /*thiz*/)
{
	RBX::Android::AndroidEngine::instance().detachSurface();
}

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeStepAndRender(JNIEnv* /*env*/, jobject /*thiz*/)
{
	RBX::Android::AndroidEngine::instance().stepAndRender();
}

JNIEXPORT jboolean JNICALL
Java_com_openrbx_android_NativeEngine_nativeLoadPlace(JNIEnv* env, jobject /*thiz*/, jstring rbxlPath)
{
	bool ok = RBX::Android::AndroidEngine::instance().loadPlace(jstringToStdString(env, rbxlPath));
	return ok ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeClosePlace(JNIEnv* /*env*/, jobject /*thiz*/)
{
	RBX::Android::AndroidEngine::instance().closePlace();
}

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeRespawn(JNIEnv* /*env*/, jobject /*thiz*/)
{
	RBX::Android::AndroidEngine::instance().respawnCharacter();
}

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeSetAvatar(
	JNIEnv* env,
	jobject /*thiz*/,
	jstring name,
	jint headColor,
	jint torsoColor,
	jint leftArmColor,
	jint rightArmColor,
	jint leftLegColor,
	jint rightLegColor,
	jstring shirtAsset,
	jstring faceAsset,
	jstring hatAsset
)
{
	RBX::Android::AvatarConfig cfg;
	cfg.playerName = jstringToStdString(env, name);
	cfg.headColor = headColor;
	cfg.torsoColor = torsoColor;
	cfg.leftArmColor = leftArmColor;
	cfg.rightArmColor = rightArmColor;
	cfg.leftLegColor = leftLegColor;
	cfg.rightLegColor = rightLegColor;
	cfg.shirtAsset = jstringToStdString(env, shirtAsset);
	cfg.faceAsset = jstringToStdString(env, faceAsset);
	cfg.hatAsset = jstringToStdString(env, hatAsset);
	RBX::Android::AndroidEngine::instance().setAvatarConfig(cfg);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeSetVirtualJoystick(JNIEnv* /*env*/, jobject /*thiz*/, jfloat x, jfloat y)
{
	RBX::Android::AndroidEngine::instance().setVirtualJoystick(x, y);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeSetVirtualJump(JNIEnv* /*env*/, jobject /*thiz*/, jboolean down)
{
	RBX::Android::AndroidEngine::instance().setVirtualJump(down == JNI_TRUE);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeTouchDown(JNIEnv* /*env*/, jobject /*thiz*/, jfloat x, jfloat y)
{
	RBX::Android::AndroidEngine::instance().touchDown(x, y);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeTouchMove(JNIEnv* /*env*/, jobject /*thiz*/, jfloat x, jfloat y)
{
	RBX::Android::AndroidEngine::instance().touchMove(x, y);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeTouchUp(JNIEnv* /*env*/, jobject /*thiz*/, jfloat x, jfloat y)
{
	RBX::Android::AndroidEngine::instance().touchUp(x, y);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativePanAndTiltCamera(JNIEnv* /*env*/, jobject /*thiz*/, jfloat dx, jfloat dy)
{
	RBX::Android::AndroidEngine::instance().panAndTiltCamera(dx, dy);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeZoomCamera(JNIEnv* /*env*/, jobject /*thiz*/, jfloat delta)
{
	RBX::Android::AndroidEngine::instance().zoomCamera(delta);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeEquipBackpackSlot(JNIEnv* /*env*/, jobject /*thiz*/, jint slotIndex)
{
	RBX::Android::AndroidEngine::instance().equipBackpackSlot(slotIndex);
}

JNIEXPORT jstring JNICALL
Java_com_openrbx_android_NativeEngine_nativeGetBackpackItemsJson(JNIEnv* env, jobject /*thiz*/)
{
	std::string json = RBX::Android::AndroidEngine::instance().getBackpackItemsJson();
	return env->NewStringUTF(json.c_str());
}

JNIEXPORT jfloat JNICALL
Java_com_openrbx_android_NativeEngine_nativeGetLocalHealthRatio(JNIEnv* /*env*/, jobject /*thiz*/)
{
	return RBX::Android::AndroidEngine::instance().getLocalHealthRatio();
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeRunLua(JNIEnv* env, jobject /*thiz*/, jstring code)
{
	RBX::Android::AndroidEngine::instance().runLuaCommand(jstringToStdString(env, code));
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeSetVisualStyle(JNIEnv* /*env*/, jobject /*thiz*/, jint mode)
{
	RBX::Android::AndroidEngine::instance().setVisualStyle(mode);
}

JNIEXPORT void JNICALL
Java_com_openrbx_android_NativeEngine_nativeSetTimeOfDay(JNIEnv* /*env*/, jobject /*thiz*/, jfloat hours)
{
	RBX::Android::AndroidEngine::instance().setTimeOfDay(hours);
}

JNIEXPORT jboolean JNICALL
Java_com_openrbx_android_NativeEngine_nativeToggleFirstPerson(JNIEnv* /*env*/, jobject /*thiz*/)
{
	return RBX::Android::AndroidEngine::instance().toggleFirstPerson() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_com_openrbx_android_NativeEngine_nativeSpawnQuickBrick(
	JNIEnv* /*env*/, jobject /*thiz*/, jint brickColorId, jboolean anchored
)
{
	RBX::Android::AndroidEngine::instance().spawnQuickBrick(brickColorId, anchored == JNI_TRUE);
}

JNIEXPORT jstring JNICALL Java_com_openrbx_android_NativeEngine_nativeGetStatsJson(JNIEnv* env, jobject /*thiz*/)
{
	std::string json = RBX::Android::AndroidEngine::instance().getStatsJson();
	return env->NewStringUTF(json.c_str());
}

} // extern "C"

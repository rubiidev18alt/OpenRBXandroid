package com.openrbx.android

import android.view.Surface

object NativeEngine {
    init {
        System.loadLibrary("openal")
        System.loadLibrary("openrbx_android")
    }

    external fun nativeInitGlobal(filesDir: String, cacheDir: String, contentDir: String)
    external fun nativeOnSurfaceCreated(surface: Surface, width: Int, height: Int)
    external fun nativeOnSurfaceChanged(width: Int, height: Int)
    external fun nativeOnSurfaceDestroyed()
    external fun nativeStepAndRender()

    external fun nativeLoadPlace(rbxlPath: String): Boolean
    external fun nativeClosePlace()
    external fun nativeRespawn()

    external fun nativeSetAvatar(
        name: String,
        headColor: Int,
        torsoColor: Int,
        leftArmColor: Int,
        rightArmColor: Int,
        leftLegColor: Int,
        rightLegColor: Int,
        shirtAsset: String,
        faceAsset: String,
        hatAsset: String
    )

    external fun nativeSetVirtualJoystick(x: Float, y: Float)
    external fun nativeSetVirtualJump(down: Boolean)
    external fun nativeTouchDown(x: Float, y: Float)
    external fun nativeTouchMove(x: Float, y: Float)
    external fun nativeTouchUp(x: Float, y: Float)
    external fun nativePanAndTiltCamera(dx: Float, dy: Float)
    external fun nativeZoomCamera(delta: Float)
    external fun nativeEquipBackpackSlot(slotIndex: Int)
    external fun nativeGetBackpackItemsJson(): String
    external fun nativeGetLocalHealthRatio(): Float
    external fun nativeRunLua(code: String)

    external fun nativeSetVisualStyle(mode: Int)
    external fun nativeSetTimeOfDay(hours: Float)
    external fun nativeToggleFirstPerson(): Boolean
    external fun nativeSpawnQuickBrick(brickColorId: Int, anchored: Boolean)
    external fun nativeGetStatsJson(): String
}

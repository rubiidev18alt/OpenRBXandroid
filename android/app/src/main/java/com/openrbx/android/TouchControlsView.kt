package com.openrbx.android

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Path
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import kotlin.math.hypot
import kotlin.math.min

class TouchControlsView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : View(context, attrs) {

    interface Listener {
        fun onJoystickChanged(x: Float, y: Float)
        fun onJumpChanged(down: Boolean)
        fun onCameraPanTilt(dx: Float, dy: Float)
        fun onCameraZoom(delta: Float)
        fun onWorldTap(x: Float, y: Float)
    }

    var listener: Listener? = null

    private val density = resources.displayMetrics.density
    private val outerRadius = 64f * density
    private val knobRadius = 26f * density
    private val jumpRadius = 42f * density
    private val touchSlop = 10f * density

    private var defaultJoyCenterX = 0f
    private var defaultJoyCenterY = 0f
    private var joyCenterX = 0f
    private var joyCenterY = 0f
    private var joyKnobX = 0f
    private var joyKnobY = 0f

    private var jumpCenterX = 0f
    private var jumpCenterY = 0f

    private var joyPointerId = MotionEvent.INVALID_POINTER_ID
    private var jumpPointerId = MotionEvent.INVALID_POINTER_ID
    private var cameraPointerId = MotionEvent.INVALID_POINTER_ID
    private var cameraSecondPointerId = MotionEvent.INVALID_POINTER_ID

    private var camDownX = 0f
    private var camDownY = 0f
    private var camLastX = 0f
    private var camLastY = 0f
    private var camMoved = false
    private var prevPinchDist = 0f

    private val joyBasePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.argb(72, 25, 29, 36)
        style = Paint.Style.FILL
    }
    private val joyRingPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.argb(145, 255, 255, 255)
        style = Paint.Style.STROKE
        strokeWidth = 2.5f * density
    }
    private val joyKnobPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.argb(190, 235, 240, 248)
        style = Paint.Style.FILL
    }
    private val jumpFillPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.argb(86, 25, 29, 36)
        style = Paint.Style.FILL
    }
    private val jumpFillActivePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.argb(150, 0, 162, 255)
        style = Paint.Style.FILL
    }
    private val jumpArrowPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.argb(225, 255, 255, 255)
        style = Paint.Style.FILL
    }
    private val arrowPath = Path()

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        defaultJoyCenterX = 28f * density + outerRadius
        defaultJoyCenterY = h - (24f * density + outerRadius)
        resetJoystick()

        jumpCenterX = w - (28f * density + jumpRadius)
        jumpCenterY = h - (24f * density + jumpRadius)
    }

    private fun resetJoystick() {
        joyCenterX = defaultJoyCenterX
        joyCenterY = defaultJoyCenterY
        joyKnobX = joyCenterX
        joyKnobY = joyCenterY
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        // Classic 2016 Roblox mobile translucent thumbstick
        canvas.drawCircle(joyCenterX, joyCenterY, outerRadius, joyBasePaint)
        canvas.drawCircle(joyCenterX, joyCenterY, outerRadius, joyRingPaint)
        canvas.drawCircle(joyKnobX, joyKnobY, knobRadius, joyKnobPaint)

        // Classic 2016 circular Jump button with upward arrow
        val isJumping = jumpPointerId != MotionEvent.INVALID_POINTER_ID
        canvas.drawCircle(
            jumpCenterX,
            jumpCenterY,
            jumpRadius,
            if (isJumping) jumpFillActivePaint else jumpFillPaint
        )
        canvas.drawCircle(jumpCenterX, jumpCenterY, jumpRadius, joyRingPaint)

        val arrowW = 14f * density
        val arrowH = 16f * density
        arrowPath.reset()
        arrowPath.moveTo(jumpCenterX, jumpCenterY - arrowH * 0.7f)
        arrowPath.lineTo(jumpCenterX + arrowW, jumpCenterY + arrowH * 0.15f)
        arrowPath.lineTo(jumpCenterX + arrowW * 0.42f, jumpCenterY + arrowH * 0.15f)
        arrowPath.lineTo(jumpCenterX + arrowW * 0.42f, jumpCenterY + arrowH * 0.7f)
        arrowPath.lineTo(jumpCenterX - arrowW * 0.42f, jumpCenterY + arrowH * 0.7f)
        arrowPath.lineTo(jumpCenterX - arrowW * 0.42f, jumpCenterY + arrowH * 0.15f)
        arrowPath.lineTo(jumpCenterX - arrowW, jumpCenterY + arrowH * 0.15f)
        arrowPath.close()
        canvas.drawPath(arrowPath, jumpArrowPaint)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val actionMasked = event.actionMasked
        val actionIdx = event.actionIndex
        val pointerId = event.getPointerId(actionIdx)

        when (actionMasked) {
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_POINTER_DOWN -> {
                val x = event.getX(actionIdx)
                val y = event.getY(actionIdx)

                if (jumpPointerId == MotionEvent.INVALID_POINTER_ID &&
                    hypot((x - jumpCenterX).toDouble(), (y - jumpCenterY).toDouble()) <= jumpRadius * 1.25
                ) {
                    jumpPointerId = pointerId
                    listener?.onJumpChanged(true)
                    invalidate()
                    return true
                }

                if (joyPointerId == MotionEvent.INVALID_POINTER_ID &&
                    x < width * 0.38f && y > height * 0.32f
                ) {
                    joyPointerId = pointerId
                    joyCenterX = x.coerceIn(outerRadius + 12f * density, width * 0.34f)
                    joyCenterY = y.coerceIn(height * 0.40f, height - outerRadius - 12f * density)
                    updateJoystickKnob(x, y)
                    invalidate()
                    return true
                }

                if (cameraPointerId == MotionEvent.INVALID_POINTER_ID) {
                    cameraPointerId = pointerId
                    camDownX = x
                    camDownY = y
                    camLastX = x
                    camLastY = y
                    camMoved = false
                } else if (cameraSecondPointerId == MotionEvent.INVALID_POINTER_ID) {
                    cameraSecondPointerId = pointerId
                    camMoved = true
                    prevPinchDist = distanceBetweenPointers(event, cameraPointerId, cameraSecondPointerId)
                }
                return true
            }

            MotionEvent.ACTION_MOVE -> {
                if (joyPointerId != MotionEvent.INVALID_POINTER_ID) {
                    val idx = event.findPointerIndex(joyPointerId)
                    if (idx >= 0) {
                        updateJoystickKnob(event.getX(idx), event.getY(idx))
                    }
                }

                if (cameraPointerId != MotionEvent.INVALID_POINTER_ID &&
                    cameraSecondPointerId != MotionEvent.INVALID_POINTER_ID
                ) {
                    val dist = distanceBetweenPointers(event, cameraPointerId, cameraSecondPointerId)
                    if (prevPinchDist > 0f && dist > 0f) {
                        val delta = (dist - prevPinchDist) / (28f * density)
                        if (kotlin.math.abs(delta) > 0.02f) {
                            listener?.onCameraZoom(delta)
                            prevPinchDist = dist
                        }
                    }
                } else if (cameraPointerId != MotionEvent.INVALID_POINTER_ID) {
                    val idx = event.findPointerIndex(cameraPointerId)
                    if (idx >= 0) {
                        val x = event.getX(idx)
                        val y = event.getY(idx)
                        if (!camMoved && hypot((x - camDownX).toDouble(), (y - camDownY).toDouble()) > touchSlop) {
                            camMoved = true
                        }
                        if (camMoved) {
                            listener?.onCameraPanTilt(x - camLastX, y - camLastY)
                        }
                        camLastX = x
                        camLastY = y
                    }
                }
                invalidate()
                return true
            }

            MotionEvent.ACTION_UP, MotionEvent.ACTION_POINTER_UP, MotionEvent.ACTION_CANCEL -> {
                if (pointerId == joyPointerId || actionMasked == MotionEvent.ACTION_CANCEL) {
                    joyPointerId = MotionEvent.INVALID_POINTER_ID
                    resetJoystick()
                    listener?.onJoystickChanged(0f, 0f)
                }
                if (pointerId == jumpPointerId || actionMasked == MotionEvent.ACTION_CANCEL) {
                    jumpPointerId = MotionEvent.INVALID_POINTER_ID
                    listener?.onJumpChanged(false)
                }
                if (pointerId == cameraSecondPointerId || actionMasked == MotionEvent.ACTION_CANCEL) {
                    cameraSecondPointerId = MotionEvent.INVALID_POINTER_ID
                    prevPinchDist = 0f
                }
                if (pointerId == cameraPointerId || actionMasked == MotionEvent.ACTION_CANCEL) {
                    if (actionMasked != MotionEvent.ACTION_CANCEL && !camMoved) {
                        listener?.onWorldTap(camLastX, camLastY)
                    }
                    cameraPointerId = cameraSecondPointerId
                    cameraSecondPointerId = MotionEvent.INVALID_POINTER_ID
                    if (cameraPointerId != MotionEvent.INVALID_POINTER_ID) {
                        val nextIdx = event.findPointerIndex(cameraPointerId)
                        if (nextIdx >= 0) {
                            camLastX = event.getX(nextIdx)
                            camLastY = event.getY(nextIdx)
                        }
                    }
                    prevPinchDist = 0f
                }
                invalidate()
                return true
            }
        }
        return super.onTouchEvent(event)
    }

    private fun updateJoystickKnob(x: Float, y: Float) {
        val dx = x - joyCenterX
        val dy = y - joyCenterY
        val maxDist = outerRadius - knobRadius * 0.25f
        val dist = hypot(dx.toDouble(), dy.toDouble()).toFloat()
        if (dist > maxDist && dist > 0f) {
            val scale = maxDist / dist
            joyKnobX = joyCenterX + dx * scale
            joyKnobY = joyCenterY + dy * scale
        } else {
            joyKnobX = x
            joyKnobY = y
        }

        val normX = ((joyKnobX - joyCenterX) / maxDist).coerceIn(-1f, 1f)
        // Screen Y increases downward, forward walk is +Y
        val normY = (-(joyKnobY - joyCenterY) / maxDist).coerceIn(-1f, 1f)
        val deadzone = 0.12f
        val mag = min(1f, hypot(normX.toDouble(), normY.toDouble()).toFloat())
        if (mag < deadzone) {
            listener?.onJoystickChanged(0f, 0f)
        } else {
            listener?.onJoystickChanged(normX, normY)
        }
    }

    private fun distanceBetweenPointers(event: MotionEvent, p1: Int, p2: Int): Float {
        val i1 = event.findPointerIndex(p1)
        val i2 = event.findPointerIndex(p2)
        if (i1 < 0 || i2 < 0) return 0f
        return hypot(
            (event.getX(i2) - event.getX(i1)).toDouble(),
            (event.getY(i2) - event.getY(i1)).toDouble()
        ).toFloat()
    }
}

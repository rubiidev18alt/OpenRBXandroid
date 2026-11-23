package com.openrbx.android

import android.os.Bundle
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.ProgressBar
import android.widget.SeekBar
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import org.json.JSONArray
import org.json.JSONObject
import java.util.concurrent.ConcurrentLinkedQueue

class GameActivity : AppCompatActivity(), SurfaceHolder.Callback {

    companion object {
        const val EXTRA_PLACE_PATH = "extra_place_path"
    }

    private lateinit var surfaceView: SurfaceView
    private lateinit var touchControls: TouchControlsView
    private lateinit var healthBar: ProgressBar
    private lateinit var tvStats: TextView
    private lateinit var backpackBar: LinearLayout
    private lateinit var modernPanel: LinearLayout
    private lateinit var btnCameraMode: Button
    private lateinit var btnStyle2007: Button
    private lateinit var btnStyleModern: Button
    private lateinit var btnStyleTwilight: Button
    private lateinit var tvTimeOfDay: TextView
    private lateinit var seekTimeOfDay: SeekBar
    private lateinit var etLuaCommand: EditText

    private var placePath: String = ""
    private var surfaceReady = false
    private var placeLoaded = false

    @Volatile
    private var renderRunning = false
    private var renderThread: Thread? = null
    private val glQueue = ConcurrentLinkedQueue<Runnable>()
    private var lastBackpackJson = ""
    private var lastHudTickMs = 0L
    private var currentStyle = 0
    private val quickColors = intArrayOf(21, 23, 24, 28, 106, 104, 194)
    private var quickColorIndex = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_game)
        hideSystemBars()

        val paths = AssetInstaller.ensureInstalled(this)
        NativeEngine.nativeInitGlobal(paths.filesDir, paths.cacheDir, paths.contentDir)
        AvatarPreferences.pushToNative(AvatarPreferences.load(this))

        placePath = intent.getStringExtra(EXTRA_PLACE_PATH) ?: paths.crossroadsPath

        surfaceView = findViewById(R.id.gameSurfaceView)
        touchControls = findViewById(R.id.touchControlsView)
        healthBar = findViewById(R.id.healthBar)
        tvStats = findViewById(R.id.tvStats)
        backpackBar = findViewById(R.id.backpackBar)
        modernPanel = findViewById(R.id.modernPanel)
        btnCameraMode = findViewById(R.id.btnCameraMode)
        btnStyle2007 = findViewById(R.id.btnStyle2007)
        btnStyleModern = findViewById(R.id.btnStyleModern)
        btnStyleTwilight = findViewById(R.id.btnStyleTwilight)
        tvTimeOfDay = findViewById(R.id.tvTimeOfDay)
        seekTimeOfDay = findViewById(R.id.seekTimeOfDay)
        etLuaCommand = findViewById(R.id.etLuaCommand)

        surfaceView.holder.addCallback(this)

        touchControls.listener = object : TouchControlsView.Listener {
            override fun onJoystickChanged(x: Float, y: Float) {
                glQueue.offer(Runnable { NativeEngine.nativeSetVirtualJoystick(x, y) })
            }

            override fun onJumpChanged(down: Boolean) {
                glQueue.offer(Runnable { NativeEngine.nativeSetVirtualJump(down) })
            }

            override fun onCameraPanTilt(dx: Float, dy: Float) {
                glQueue.offer(Runnable { NativeEngine.nativePanAndTiltCamera(dx, dy) })
            }

            override fun onCameraZoom(delta: Float) {
                glQueue.offer(Runnable { NativeEngine.nativeZoomCamera(delta) })
            }

            override fun onWorldTap(x: Float, y: Float) {
                glQueue.offer(Runnable {
                    NativeEngine.nativeTouchDown(x, y)
                    NativeEngine.nativeTouchUp(x, y)
                })
            }
        }

        findViewById<Button>(R.id.btnLeave).setOnClickListener {
            glQueue.offer(Runnable { NativeEngine.nativeClosePlace() })
            finish()
        }

        findViewById<Button>(R.id.btnRespawn).setOnClickListener {
            glQueue.offer(Runnable { NativeEngine.nativeRespawn() })
        }

        btnCameraMode.setOnClickListener {
            glQueue.offer(Runnable {
                val fp = NativeEngine.nativeToggleFirstPerson()
                runOnUiThread {
                    btnCameraMode.text = if (fp) "1P" else "3P"
                }
            })
        }

        findViewById<Button>(R.id.btnSpawnBrick).setOnClickListener {
            val colorId = quickColors[quickColorIndex % quickColors.size]
            quickColorIndex++
            glQueue.offer(Runnable { NativeEngine.nativeSpawnQuickBrick(colorId, false) })
        }

        findViewById<Button>(R.id.btnToggleModernPanel).setOnClickListener {
            modernPanel.visibility = if (modernPanel.visibility == View.VISIBLE) View.GONE else View.VISIBLE
        }

        btnStyle2007.setOnClickListener { setVisualStyle(0) }
        btnStyleModern.setOnClickListener { setVisualStyle(1) }
        btnStyleTwilight.setOnClickListener { setVisualStyle(2) }

        seekTimeOfDay.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(sb: SeekBar?, progress: Int, fromUser: Boolean) {
                tvTimeOfDay.text = String.format("Sun Time: %02d:00", progress)
                if (fromUser) {
                    val h = progress.toFloat()
                    glQueue.offer(Runnable { NativeEngine.nativeSetTimeOfDay(h) })
                }
            }
            override fun onStartTrackingTouch(sb: SeekBar?) {}
            override fun onStopTrackingTouch(sb: SeekBar?) {}
        })

        findViewById<Button>(R.id.btnRunLua).setOnClickListener {
            val code = etLuaCommand.text.toString().trim()
            if (code.isNotEmpty()) {
                glQueue.offer(Runnable { NativeEngine.nativeRunLua(code) })
                etLuaCommand.setText("")
            }
        }
    }

    private fun setVisualStyle(mode: Int) {
        currentStyle = mode
        glQueue.offer(Runnable { NativeEngine.nativeSetVisualStyle(mode) })
        val buttons = listOf(btnStyle2007 to 0, btnStyleModern to 1, btnStyleTwilight to 2)
        for ((btn, id) in buttons) {
            val active = id == mode
            btn.setTextColor(if (active) 0xFFFFFFFF.toInt() else 0xFF1F2B3D.toInt())
            btn.setBackgroundResource(
                if (active) R.drawable.bg_retro_2007_button else R.drawable.bg_retro_2007_slot
            )
        }
    }

    private fun hideSystemBars() {
        WindowCompat.setDecorFitsSystemWindows(window, false)
        val controller = WindowCompat.getInsetsController(window, window.decorView)
        controller.systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        controller.hide(WindowInsetsCompat.Type.systemBars())
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        surfaceReady = true
        startRenderLoop(holder)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        glQueue.offer(Runnable {
            NativeEngine.nativeOnSurfaceChanged(width, height)
        })
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        surfaceReady = false
        stopRenderLoop()
    }

    private fun startRenderLoop(holder: SurfaceHolder) {
        if (renderRunning) return
        renderRunning = true
        renderThread = Thread({
            val rect = holder.surfaceFrame
            NativeEngine.nativeOnSurfaceCreated(holder.surface, rect.width(), rect.height())
            NativeEngine.nativeSetVisualStyle(currentStyle)
            if (!placeLoaded && placePath.isNotEmpty()) {
                val ok = NativeEngine.nativeLoadPlace(placePath)
                placeLoaded = ok
                if (!ok) {
                    runOnUiThread {
                        Toast.makeText(this, "Failed to load place", Toast.LENGTH_LONG).show()
                    }
                }
            }

            while (renderRunning && surfaceReady) {
                while (true) {
                    val task = glQueue.poll() ?: break
                    task.run()
                }
                NativeEngine.nativeStepAndRender()

                val now = System.currentTimeMillis()
                if (now - lastHudTickMs >= 250L) {
                    lastHudTickMs = now
                    val backpackJson = NativeEngine.nativeGetBackpackItemsJson()
                    val statsJson = NativeEngine.nativeGetStatsJson()
                    runOnUiThread { updateHud(statsJson, backpackJson) }
                }
            }

            NativeEngine.nativeOnSurfaceDestroyed()
        }, "OpenRBX-RenderThread").apply { start() }
    }

    private fun stopRenderLoop() {
        renderRunning = false
        try {
            renderThread?.join(1500L)
        } catch (_: InterruptedException) {
        }
        renderThread = null
    }

    private fun updateHud(statsJson: String, backpackJson: String) {
        try {
            val obj = JSONObject(statsJson)
            val fps = obj.optInt("fps", 0)
            val parts = obj.optString("parts", "0")
            val timeHours = obj.optDouble("timeHours", 14.0)
            val fp = obj.optBoolean("firstPerson", false)
            val health = obj.optDouble("health", 1.0).toFloat()

            val h = timeHours.toInt().coerceIn(0, 23)
            val m = ((timeHours - h) * 60.0).toInt().coerceIn(0, 59)
            tvStats.text = String.format("%d FPS • %s parts • %02d:%02d", fps, parts, h, m)
            btnCameraMode.text = if (fp) "1P" else "3P"
            healthBar.progress = (health * 100f).toInt().coerceIn(0, 100)
        } catch (_: Exception) {
        }

        if (backpackJson == lastBackpackJson) return
        lastBackpackJson = backpackJson

        backpackBar.removeAllViews()
        try {
            val arr = JSONArray(backpackJson)
            for (i in 0 until arr.length()) {
                val obj = arr.getJSONObject(i)
                val slot = obj.getInt("slot")
                val name = obj.getString("name")
                val selected = obj.optBoolean("selected", false)

                val btn = Button(this).apply {
                    text = "${slot + 1}\n$name"
                    isAllCaps = false
                    textSize = 10f
                    setTextColor(if (selected) 0xFFFFFFFF.toInt() else 0xFF1F2B3D.toInt())
                    setBackgroundResource(
                        if (selected) R.drawable.bg_retro_2007_button else R.drawable.bg_retro_2007_slot
                    )
                    setOnClickListener {
                        glQueue.offer(Runnable { NativeEngine.nativeEquipBackpackSlot(slot) })
                    }
                }
                val lp = LinearLayout.LayoutParams(
                    (62 * resources.displayMetrics.density).toInt(),
                    (46 * resources.displayMetrics.density).toInt()
                ).apply {
                    marginEnd = (4 * resources.displayMetrics.density).toInt()
                }
                backpackBar.addView(btn, lp)
            }
        } catch (_: Exception) {
        }
    }

    override fun onDestroy() {
        stopRenderLoop()
        if (isFinishing) {
            NativeEngine.nativeClosePlace()
        }
        super.onDestroy()
    }
}

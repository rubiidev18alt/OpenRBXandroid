package com.openrbx.android

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import org.json.JSONArray
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var installPaths: AssetInstaller.InstallPaths
    private lateinit var tvActiveAvatar: TextView
    private lateinit var tvNoRecent: TextView
    private lateinit var recentContainer: LinearLayout

    private val pickRbxlLauncher = registerForActivityResult(
        ActivityResultContracts.OpenDocument()
    ) { uri: Uri? ->
        if (uri != null) {
            val copied = AssetInstaller.copyUriToImportedPlace(this, uri)
            if (copied != null) {
                addRecentPlace(copied.absolutePath)
                launchPlace(copied.absolutePath)
            } else {
                Toast.makeText(this, "Could not open .rbxl file", Toast.LENGTH_SHORT).show()
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        installPaths = AssetInstaller.ensureInstalled(this)
        NativeEngine.nativeInitGlobal(
            installPaths.filesDir,
            installPaths.cacheDir,
            installPaths.contentDir
        )

        tvActiveAvatar = findViewById(R.id.tvActiveAvatar)
        tvNoRecent = findViewById(R.id.tvNoRecent)
        recentContainer = findViewById(R.id.recentPlacesContainer)

        findViewById<Button>(R.id.btnPlayCrossroads).setOnClickListener {
            launchPlace(installPaths.crossroadsPath)
        }
        findViewById<Button>(R.id.btnPlayHappyHome).setOnClickListener {
            launchPlace(installPaths.happyHomePath)
        }
        findViewById<Button>(R.id.btnPlayStarter).setOnClickListener {
            launchPlace(installPaths.starterPlacePath)
        }
        findViewById<Button>(R.id.btnOpenRbxl).setOnClickListener {
            pickRbxlLauncher.launch(arrayOf("*/*"))
        }
        findViewById<Button>(R.id.btnAvatarEditor).setOnClickListener {
            startActivity(Intent(this, AvatarEditorActivity::class.java))
        }
    }

    override fun onResume() {
        super.onResume()
        val avatar = AvatarPreferences.load(this)
        AvatarPreferences.pushToNative(avatar)
        tvActiveAvatar.text = "${avatar.playerName} • Ready"
        refreshRecentPlaces()
    }

    private fun launchPlace(path: String) {
        val avatar = AvatarPreferences.load(this)
        AvatarPreferences.pushToNative(avatar)
        startActivity(
            Intent(this, GameActivity::class.java).putExtra(GameActivity.EXTRA_PLACE_PATH, path)
        )
    }

    private fun addRecentPlace(path: String) {
        val list = loadRecentList().toMutableList()
        list.remove(path)
        list.add(0, path)
        while (list.size > 8) list.removeLast()
        val arr = JSONArray(list)
        getSharedPreferences("openrbx_recent", MODE_PRIVATE)
            .edit()
            .putString("paths", arr.toString())
            .apply()
    }

    private fun loadRecentList(): List<String> {
        val raw = getSharedPreferences("openrbx_recent", MODE_PRIVATE)
            .getString("paths", "[]") ?: "[]"
        val arr = JSONArray(raw)
        val out = mutableListOf<String>()
        for (i in 0 until arr.length()) {
            val p = arr.optString(i)
            if (p.isNotEmpty() && File(p).exists()) out.add(p)
        }
        return out
    }

    private fun refreshRecentPlaces() {
        recentContainer.removeAllViews()
        val recents = loadRecentList()
        tvNoRecent.visibility = if (recents.isEmpty()) View.VISIBLE else View.GONE

        for (path in recents) {
            val file = File(path)
            val btn = Button(this).apply {
                text = "▶  ${file.nameWithoutExtension}"
                isAllCaps = false
                setTextColor(0xFF1F2B3D.toInt())
                setBackgroundResource(R.drawable.bg_retro_2007_slot)
                textAlignment = View.TEXT_ALIGNMENT_TEXT_START
                setPadding(28, 16, 28, 16)
                setOnClickListener { launchPlace(path) }
            }
            val lp = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
            ).apply {
                bottomMargin = (8 * resources.displayMetrics.density).toInt()
            }
            recentContainer.addView(btn, lp)
        }
    }
}

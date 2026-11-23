package com.openrbx.android

import android.graphics.drawable.GradientDrawable
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.GridLayout
import android.widget.LinearLayout
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity

class AvatarEditorActivity : AppCompatActivity() {

    private enum class BodyPart {
        HEAD, TORSO, LEFT_ARM, RIGHT_ARM, LEFT_LEG, RIGHT_LEG
    }

    private lateinit var profile: AvatarProfile
    private var selectedPart = BodyPart.TORSO

    private lateinit var etPlayerName: EditText
    private lateinit var previewHead: View
    private lateinit var previewTorso: View
    private lateinit var previewLeftArm: View
    private lateinit var previewRightArm: View
    private lateinit var previewLeftLeg: View
    private lateinit var previewRightLeg: View

    private lateinit var bodyPartTabs: LinearLayout
    private lateinit var colorPaletteGrid: GridLayout
    private lateinit var faceChoicesRow: LinearLayout
    private lateinit var shirtChoicesRow: LinearLayout
    private lateinit var hatChoicesRow: LinearLayout

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_avatar_editor)

        profile = AvatarPreferences.load(this)

        etPlayerName = findViewById(R.id.etPlayerName)
        previewHead = findViewById(R.id.previewHead)
        previewTorso = findViewById(R.id.previewTorso)
        previewLeftArm = findViewById(R.id.previewLeftArm)
        previewRightArm = findViewById(R.id.previewRightArm)
        previewLeftLeg = findViewById(R.id.previewLeftLeg)
        previewRightLeg = findViewById(R.id.previewRightLeg)
        bodyPartTabs = findViewById(R.id.bodyPartTabs)
        colorPaletteGrid = findViewById(R.id.colorPaletteGrid)
        faceChoicesRow = findViewById(R.id.faceChoicesRow)
        shirtChoicesRow = findViewById(R.id.shirtChoicesRow)
        hatChoicesRow = findViewById(R.id.hatChoicesRow)

        etPlayerName.setText(profile.playerName)

        findViewById<Button>(R.id.btnBackAvatar).setOnClickListener { finish() }
        findViewById<Button>(R.id.btnSaveAvatar).setOnClickListener {
            profile = profile.copy(
                playerName = etPlayerName.text.toString().trim().ifEmpty { "Bloxxer07" }
            )
            AvatarPreferences.save(this, profile)
            AvatarPreferences.pushToNative(profile)
            Toast.makeText(this, "Character saved", Toast.LENGTH_SHORT).show()
            finish()
        }

        findViewById<Button>(R.id.btnPresetNoob).setOnClickListener {
            applyPreset(AvatarPreferences.presetNoob(etPlayerName.text.toString().trim()))
        }
        findViewById<Button>(R.id.btnPresetBloxxer).setOnClickListener {
            applyPreset(AvatarPreferences.presetBloxxer(etPlayerName.text.toString().trim()))
        }
        findViewById<Button>(R.id.btnPresetGuest).setOnClickListener {
            applyPreset(AvatarPreferences.presetGuest(etPlayerName.text.toString().trim()))
        }
        findViewById<Button>(R.id.btnPresetBuilder).setOnClickListener {
            applyPreset(AvatarPreferences.presetBuilder(etPlayerName.text.toString().trim()))
        }

        buildPartTabs()
        buildColorGrid()
        rebuildAssetRows()
        updatePreview()
    }

    private fun applyPreset(preset: AvatarProfile) {
        profile = preset
        etPlayerName.setText(profile.playerName)
        rebuildAssetRows()
        updatePreview()
    }

    private fun buildPartTabs() {
        bodyPartTabs.removeAllViews()
        val parts = listOf(
            BodyPart.HEAD to "Head",
            BodyPart.TORSO to "Torso",
            BodyPart.LEFT_ARM to "L Arm",
            BodyPart.RIGHT_ARM to "R Arm",
            BodyPart.LEFT_LEG to "L Leg",
            BodyPart.RIGHT_LEG to "R Leg"
        )
        for ((part, label) in parts) {
            val active = part == selectedPart
            val btn = Button(this).apply {
                text = label
                isAllCaps = false
                setTextColor(if (active) 0xFFFFFFFF.toInt() else 0xFF1F2B3D.toInt())
                setBackgroundResource(
                    if (active) R.drawable.bg_retro_2007_button else R.drawable.bg_retro_2007_slot
                )
                setOnClickListener {
                    selectedPart = part
                    buildPartTabs()
                }
            }
            val lp = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                (38 * resources.displayMetrics.density).toInt()
            ).apply {
                marginEnd = (8 * resources.displayMetrics.density).toInt()
            }
            bodyPartTabs.addView(btn, lp)
        }
    }

    private fun buildColorGrid() {
        colorPaletteGrid.removeAllViews()
        val sizePx = (44 * resources.displayMetrics.density).toInt()
        val marginPx = (6 * resources.displayMetrics.density).toInt()

        for (opt in AvatarPreferences.brickColors) {
            val swatch = View(this).apply {
                background = GradientDrawable().apply {
                    setColor(opt.argb)
                    cornerRadius = 6f * resources.displayMetrics.density
                    setStroke((2 * resources.displayMetrics.density).toInt(), 0xFF616A78.toInt())
                }
                contentDescription = opt.name
                setOnClickListener {
                    profile = when (selectedPart) {
                        BodyPart.HEAD -> profile.copy(headColor = opt.id)
                        BodyPart.TORSO -> profile.copy(torsoColor = opt.id)
                        BodyPart.LEFT_ARM -> profile.copy(leftArmColor = opt.id)
                        BodyPart.RIGHT_ARM -> profile.copy(rightArmColor = opt.id)
                        BodyPart.LEFT_LEG -> profile.copy(leftLegColor = opt.id)
                        BodyPart.RIGHT_LEG -> profile.copy(rightLegColor = opt.id)
                    }
                    updatePreview()
                }
            }
            val lp = GridLayout.LayoutParams().apply {
                width = sizePx
                height = sizePx
                setMargins(marginPx, marginPx, marginPx, marginPx)
            }
            colorPaletteGrid.addView(swatch, lp)
        }
    }

    private fun rebuildAssetRows() {
        buildAssetChoiceRow(faceChoicesRow, AvatarPreferences.faceChoices, profile.faceAsset) { chosen ->
            profile = profile.copy(faceAsset = chosen)
            rebuildAssetRows()
        }
        buildAssetChoiceRow(shirtChoicesRow, AvatarPreferences.shirtChoices, profile.shirtAsset) { chosen ->
            profile = profile.copy(shirtAsset = chosen)
            rebuildAssetRows()
        }
        buildAssetChoiceRow(hatChoicesRow, AvatarPreferences.hatChoices, profile.hatAsset) { chosen ->
            profile = profile.copy(hatAsset = chosen)
            rebuildAssetRows()
        }
    }

    private fun buildAssetChoiceRow(
        container: LinearLayout,
        choices: List<AssetChoice>,
        currentValue: String,
        onPick: (String) -> Unit
    ) {
        container.removeAllViews()
        for (choice in choices) {
            val active = choice.assetUri == currentValue
            val btn = Button(this).apply {
                text = choice.label
                isAllCaps = false
                setTextColor(if (active) 0xFFFFFFFF.toInt() else 0xFF1F2B3D.toInt())
                setBackgroundResource(
                    if (active) R.drawable.bg_retro_2007_button else R.drawable.bg_retro_2007_slot
                )
                setOnClickListener { onPick(choice.assetUri) }
            }
            val lp = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                (38 * resources.displayMetrics.density).toInt()
            ).apply {
                marginEnd = (8 * resources.displayMetrics.density).toInt()
            }
            container.addView(btn, lp)
        }
    }

    private fun updatePreview() {
        previewHead.setBackgroundColor(AvatarPreferences.colorForId(profile.headColor))
        previewTorso.setBackgroundColor(AvatarPreferences.colorForId(profile.torsoColor))
        previewLeftArm.setBackgroundColor(AvatarPreferences.colorForId(profile.leftArmColor))
        previewRightArm.setBackgroundColor(AvatarPreferences.colorForId(profile.rightArmColor))
        previewLeftLeg.setBackgroundColor(AvatarPreferences.colorForId(profile.leftLegColor))
        previewRightLeg.setBackgroundColor(AvatarPreferences.colorForId(profile.rightLegColor))
    }
}

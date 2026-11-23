package com.openrbx.android

import android.content.Context
import android.graphics.Color

data class BrickColorOption(
    val id: Int,
    val name: String,
    val argb: Int
)

data class AssetChoice(
    val label: String,
    val assetUri: String
)

data class AvatarProfile(
    val playerName: String = "Bloxxer07",
    val headColor: Int = 24,
    val torsoColor: Int = 23,
    val leftArmColor: Int = 24,
    val rightArmColor: Int = 24,
    val leftLegColor: Int = 119,
    val rightLegColor: Int = 119,
    val shirtAsset: String = "rbxasset://textures/shirt_classic.png",
    val faceAsset: String = "rbxasset://textures/face_smile.png",
    val hatAsset: String = "rbxasset://hats/classic_cap.rbxm"
)

object AvatarPreferences {
    private const val PREF_NAME = "openrbx_avatar"

    val brickColors = listOf(
        BrickColorOption(24, "Bright Yellow", Color.rgb(245, 205, 48)),
        BrickColorOption(23, "Bright Blue", Color.rgb(13, 105, 172)),
        BrickColorOption(21, "Bright Red", Color.rgb(196, 40, 28)),
        BrickColorOption(119, "Br. Yellowish Green", Color.rgb(164, 189, 71)),
        BrickColorOption(28, "Dark Green", Color.rgb(40, 127, 71)),
        BrickColorOption(1, "White", Color.rgb(242, 243, 243)),
        BrickColorOption(194, "Medium Stone Grey", Color.rgb(163, 162, 165)),
        BrickColorOption(199, "Dark Stone Grey", Color.rgb(99, 95, 98)),
        BrickColorOption(26, "Black", Color.rgb(27, 42, 53)),
        BrickColorOption(106, "Bright Orange", Color.rgb(218, 133, 65)),
        BrickColorOption(104, "Bright Violet", Color.rgb(107, 50, 124)),
        BrickColorOption(22, "Reddish Brown", Color.rgb(105, 64, 40)),
        BrickColorOption(5, "Brick Yellow", Color.rgb(215, 197, 154)),
        BrickColorOption(102, "Medium Blue", Color.rgb(110, 153, 202)),
        BrickColorOption(222, "Light Reddish Violet", Color.rgb(232, 186, 200)),
        BrickColorOption(107, "Bright Bluish Green", Color.rgb(0, 143, 156))
    )

    val shirtChoices = listOf(
        AssetChoice("None", ""),
        AssetChoice("2007 R Emblem", "rbxasset://textures/shirt_classic.png"),
        AssetChoice("Bloxxer Crest", "rbxasset://textures/shirt_bloxxer.png"),
        AssetChoice("Guest 2007", "rbxasset://textures/shirt_guest.png"),
        AssetChoice("Retro Stripes", "rbxasset://textures/shirt_stripes.png")
    )

    val faceChoices = listOf(
        AssetChoice("Classic Smile", "rbxasset://textures/face_smile.png"),
        AssetChoice(":D Epic Smile", "rbxasset://textures/face_epic.png"),
        AssetChoice("Wink", "rbxasset://textures/face_wink.png"),
        AssetChoice("Chill", "rbxasset://textures/face_chill.png")
    )

    val hatChoices = listOf(
        AssetChoice("None", ""),
        AssetChoice("Red Roblox Cap", "rbxasset://hats/classic_cap.rbxm"),
        AssetChoice("Blue Roblox Cap", "rbxasset://hats/blue_cap.rbxm"),
        AssetChoice("Top Hat", "rbxasset://hats/top_hat.rbxm"),
        AssetChoice("Classic Fedora", "rbxasset://hats/fedora.rbxm")
    )

    fun presetNoob(name: String) = AvatarProfile(
        playerName = name.ifBlank { "Noob2007" },
        headColor = 24, torsoColor = 23, leftArmColor = 24, rightArmColor = 24,
        leftLegColor = 119, rightLegColor = 119,
        shirtAsset = "", faceAsset = "rbxasset://textures/face_smile.png", hatAsset = ""
    )

    fun presetBloxxer(name: String) = AvatarProfile(
        playerName = name.ifBlank { "Bloxxer07" },
        headColor = 24, torsoColor = 21, leftArmColor = 24, rightArmColor = 24,
        leftLegColor = 26, rightLegColor = 26,
        shirtAsset = "rbxasset://textures/shirt_bloxxer.png",
        faceAsset = "rbxasset://textures/face_epic.png",
        hatAsset = "rbxasset://hats/classic_cap.rbxm"
    )

    fun presetGuest(name: String) = AvatarProfile(
        playerName = name.ifBlank { "Guest 1337" },
        headColor = 1, torsoColor = 26, leftArmColor = 1, rightArmColor = 1,
        leftLegColor = 26, rightLegColor = 26,
        shirtAsset = "rbxasset://textures/shirt_guest.png",
        faceAsset = "rbxasset://textures/face_smile.png",
        hatAsset = "rbxasset://hats/blue_cap.rbxm"
    )

    fun presetBuilder(name: String) = AvatarProfile(
        playerName = name.ifBlank { "Builderman" },
        headColor = 5, torsoColor = 106, leftArmColor = 5, rightArmColor = 5,
        leftLegColor = 199, rightLegColor = 199,
        shirtAsset = "rbxasset://textures/shirt_classic.png",
        faceAsset = "rbxasset://textures/face_wink.png",
        hatAsset = "rbxasset://hats/fedora.rbxm"
    )

    fun colorForId(id: Int): Int =
        brickColors.firstOrNull { it.id == id }?.argb ?: Color.rgb(163, 162, 165)

    fun load(context: Context): AvatarProfile {
        val p = context.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE)
        return AvatarProfile(
            playerName = p.getString("player_name", "Bloxxer07") ?: "Bloxxer07",
            headColor = p.getInt("head_color", 24),
            torsoColor = p.getInt("torso_color", 23),
            leftArmColor = p.getInt("left_arm_color", 24),
            rightArmColor = p.getInt("right_arm_color", 24),
            leftLegColor = p.getInt("left_leg_color", 119),
            rightLegColor = p.getInt("right_leg_color", 119),
            shirtAsset = p.getString("shirt_asset", "rbxasset://textures/shirt_classic.png")
                ?: "rbxasset://textures/shirt_classic.png",
            faceAsset = p.getString("face_asset", "rbxasset://textures/face_smile.png")
                ?: "rbxasset://textures/face_smile.png",
            hatAsset = p.getString("hat_asset", "rbxasset://hats/classic_cap.rbxm")
                ?: "rbxasset://hats/classic_cap.rbxm"
        )
    }

    fun save(context: Context, profile: AvatarProfile) {
        context.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE).edit()
            .putString("player_name", profile.playerName.ifBlank { "Bloxxer07" })
            .putInt("head_color", profile.headColor)
            .putInt("torso_color", profile.torsoColor)
            .putInt("left_arm_color", profile.leftArmColor)
            .putInt("right_arm_color", profile.rightArmColor)
            .putInt("left_leg_color", profile.leftLegColor)
            .putInt("right_leg_color", profile.rightLegColor)
            .putString("shirt_asset", profile.shirtAsset)
            .putString("face_asset", profile.faceAsset)
            .putString("hat_asset", profile.hatAsset)
            .apply()
    }

    fun pushToNative(profile: AvatarProfile) {
        NativeEngine.nativeSetAvatar(
            profile.playerName.ifBlank { "Bloxxer07" },
            profile.headColor,
            profile.torsoColor,
            profile.leftArmColor,
            profile.rightArmColor,
            profile.leftLegColor,
            profile.rightLegColor,
            profile.shirtAsset,
            profile.faceAsset,
            profile.hatAsset
        )
    }
}

package com.openrbx.android

import android.content.Context
import android.net.Uri
import android.provider.OpenableColumns
import java.io.File
import java.io.FileOutputStream

object AssetInstaller {
    private const val PREF_ASSETS = "openrbx_assets"
    private const val KEY_ASSET_VERSION = "asset_version"
    private const val CURRENT_ASSET_VERSION = 2

    fun ensureInstalled(context: Context): InstallPaths {
        val contentDir = File(context.filesDir, "content")
        val placesDir = File(context.filesDir, "places")
        val prefs = context.getSharedPreferences(PREF_ASSETS, Context.MODE_PRIVATE)
        val installedVersion = prefs.getInt(KEY_ASSET_VERSION, 0)

        if (installedVersion < CURRENT_ASSET_VERSION ||
            !File(contentDir, "fonts/character.rbxm").exists() ||
            !File(placesDir, "crossroads_2007.rbxl").exists()
        ) {
            copyAssetDir(context, "content", contentDir)
            copyAssetDir(context, "places", placesDir)
            prefs.edit().putInt(KEY_ASSET_VERSION, CURRENT_ASSET_VERSION).apply()
        }

        return InstallPaths(
            filesDir = context.filesDir.absolutePath,
            cacheDir = context.cacheDir.absolutePath,
            contentDir = contentDir.absolutePath,
            crossroadsPath = File(placesDir, "crossroads_2007.rbxl").absolutePath,
            happyHomePath = File(placesDir, "happy_home_2007.rbxl").absolutePath,
            starterPlacePath = File(placesDir, "classic_baseplate.rbxl").absolutePath
        )
    }

    fun copyUriToImportedPlace(context: Context, uri: Uri): File? {
        val displayName = queryDisplayName(context, uri) ?: "imported_place.rbxl"
        val safeName = displayName.replace(Regex("[^A-Za-z0-9._-]"), "_")
        val outDir = File(context.filesDir, "imported_places").apply { mkdirs() }
        val outFile = File(outDir, safeName)

        context.contentResolver.openInputStream(uri)?.use { input ->
            FileOutputStream(outFile).use { output ->
                input.copyTo(output)
            }
            return outFile
        }
        return null
    }

    private fun queryDisplayName(context: Context, uri: Uri): String? {
        context.contentResolver.query(uri, arrayOf(OpenableColumns.DISPLAY_NAME), null, null, null)?.use { cursor ->
            if (cursor.moveToFirst()) {
                val idx = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                if (idx >= 0) return cursor.getString(idx)
            }
        }
        return uri.lastPathSegment
    }

    private fun copyAssetDir(context: Context, assetPath: String, destDir: File) {
        val entries = context.assets.list(assetPath) ?: return
        if (entries.isEmpty()) {
            destDir.parentFile?.mkdirs()
            context.assets.open(assetPath).use { input ->
                FileOutputStream(destDir).use { output ->
                    input.copyTo(output)
                }
            }
        } else {
            destDir.mkdirs()
            for (entry in entries) {
                copyAssetDir(context, "$assetPath/$entry", File(destDir, entry))
            }
        }
    }

    data class InstallPaths(
        val filesDir: String,
        val cacheDir: String,
        val contentDir: String,
        val crossroadsPath: String,
        val happyHomePath: String,
        val starterPlacePath: String
    )
}

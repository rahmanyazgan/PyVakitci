package com.rahmanyazgan.pyvakitci.ui.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

data class AppColors(
    val background: Color,
    val surface: Color,
    val primary: Color,
    val text: Color,
    val cardBorder: Color
)

fun getAppColors(themeKey: String): AppColors {
    return when (themeKey.lowercase()) {
        "koyu" -> AppColors(DarkBg, DarkSurface, DarkPrimary, DarkText, DarkCardBorder)
        "gece_mavisi" -> AppColors(NightBlueBg, NightBlueSurface, NightBluePrimary, NightBlueText, NightBlueCardBorder)
        "zumrut" -> AppColors(EmeraldBg, EmeraldSurface, EmeraldPrimary, EmeraldText, EmeraldCardBorder)
        "kehribar" -> AppColors(AmberBg, AmberSurface, AmberPrimary, AmberText, AmberCardBorder)
        else -> AppColors(DefaultLightBg, DefaultLightSurface, DefaultLightPrimary, DefaultLightText, DefaultLightCardBorder)
    }
}

@Composable
fun PyVakitciTheme(
    themeKey: String = "varsayilan",
    content: @Composable () -> Unit
) {
    val appColors = getAppColors(themeKey)
    val colorScheme = if (themeKey == "varsayilan") {
        lightColorScheme(
            primary = appColors.primary,
            background = appColors.background,
            surface = appColors.surface,
            onBackground = appColors.text,
            onSurface = appColors.text
        )
    } else {
        darkColorScheme(
            primary = appColors.primary,
            background = appColors.background,
            surface = appColors.surface,
            onBackground = appColors.text,
            onSurface = appColors.text
        )
    }

    MaterialTheme(
        colorScheme = colorScheme,
        typography = Typography,
        content = content
    )
}


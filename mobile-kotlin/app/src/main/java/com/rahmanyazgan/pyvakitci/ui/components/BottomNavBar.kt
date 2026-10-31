package com.rahmanyazgan.pyvakitci.ui.components

import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AccessTime
import androidx.compose.material.icons.filled.Explore
import androidx.compose.material.icons.filled.MusicNote
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material3.Icon
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.NavigationBarItemDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.util.L10n

@Composable
fun BottomNavBar(
    selectedTab: Int,
    onTabSelected: (Int) -> Unit,
    containerColor: Color,
    selectedColor: Color,
    unselectedColor: Color
) {
    NavigationBar(
        containerColor = containerColor,
        tonalElevation = 8.dp,
        modifier = Modifier
            .navigationBarsPadding()
            .clip(RoundedCornerShape(topStart = 16.dp, topEnd = 16.dp))
    ) {
        val items = listOf(
            Triple(0, L10n.t("nav_vakit"), Icons.Default.AccessTime),
            Triple(1, L10n.t("nav_hava"), Icons.Default.WbSunny),
            Triple(2, L10n.t("nav_kible"), Icons.Default.Explore),
            Triple(3, L10n.t("nav_sesler"), Icons.Default.MusicNote),
            Triple(4, L10n.t("nav_ayarlar"), Icons.Default.Settings)
        )

        items.forEach { (index, title, icon) ->
            val isSelected = selectedTab == index
            NavigationBarItem(
                selected = isSelected,
                onClick = { onTabSelected(index) },
                icon = {
                    Icon(
                        imageVector = icon,
                        contentDescription = title,
                        modifier = Modifier.size(24.dp)
                    )
                },
                label = {
                    Text(
                        text = title,
                        fontSize = 12.sp,
                        fontWeight = if (isSelected) FontWeight.Bold else FontWeight.SemiBold,
                        maxLines = 1
                    )
                },
                colors = NavigationBarItemDefaults.colors(
                    selectedIconColor = Color.White,
                    selectedTextColor = selectedColor,
                    unselectedIconColor = unselectedColor.copy(alpha = 0.8f),
                    unselectedTextColor = unselectedColor.copy(alpha = 0.85f),
                    indicatorColor = selectedColor
                )
            )
        }
    }
}

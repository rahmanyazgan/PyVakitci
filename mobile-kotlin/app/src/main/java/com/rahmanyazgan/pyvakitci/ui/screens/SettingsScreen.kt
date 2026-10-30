package com.rahmanyazgan.pyvakitci.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.filled.Check
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.Icon
import androidx.compose.material3.RadioButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.data.remote.DiyanetApiService
import androidx.compose.runtime.rememberCoroutineScope
import kotlinx.coroutines.launch
import com.rahmanyazgan.pyvakitci.data.model.City
import com.rahmanyazgan.pyvakitci.data.model.Country
import com.rahmanyazgan.pyvakitci.data.model.District
import com.rahmanyazgan.pyvakitci.data.model.SelectedLocation
import com.rahmanyazgan.pyvakitci.ui.theme.AppColors

@Composable
fun SettingsScreen(
    prefs: PreferencesManager,
    countries: List<Country>,
    cities: List<City>,
    districts: List<District>,
    colors: AppColors,
    diyanetApi: DiyanetApiService? = null,
    onSaveLocation: (SelectedLocation) -> Unit,
    onThemeChanged: (String) -> Unit,
    onInterfaceModeChanged: (String) -> Unit,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()
    val scope = rememberCoroutineScope()

    var currentCities by remember(cities) { mutableStateOf(cities) }
    var currentDistricts by remember(districts) { mutableStateOf(districts) }
    var isLoadingLocation by remember { mutableStateOf(false) }

    var selectedCountry by remember { mutableStateOf(countries.find { it.id == prefs.location.countryId } ?: Country("2", "TÜRKİYE")) }
    var selectedCity by remember { mutableStateOf(currentCities.find { it.id == prefs.location.cityId } ?: (currentCities.firstOrNull() ?: City("539", "İSTANBUL"))) }
    var selectedDistrict by remember { mutableStateOf(currentDistricts.find { it.id == prefs.location.districtId } ?: (currentDistricts.firstOrNull() ?: District("9541", "İSTANBUL"))) }

    var countryExpanded by remember { mutableStateOf(false) }
    var cityExpanded by remember { mutableStateOf(false) }
    var districtExpanded by remember { mutableStateOf(false) }

    var currentTheme by remember { mutableStateOf(prefs.theme) }
    var currentMode by remember { mutableStateOf(prefs.interfaceMode) }

    var savedMessage by remember { mutableStateOf(false) }

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(colors.background)
            .padding(16.dp)
            .verticalScroll(scrollState)
    ) {
        Text(
            text = "Bölgesel ve Genel Ayarlar",
            fontSize = 20.sp,
            fontWeight = FontWeight.Bold,
            color = colors.text
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Bölgesel Ayarlar Kartı
        Card(
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(2.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(
                        text = "Konum Seçimi",
                        fontSize = 16.sp,
                        fontWeight = FontWeight.Bold,
                        color = colors.primary
                    )
                    if (isLoadingLocation) {
                        Text(
                            text = "Yükleniyor...",
                            fontSize = 12.sp,
                            color = colors.primary
                        )
                    }
                }

                Spacer(modifier = Modifier.height(12.dp))

                // Ülke Seçimi
                Text(text = "Ülke", fontSize = 12.sp, color = colors.text.copy(alpha = 0.6f))
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clickable { countryExpanded = true }
                        .padding(vertical = 8.dp)
                ) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(text = selectedCountry.name, fontSize = 15.sp, fontWeight = FontWeight.SemiBold, color = colors.text)
                        Icon(imageVector = Icons.Default.ArrowDropDown, contentDescription = null, tint = colors.text)
                    }
                    DropdownMenu(
                        expanded = countryExpanded,
                        onDismissRequest = { countryExpanded = false }
                    ) {
                        countries.forEach { c ->
                            DropdownMenuItem(
                                text = { Text(c.name) },
                                onClick = {
                                    selectedCountry = c
                                    countryExpanded = false
                                    if (diyanetApi != null) {
                                        isLoadingLocation = true
                                        scope.launch {
                                            val fetchedCities = diyanetApi.getCities(c.id)
                                            currentCities = fetchedCities
                                            val firstCity = fetchedCities.firstOrNull() ?: City("1", "MERKEZ")
                                            selectedCity = firstCity

                                            val fetchedDistricts = diyanetApi.getDistricts(c.id, firstCity.id, firstCity.name)
                                            currentDistricts = fetchedDistricts
                                            selectedDistrict = fetchedDistricts.firstOrNull() ?: District(firstCity.id, firstCity.name)
                                            isLoadingLocation = false
                                        }
                                    }
                                }
                            )
                        }
                    }
                }

                Spacer(modifier = Modifier.height(8.dp))

                // Şehir Seçimi
                Text(text = "Şehir", fontSize = 12.sp, color = colors.text.copy(alpha = 0.6f))
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clickable { cityExpanded = true }
                        .padding(vertical = 8.dp)
                ) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(text = selectedCity.name, fontSize = 15.sp, fontWeight = FontWeight.SemiBold, color = colors.text)
                        Icon(imageVector = Icons.Default.ArrowDropDown, contentDescription = null, tint = colors.text)
                    }
                    DropdownMenu(
                        expanded = cityExpanded,
                        onDismissRequest = { cityExpanded = false }
                    ) {
                        currentCities.forEach { ci ->
                            DropdownMenuItem(
                                text = { Text(ci.name) },
                                onClick = {
                                    selectedCity = ci
                                    cityExpanded = false
                                    if (diyanetApi != null) {
                                        isLoadingLocation = true
                                        scope.launch {
                                            val fetchedDistricts = diyanetApi.getDistricts(selectedCountry.id, ci.id, ci.name)
                                            currentDistricts = fetchedDistricts
                                            selectedDistrict = fetchedDistricts.firstOrNull() ?: District(ci.id, ci.name)
                                            isLoadingLocation = false
                                        }
                                    }
                                }
                            )
                        }
                    }
                }

                Spacer(modifier = Modifier.height(8.dp))

                // İlçe Seçimi
                Text(text = "İlçe", fontSize = 12.sp, color = colors.text.copy(alpha = 0.6f))
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clickable { districtExpanded = true }
                        .padding(vertical = 8.dp)
                ) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(text = selectedDistrict.name, fontSize = 15.sp, fontWeight = FontWeight.SemiBold, color = colors.text)
                        Icon(imageVector = Icons.Default.ArrowDropDown, contentDescription = null, tint = colors.text)
                    }
                    DropdownMenu(
                        expanded = districtExpanded,
                        onDismissRequest = { districtExpanded = false }
                    ) {
                        currentDistricts.forEach { d ->
                            DropdownMenuItem(
                                text = { Text(d.name) },
                                onClick = {
                                    selectedDistrict = d
                                    districtExpanded = false
                                }
                            )
                        }
                    }
                }

                Spacer(modifier = Modifier.height(14.dp))

                Button(
                    onClick = {
                        val newLoc = SelectedLocation(
                            countryId = selectedCountry.id,
                            countryName = selectedCountry.name,
                            cityId = selectedCity.id,
                            cityName = selectedCity.name,
                            districtId = selectedDistrict.id,
                            districtName = selectedDistrict.name
                        )
                        onSaveLocation(newLoc)
                        savedMessage = true
                    },
                    colors = ButtonDefaults.buttonColors(containerColor = colors.primary),
                    shape = RoundedCornerShape(10.dp),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Icon(imageVector = Icons.Default.Check, contentDescription = null)
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(text = "Konumu Kaydet ve Vakitleri Güncelle")
                }

                if (savedMessage) {
                    Spacer(modifier = Modifier.height(6.dp))
                    Text(
                        text = "Konum kaydedildi!",
                        color = colors.primary,
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold,
                        modifier = Modifier.align(Alignment.CenterHorizontally)
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(18.dp))

        // Arayüz Modu Seçimi
        Card(
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(2.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(
                    text = "Arayüz Modu",
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Bold,
                    color = colors.primary
                )

                Spacer(modifier = Modifier.height(8.dp))

                val modes = listOf(
                    "saat" to "Kadranlı Vakit Saati (Varsayılan)",
                    "gokyuzu" to "Dinamik Gökyüzü Modu",
                    "kartlar" to "Modern Kartlar Modu"
                )

                modes.forEach { (modeKey, label) ->
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable {
                                currentMode = modeKey
                                prefs.interfaceMode = modeKey
                                onInterfaceModeChanged(modeKey)
                            }
                            .padding(vertical = 4.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        RadioButton(
                            selected = currentMode == modeKey,
                            onClick = {
                                currentMode = modeKey
                                prefs.interfaceMode = modeKey
                                onInterfaceModeChanged(modeKey)
                            }
                        )
                        Text(text = label, fontSize = 14.sp, color = colors.text)
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(18.dp))

        // Tema Seçimi (5 Tema)
        Card(
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(2.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(
                    text = "Renk Teması",
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Bold,
                    color = colors.primary
                )

                Spacer(modifier = Modifier.height(8.dp))

                val themes = listOf(
                    "varsayilan" to "Açık Tema (Varsayılan)",
                    "koyu" to "Modern Koyu Tema",
                    "gece_mavisi" to "Gece Mavisi",
                    "zumrut" to "Zümrüt Yeşili",
                    "kehribar" to "Sıcak Kehribar"
                )

                themes.forEach { (themeKey, label) ->
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable {
                                currentTheme = themeKey
                                prefs.theme = themeKey
                                onThemeChanged(themeKey)
                            }
                            .padding(vertical = 4.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        RadioButton(
                            selected = currentTheme == themeKey,
                            onClick = {
                                currentTheme = themeKey
                                prefs.theme = themeKey
                                onThemeChanged(themeKey)
                            }
                        )
                        Text(text = label, fontSize = 14.sp, color = colors.text)
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(20.dp))

        // Hakkında
        Text(
            text = "PyVakitci Mobil v1.8 • Diyanet İşleri Başkanlığı",
            fontSize = 11.sp,
            color = colors.text.copy(alpha = 0.5f),
            modifier = Modifier.align(Alignment.CenterHorizontally)
        )
    }
}


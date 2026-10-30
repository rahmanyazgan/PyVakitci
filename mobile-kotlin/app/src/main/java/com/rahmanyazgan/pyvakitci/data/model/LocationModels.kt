package com.rahmanyazgan.pyvakitci.data.model

data class Country(
    val id: String,
    val name: String
)

data class City(
    val id: String,
    val name: String
)

data class District(
    val id: String,
    val name: String
)

data class SelectedLocation(
    val countryId: String = "2",
    val countryName: String = "TÜRKİYE",
    val cityId: String = "539",
    val cityName: String = "İSTANBUL",
    val districtId: String = "9541",
    val districtName: String = "İSTANBUL",
    val latitude: Double = 41.0082,
    val longitude: Double = 28.9784
)


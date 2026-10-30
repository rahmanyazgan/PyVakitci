import Foundation

public struct Country: Identifiable, Codable, Hashable {
    public let id: String
    public let name: String

    public init(id: String, name: String) {
        self.id = id
        self.name = name
    }
}

public struct City: Identifiable, Codable, Hashable {
    public let id: String
    public let name: String

    public init(id: String, name: String) {
        self.id = id
        self.name = name
    }
}

public struct District: Identifiable, Codable, Hashable {
    public let id: String
    public let name: String

    public init(id: String, name: String) {
        self.id = id
        self.name = name
    }
}

public struct SelectedLocation: Codable, Equatable {
    public var countryId: String
    public var countryName: String
    public var cityId: String
    public var cityName: String
    public var districtId: String
    public var districtName: String
    public var latitude: Double
    public var longitude: Double

    public init(
        countryId: String = "2",
        countryName: String = "TÜRKİYE",
        cityId: String = "539",
        cityName: String = "İSTANBUL",
        districtId: String = "9541",
        districtName: String = "İSTANBUL",
        latitude: Double = 41.0082,
        longitude: Double = 28.9784
    ) {
        self.countryId = countryId
        self.countryName = countryName
        self.cityId = cityId
        self.cityName = cityName
        self.districtId = districtId
        self.districtName = districtName
        self.latitude = latitude
        self.longitude = longitude
    }
}


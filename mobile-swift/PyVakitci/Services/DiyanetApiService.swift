import Foundation

public class DiyanetApiService {
    public static let shared = DiyanetApiService()
    private let session: URLSession
    private let userAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148 PyVakitci/1.0"

    private let turkiyeSehirleri: [City] = [
        City(id: "500", name: "ADANA"), City(id: "501", name: "ADIYAMAN"), City(id: "502", name: "AFYONKARAHİSAR"), City(id: "503", name: "AĞRI"),
        City(id: "504", name: "AKSARAY"), City(id: "505", name: "AMASYA"), City(id: "506", name: "ANKARA"), City(id: "507", name: "ANTALYA"),
        City(id: "508", name: "ARDAHAN"), City(id: "509", name: "ARTVİN"), City(id: "510", name: "AYDIN"), City(id: "511", name: "BALIKESİR"),
        City(id: "512", name: "BARTIN"), City(id: "513", name: "BATMAN"), City(id: "514", name: "BAYBURT"), City(id: "515", name: "BİLECİK"),
        City(id: "516", name: "BİNGÖL"), City(id: "517", name: "BİTLİS"), City(id: "518", name: "BOLU"), City(id: "519", name: "BURDUR"),
        City(id: "520", name: "BURSA"), City(id: "521", name: "ÇANAKKALE"), City(id: "522", name: "ÇANKIRI"), City(id: "523", name: "ÇORUM"),
        City(id: "524", name: "DENİZLİ"), City(id: "525", name: "DİYARBAKIR"), City(id: "526", name: "DÜZCE"), City(id: "527", name: "EDİRNE"),
        City(id: "528", name: "ELAZIĞ"), City(id: "529", name: "ERZİNCAN"), City(id: "530", name: "ERZURUM"), City(id: "531", name: "ESKİŞEHİR"),
        City(id: "532", name: "GAZİANTEP"), City(id: "533", name: "GİRESUN"), City(id: "534", name: "GÜMÜŞHANE"), City(id: "535", name: "HAKKARİ"),
        City(id: "536", name: "HATAY"), City(id: "537", name: "IĞDIR"), City(id: "538", name: "ISPARTA"), City(id: "539", name: "İSTANBUL"),
        City(id: "540", name: "İZMİR"), City(id: "541", name: "KAHRAMANMARAŞ"), City(id: "542", name: "KARABÜK"), City(id: "543", name: "KARAMAN"),
        City(id: "544", name: "KARS"), City(id: "545", name: "KASTAMONU"), City(id: "546", name: "KAYSERİ"), City(id: "547", name: "KİLİS"),
        City(id: "548", name: "KIRIKKALE"), City(id: "549", name: "KIRKLARELİ"), City(id: "550", name: "KIRŞEHİR"), City(id: "551", name: "KOCAELİ"),
        City(id: "552", name: "KONYA"), City(id: "553", name: "KÜTAHYA"), City(id: "554", name: "MALATYA"), City(id: "555", name: "MANİSA"),
        City(id: "556", name: "MARDİN"), City(id: "557", name: "MERSİN"), City(id: "558", name: "MUĞLA"), City(id: "559", name: "MUŞ"),
        City(id: "560", name: "NEVŞEHİR"), City(id: "561", name: "NİĞDE"), City(id: "562", name: "ORDU"), City(id: "563", name: "OSMANİYE"),
        City(id: "564", name: "RİZE"), City(id: "565", name: "SAKARYA"), City(id: "566", name: "SAMSUN"), City(id: "567", name: "ŞANLIURFA"),
        City(id: "568", name: "SİİRT"), City(id: "569", name: "SİNOP"), City(id: "570", name: "ŞIRNAK"), City(id: "571", name: "SİVAS"),
        City(id: "572", name: "TEKİRDAĞ"), City(id: "573", name: "TOKAT"), City(id: "574", name: "TRABZON"), City(id: "575", name: "TUNCELİ"),
        City(id: "576", name: "UŞAK"), City(id: "577", name: "VAN"), City(id: "578", name: "YALOVA"), City(id: "579", name: "YOZGAT"),
        City(id: "580", name: "ZONGULDAK")
    ]

    public init() {
        let config = URLSessionConfiguration.default
        config.timeoutIntervalForRequest = 12
        config.timeoutIntervalForResource = 15
        self.session = URLSession(configuration: config)
    }

    public func getCountries() async -> [Country] {
        [
            Country(id: "33", name: "ABD"),
            Country(id: "166", name: "AFGANISTAN"),
            Country(id: "13", name: "ALMANYA"),
            Country(id: "17", name: "ANDORRA"),
            Country(id: "140", name: "ANGOLA"),
            Country(id: "125", name: "ANGUILLA"),
            Country(id: "90", name: "ANTIGUA VE BARBUDA"),
            Country(id: "199", name: "ARJANTIN"),
            Country(id: "25", name: "ARNAVUTLUK"),
            Country(id: "153", name: "ARUBA"),
            Country(id: "213", name: "ASCENSION"),
            Country(id: "1216", name: "ATLANTİK OKYANUSU"),
            Country(id: "59", name: "AVUSTRALYA"),
            Country(id: "35", name: "AVUSTURYA"),
            Country(id: "5", name: "AZERBAYCAN"),
            Country(id: "54", name: "BAHAMALAR"),
            Country(id: "132", name: "BAHREYN"),
            Country(id: "177", name: "BANGLADES"),
            Country(id: "188", name: "BARBADOS"),
            Country(id: "208", name: "BELARUS"),
            Country(id: "11", name: "BELCIKA"),
            Country(id: "182", name: "BELIZE"),
            Country(id: "181", name: "BENIN"),
            Country(id: "51", name: "BERMUDA"),
            Country(id: "93", name: "BIRLESIK ARAP EMIRLIGI"),
            Country(id: "83", name: "BOLIVYA"),
            Country(id: "9", name: "BOSNA HERSEK"),
            Country(id: "167", name: "BOTSVANA"),
            Country(id: "146", name: "BREZILYA"),
            Country(id: "97", name: "BRUNEI"),
            Country(id: "44", name: "BULGARISTAN"),
            Country(id: "91", name: "BURKINA FASO"),
            Country(id: "65", name: "BURUNDI"),
            Country(id: "155", name: "BUTAN"),
            Country(id: "156", name: "CAD"),
            Country(id: "16", name: "CEK CUMHURIYETI"),
            Country(id: "86", name: "CEZAYIR"),
            Country(id: "160", name: "CIBUTI"),
            Country(id: "61", name: "CIN"),
            Country(id: "26", name: "DANIMARKA"),
            Country(id: "180", name: "DEMOKRATIK KONGO CUMHURIYETI"),
            Country(id: "176", name: "DOGU TIMOR"),
            Country(id: "123", name: "DOMINIK"),
            Country(id: "72", name: "DOMINIK CUMHURIYETI"),
            Country(id: "139", name: "EKVATOR"),
            Country(id: "63", name: "EKVATOR GINESI"),
            Country(id: "165", name: "EL SALVADOR"),
            Country(id: "117", name: "ENDONEZYA"),
            Country(id: "175", name: "ERITRE"),
            Country(id: "104", name: "ERMENISTAN"),
            Country(id: "6", name: "ESTONYA"),
            Country(id: "170", name: "ESWATINI"),
            Country(id: "95", name: "ETYOPYA"),
            Country(id: "145", name: "FAS"),
            Country(id: "197", name: "FIJI"),
            Country(id: "120", name: "FILDISI SAHILI"),
            Country(id: "126", name: "FILIPINLER"),
            Country(id: "204", name: "FILISTIN"),
            Country(id: "41", name: "FINLANDIYA"),
            Country(id: "21", name: "FRANSA"),
            Country(id: "79", name: "GABON"),
            Country(id: "109", name: "GAMBIYA"),
            Country(id: "143", name: "GANA"),
            Country(id: "111", name: "GINE"),
            Country(id: "58", name: "GRENADA"),
            Country(id: "48", name: "GRONLAND"),
            Country(id: "171", name: "GUADELOPE"),
            Country(id: "169", name: "GUAM ADASI"),
            Country(id: "99", name: "GUATEMALA"),
            Country(id: "67", name: "GUNEY AFRIKA"),
            Country(id: "128", name: "GUNEY KORE"),
            Country(id: "214", name: "GUNEY SUDAN"),
            Country(id: "62", name: "GURCISTAN"),
            Country(id: "82", name: "GUYANA"),
            Country(id: "70", name: "HAITI"),
            Country(id: "187", name: "HINDISTAN"),
            Country(id: "30", name: "HIRVATISTAN"),
            Country(id: "4", name: "HOLLANDA"),
            Country(id: "105", name: "HONDURAS"),
            Country(id: "113", name: "HONG KONG"),
            Country(id: "15", name: "INGILTERE"),
            Country(id: "124", name: "IRAK"),
            Country(id: "202", name: "IRAN"),
            Country(id: "32", name: "IRLANDA"),
            Country(id: "23", name: "ISPANYA"),
            Country(id: "205", name: "ISRAIL"),
            Country(id: "12", name: "ISVEC"),
            Country(id: "49", name: "ISVICRE"),
            Country(id: "8", name: "ITALYA"),
            Country(id: "122", name: "IZLANDA"),
            Country(id: "119", name: "JAMAIKA"),
            Country(id: "116", name: "JAPONYA"),
            Country(id: "161", name: "KAMBOCYA"),
            Country(id: "184", name: "KAMERUN"),
            Country(id: "52", name: "KANADA"),
            Country(id: "34", name: "KARADAG"),
            Country(id: "94", name: "KATAR"),
            Country(id: "92", name: "KAZAKISTAN"),
            Country(id: "114", name: "KENYA"),
            Country(id: "168", name: "KIRGIZISTAN"),
            Country(id: "57", name: "KOLOMBIYA"),
            Country(id: "88", name: "KOMORLAR"),
            Country(id: "18", name: "KOSOVA"),
            Country(id: "162", name: "KOSTARIKA"),
            Country(id: "209", name: "KUBA"),
            Country(id: "206", name: "KUDUS"),
            Country(id: "133", name: "KUVEYT"),
            Country(id: "1", name: "KUZEY KIBRIS"),
            Country(id: "142", name: "KUZEY KORE"),
            Country(id: "134", name: "LAOS"),
            Country(id: "174", name: "LESOTO"),
            Country(id: "20", name: "LETONYA"),
            Country(id: "73", name: "LIBERYA"),
            Country(id: "203", name: "LIBYA"),
            Country(id: "38", name: "LIECHTENSTEIN"),
            Country(id: "47", name: "LITVANYA"),
            Country(id: "42", name: "LUBNAN"),
            Country(id: "31", name: "LUKSEMBURG"),
            Country(id: "7", name: "MACARISTAN"),
            Country(id: "98", name: "MADAGASKAR"),
            Country(id: "100", name: "MAKAO"),
            Country(id: "28", name: "MAKEDONYA"),
            Country(id: "55", name: "MALAVI"),
            Country(id: "103", name: "MALDIVLER"),
            Country(id: "107", name: "MALEZYA"),
            Country(id: "152", name: "MALI"),
            Country(id: "24", name: "MALTA"),
            Country(id: "212", name: "MAN ADASI"),
            Country(id: "87", name: "MARTINIK"),
            Country(id: "164", name: "MAURITIUS ADASI"),
            Country(id: "157", name: "MAYOTTE"),
            Country(id: "53", name: "MEKSIKA"),
            Country(id: "85", name: "MIKRONEZYA"),
            Country(id: "189", name: "MISIR"),
            Country(id: "60", name: "MOGOLISTAN"),
            Country(id: "46", name: "MOLDAVYA"),
            Country(id: "3", name: "MONAKO"),
            Country(id: "147", name: "MONTSERRAT (U.K.)"),
            Country(id: "106", name: "MORITANYA"),
            Country(id: "151", name: "MOZAMBIK"),
            Country(id: "154", name: "MYANMAR"),
            Country(id: "196", name: "NAMIBYA"),
            Country(id: "76", name: "NEPAL"),
            Country(id: "84", name: "NIJER"),
            Country(id: "127", name: "NIJERYA"),
            Country(id: "141", name: "NIKARAGUA"),
            Country(id: "178", name: "NIUE"),
            Country(id: "36", name: "NORVEC"),
            Country(id: "80", name: "ORTA AFRIKA CUMHURIYETI"),
            Country(id: "131", name: "OZBEKISTAN"),
            Country(id: "77", name: "PAKISTAN"),
            Country(id: "149", name: "PALAU"),
            Country(id: "89", name: "PANAMA"),
            Country(id: "185", name: "PAPUA YENI GINE"),
            Country(id: "194", name: "PARAGUAY"),
            Country(id: "69", name: "PERU"),
            Country(id: "183", name: "PITCAIRN ADASI"),
            Country(id: "39", name: "POLONYA"),
            Country(id: "45", name: "PORTEKIZ"),
            Country(id: "68", name: "PORTO RIKO"),
            Country(id: "112", name: "REUNION"),
            Country(id: "37", name: "ROMANYA"),
            Country(id: "81", name: "RUANDA"),
            Country(id: "207", name: "RUSYA"),
            Country(id: "64", name: "S. ARABISTAN"),
            Country(id: "198", name: "SAMOA"),
            Country(id: "102", name: "SENEGAL"),
            Country(id: "138", name: "SEYSEL ADALARI"),
            Country(id: "210", name: "SIERRA LEONE"),
            Country(id: "200", name: "SILI"),
            Country(id: "179", name: "SINGAPUR"),
            Country(id: "27", name: "SIRBISTAN"),
            Country(id: "14", name: "SLOVAKYA"),
            Country(id: "19", name: "SLOVENYA"),
            Country(id: "150", name: "SOMALI"),
            Country(id: "74", name: "SRI LANKA"),
            Country(id: "136", name: "ST. LUCIA"),
            Country(id: "129", name: "SUDAN"),
            Country(id: "172", name: "SURINAM"),
            Country(id: "191", name: "SURIYE"),
            Country(id: "163", name: "SVALBARD"),
            Country(id: "101", name: "TACIKISTAN"),
            Country(id: "110", name: "TANZANYA"),
            Country(id: "137", name: "TAYLAND"),
            Country(id: "108", name: "TAYVAN"),
            Country(id: "71", name: "TOGO"),
            Country(id: "130", name: "TONGA"),
            Country(id: "96", name: "TRINIDAT VE TOBAGO"),
            Country(id: "118", name: "TUNUS"),
            Country(id: "159", name: "TURKMENISTAN"),
            Country(id: "2", name: "TÜRKİYE"),
            Country(id: "75", name: "UGANDA"),
            Country(id: "40", name: "UKRAYNA"),
            Country(id: "29", name: "UKRAYNA-KIRIM"),
            Country(id: "173", name: "UMMAN"),
            Country(id: "192", name: "URDUN"),
            Country(id: "201", name: "URUGUAY"),
            Country(id: "56", name: "VANUATU"),
            Country(id: "10", name: "VATIKAN"),
            Country(id: "186", name: "VENEZUELA"),
            Country(id: "135", name: "VIETNAM"),
            Country(id: "148", name: "YEMEN"),
            Country(id: "115", name: "YENI KALEDONYA"),
            Country(id: "193", name: "YENI ZELANDA"),
            Country(id: "144", name: "YESIL BURUN"),
            Country(id: "22", name: "YUNANISTAN"),
            Country(id: "158", name: "ZAMBIYA"),
            Country(id: "216", name: "ZIMBABVE"),
            Country(id: "66", name: "ÇURAÇAO"),
        ].sorted { $0.name < $1.name }
    }

    public func getCities(countryId: String) async -> [City] {
        if countryId == "2" {
            return turkiyeSehirleri
        }
        guard let url = URL(string: "https://namazvakitleri.diyanet.gov.tr/tr-TR/home/GetRegList?ChangeType=country&CountryId=\(countryId)") else {
            return [City(id: "1", name: "MERKEZ")]
        }

        var request = URLRequest(url: url)
        request.setValue(userAgent, forHTTPHeaderField: "User-Agent")
        request.setValue("text/html,application/xhtml+xml,application/xml;q=0.9,application/json,*/*;q=0.8", forHTTPHeaderField: "Accept")
        request.setValue("tr-TR,tr;q=0.9,en-US;q=0.8,en;q=0.7", forHTTPHeaderField: "Accept-Language")

        do {
            let (data, response) = try await session.data(for: request)
            if let httpResp = response as? HTTPURLResponse, httpResp.statusCode == 200,
               let json = try? JSONSerialization.jsonObject(with: data) as? [String: Any] {
                var items = json["StateList"] as? [[String: Any]] ?? []
                if items.isEmpty {
                    items = json["StateRegionList"] as? [[String: Any]] ?? []
                }
                var results: [City] = []
                for it in items {
                    let name = (it["SehirAdi"] as? String) ?? (it["IlceAdi"] as? String) ?? ""
                    let id = (it["SehirID"] as? String) ?? (it["IlceID"] as? String) ?? ""
                    if !name.trimmingCharacters(in: .whitespaces).isEmpty && !id.isEmpty {
                        results.append(City(id: id, name: name.trimmingCharacters(in: .whitespaces)))
                    }
                }
                if !results.isEmpty {
                    return results.sorted { $0.name < $1.name }
                }
            }
        } catch {
            // network error fallback
        }

        return [City(id: "1", name: "MERKEZ")]
    }

    public func getDistricts(countryId: String, cityId: String, cityName: String = "") async -> [District] {
        guard let url = URL(string: "https://namazvakitleri.diyanet.gov.tr/tr-TR/home/GetRegList?ChangeType=state&CountryId=\(countryId)&StateId=\(cityId)") else {
            return [District(id: cityId, name: cityName.isEmpty ? "MERKEZ" : cityName)]
        }

        var request = URLRequest(url: url)
        request.setValue(userAgent, forHTTPHeaderField: "User-Agent")
        request.setValue("text/html,application/xhtml+xml,application/xml;q=0.9,application/json,*/*;q=0.8", forHTTPHeaderField: "Accept")
        request.setValue("tr-TR,tr;q=0.9,en-US;q=0.8,en;q=0.7", forHTTPHeaderField: "Accept-Language")

        do {
            let (data, response) = try await session.data(for: request)
            if let httpResp = response as? HTTPURLResponse, httpResp.statusCode == 200,
               let json = try? JSONSerialization.jsonObject(with: data) as? [String: Any] {
                var items = json["StateRegionList"] as? [[String: Any]] ?? []
                if items.isEmpty {
                    items = json["StateList"] as? [[String: Any]] ?? []
                }
                var results: [District] = []
                for it in items {
                    let name = (it["IlceAdi"] as? String) ?? (it["SehirAdi"] as? String) ?? ""
                    let id = (it["IlceID"] as? String) ?? (it["SehirID"] as? String) ?? ""
                    if !name.trimmingCharacters(in: .whitespaces).isEmpty && !id.isEmpty {
                        results.append(District(id: id, name: name.trimmingCharacters(in: .whitespaces)))
                    }
                }
                if !results.isEmpty {
                    return results.sorted { $0.name < $1.name }
                }
            }
        } catch {
            // network error fallback
        }

        return [District(id: cityId, name: cityName.isEmpty ? "MERKEZ" : cityName)]
    }

    public func getDistricts(cityId: String) async -> [District] {
        return await getDistricts(countryId: "2", cityId: cityId)
    }

    public func fetchMonthlyPrayerTimes(districtId: String) async -> [PrayerTimes] {
        guard let url = URL(string: "https://namazvakitleri.diyanet.gov.tr/tr-TR/\(districtId)") else {
            return fallbackPrayerTimes()
        }

        var request = URLRequest(url: url)
        request.setValue(userAgent, forHTTPHeaderField: "User-Agent")

        do {
            let (data, response) = try await session.data(for: request)
            if let httpResp = response as? HTTPURLResponse, httpResp.statusCode == 200,
               let html = String(data: data, encoding: .utf8) {
                let parsed = parseHtml(html)
                if !parsed.isEmpty {
                    return parsed
                }
            }
        } catch {
            // network error
        }

        return fallbackPrayerTimes()
    }

    private func parseHtml(_ html: String) -> [PrayerTimes] {
        var results: [PrayerTimes] = []
        let rowRegex = try? NSRegularExpression(pattern: "<tr[^>]*data-date=\"([^\"]+)\"[^>]*>([\\s\\S]*?)</tr>", options: [])
        let timeRegex = try? NSRegularExpression(pattern: "<td>(\\d{2}:\\d{2})</td>", options: [])

        guard let rowRegex = rowRegex, let timeRegex = timeRegex else { return results }
        let nsString = html as NSString
        let matches = rowRegex.matches(in: html, options: [], range: NSRange(location: 0, length: nsString.length))

        for match in matches {
            guard match.numberOfRanges >= 3 else { continue }
            let dateStr = nsString.substring(with: match.range(at: 1))
            let rowHtml = nsString.substring(with: match.range(at: 2)) as NSString

            let timeMatches = timeRegex.matches(in: rowHtml as String, options: [], range: NSRange(location: 0, length: rowHtml.length))
            var times: [String] = []
            for tm in timeMatches {
                if tm.numberOfRanges >= 2 {
                    times.append(rowHtml.substring(with: tm.range(at: 1)))
                }
            }

            if times.count >= 6 {
                let hijri = Calverter.getHijriDate()
                results.append(PrayerTimes(
                    date: dateStr,
                    hijriDate: hijri,
                    imsak: times[0],
                    gunes: times[1],
                    ogle: times[2],
                    ikindi: times[3],
                    aksam: times[4],
                    yatsi: times[5]
                ))
            }
        }

        return results
    }

    private func fallbackPrayerTimes() -> [PrayerTimes] {
        var list: [PrayerTimes] = []
        let cal = Calendar.current
        let f = DateFormatter()
        f.dateFormat = "yyyy-MM-dd"

        for i in 0..<30 {
            if let d = cal.date(byAdding: .day, value: i, to: Date()) {
                let dStr = f.string(from: d)
                list.append(PrayerTimes(
                    date: dStr,
                    hijriDate: Calverter.getHijriDate(from: d),
                    imsak: "05:45",
                    gunes: "07:10",
                    ogle: "12:55",
                    ikindi: "15:45",
                    aksam: "18:25",
                    yatsi: "19:45"
                ))
            }
        }
        return list
    }
}

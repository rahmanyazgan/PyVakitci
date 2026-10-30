package com.rahmanyazgan.pyvakitci.data.remote

import com.rahmanyazgan.pyvakitci.data.model.City
import com.rahmanyazgan.pyvakitci.data.model.Country
import com.rahmanyazgan.pyvakitci.data.model.District
import com.rahmanyazgan.pyvakitci.data.model.PrayerTimes
import com.rahmanyazgan.pyvakitci.util.Calverter
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import org.json.JSONObject
import java.util.Calendar
import java.util.concurrent.TimeUnit
import java.util.regex.Pattern

class DiyanetApiService {
    private val client = OkHttpClient.Builder()
        .connectTimeout(12, TimeUnit.SECONDS)
        .readTimeout(15, TimeUnit.SECONDS)
        .followRedirects(true)
        .build()

    private val userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36"

    private val turkiyeSehirleri = listOf(
        City("500", "ADANA"), City("501", "ADIYAMAN"), City("502", "AFYONKARAHİSAR"), City("503", "AĞRI"),
        City("504", "AKSARAY"), City("505", "AMASYA"), City("506", "ANKARA"), City("507", "ANTALYA"),
        City("508", "ARDAHAN"), City("509", "ARTVİN"), City("510", "AYDIN"), City("511", "BALIKESİR"),
        City("512", "BARTIN"), City("513", "BATMAN"), City("514", "BAYBURT"), City("515", "BİLECİK"),
        City("516", "BİNGÖL"), City("517", "BİTLİS"), City("518", "BOLU"), City("519", "BURDUR"),
        City("520", "BURSA"), City("521", "ÇANAKKALE"), City("522", "ÇANKIRI"), City("523", "ÇORUM"),
        City("524", "DENİZLİ"), City("525", "DİYARBAKIR"), City("526", "DÜZCE"), City("527", "EDİRNE"),
        City("528", "ELAZIĞ"), City("529", "ERZİNCAN"), City("530", "ERZURUM"), City("531", "ESKİŞEHİR"),
        City("532", "GAZİANTEP"), City("533", "GİRESUN"), City("534", "GÜMÜŞHANE"), City("535", "HAKKARİ"),
        City("536", "HATAY"), City("537", "IĞDIR"), City("538", "ISPARTA"), City("539", "İSTANBUL"),
        City("540", "İZMİR"), City("541", "KAHRAMANMARAŞ"), City("542", "KARABÜK"), City("543", "KARAMAN"),
        City("544", "KARS"), City("545", "KASTAMONU"), City("546", "KAYSERİ"), City("547", "KİLİS"),
        City("548", "KIRIKKALE"), City("549", "KIRKLARELİ"), City("550", "KIRŞEHİR"), City("551", "KOCAELİ"),
        City("552", "KONYA"), City("553", "KÜTAHYA"), City("554", "MALATYA"), City("555", "MANİSA"),
        City("556", "MARDİN"), City("557", "MERSİN"), City("558", "MUĞLA"), City("559", "MUŞ"),
        City("560", "NEVŞEHİR"), City("561", "NİĞDE"), City("562", "ORDU"), City("563", "OSMANİYE"),
        City("564", "RİZE"), City("565", "SAKARYA"), City("566", "SAMSUN"), City("567", "ŞANLIURFA"),
        City("568", "SİİRT"), City("569", "SİNOP"), City("570", "ŞIRNAK"), City("571", "SİVAS"),
        City("572", "TEKİRDAĞ"), City("573", "TOKAT"), City("574", "TRABZON"), City("575", "TUNCELİ"),
        City("576", "UŞAK"), City("577", "VAN"), City("578", "YALOVA"), City("579", "YOZGAT"),
        City("580", "ZONGULDAK")
    )

    suspend fun getCountries(): List<Country> = withContext(Dispatchers.IO) {
        val list = mutableListOf<Country>()
        list.add(Country("33", "ABD"))
        list.add(Country("166", "AFGANISTAN"))
        list.add(Country("13", "ALMANYA"))
        list.add(Country("17", "ANDORRA"))
        list.add(Country("140", "ANGOLA"))
        list.add(Country("125", "ANGUILLA"))
        list.add(Country("90", "ANTIGUA VE BARBUDA"))
        list.add(Country("199", "ARJANTIN"))
        list.add(Country("25", "ARNAVUTLUK"))
        list.add(Country("153", "ARUBA"))
        list.add(Country("213", "ASCENSION"))
        list.add(Country("1216", "ATLANTİK OKYANUSU"))
        list.add(Country("59", "AVUSTRALYA"))
        list.add(Country("35", "AVUSTURYA"))
        list.add(Country("5", "AZERBAYCAN"))
        list.add(Country("54", "BAHAMALAR"))
        list.add(Country("132", "BAHREYN"))
        list.add(Country("177", "BANGLADES"))
        list.add(Country("188", "BARBADOS"))
        list.add(Country("208", "BELARUS"))
        list.add(Country("11", "BELCIKA"))
        list.add(Country("182", "BELIZE"))
        list.add(Country("181", "BENIN"))
        list.add(Country("51", "BERMUDA"))
        list.add(Country("93", "BIRLESIK ARAP EMIRLIGI"))
        list.add(Country("83", "BOLIVYA"))
        list.add(Country("9", "BOSNA HERSEK"))
        list.add(Country("167", "BOTSVANA"))
        list.add(Country("146", "BREZILYA"))
        list.add(Country("97", "BRUNEI"))
        list.add(Country("44", "BULGARISTAN"))
        list.add(Country("91", "BURKINA FASO"))
        list.add(Country("65", "BURUNDI"))
        list.add(Country("155", "BUTAN"))
        list.add(Country("156", "CAD"))
        list.add(Country("16", "CEK CUMHURIYETI"))
        list.add(Country("86", "CEZAYIR"))
        list.add(Country("160", "CIBUTI"))
        list.add(Country("61", "CIN"))
        list.add(Country("26", "DANIMARKA"))
        list.add(Country("180", "DEMOKRATIK KONGO CUMHURIYETI"))
        list.add(Country("176", "DOGU TIMOR"))
        list.add(Country("123", "DOMINIK"))
        list.add(Country("72", "DOMINIK CUMHURIYETI"))
        list.add(Country("139", "EKVATOR"))
        list.add(Country("63", "EKVATOR GINESI"))
        list.add(Country("165", "EL SALVADOR"))
        list.add(Country("117", "ENDONEZYA"))
        list.add(Country("175", "ERITRE"))
        list.add(Country("104", "ERMENISTAN"))
        list.add(Country("6", "ESTONYA"))
        list.add(Country("170", "ESWATINI"))
        list.add(Country("95", "ETYOPYA"))
        list.add(Country("145", "FAS"))
        list.add(Country("197", "FIJI"))
        list.add(Country("120", "FILDISI SAHILI"))
        list.add(Country("126", "FILIPINLER"))
        list.add(Country("204", "FILISTIN"))
        list.add(Country("41", "FINLANDIYA"))
        list.add(Country("21", "FRANSA"))
        list.add(Country("79", "GABON"))
        list.add(Country("109", "GAMBIYA"))
        list.add(Country("143", "GANA"))
        list.add(Country("111", "GINE"))
        list.add(Country("58", "GRENADA"))
        list.add(Country("48", "GRONLAND"))
        list.add(Country("171", "GUADELOPE"))
        list.add(Country("169", "GUAM ADASI"))
        list.add(Country("99", "GUATEMALA"))
        list.add(Country("67", "GUNEY AFRIKA"))
        list.add(Country("128", "GUNEY KORE"))
        list.add(Country("214", "GUNEY SUDAN"))
        list.add(Country("62", "GURCISTAN"))
        list.add(Country("82", "GUYANA"))
        list.add(Country("70", "HAITI"))
        list.add(Country("187", "HINDISTAN"))
        list.add(Country("30", "HIRVATISTAN"))
        list.add(Country("4", "HOLLANDA"))
        list.add(Country("105", "HONDURAS"))
        list.add(Country("113", "HONG KONG"))
        list.add(Country("15", "INGILTERE"))
        list.add(Country("124", "IRAK"))
        list.add(Country("202", "IRAN"))
        list.add(Country("32", "IRLANDA"))
        list.add(Country("23", "ISPANYA"))
        list.add(Country("205", "ISRAIL"))
        list.add(Country("12", "ISVEC"))
        list.add(Country("49", "ISVICRE"))
        list.add(Country("8", "ITALYA"))
        list.add(Country("122", "IZLANDA"))
        list.add(Country("119", "JAMAIKA"))
        list.add(Country("116", "JAPONYA"))
        list.add(Country("161", "KAMBOCYA"))
        list.add(Country("184", "KAMERUN"))
        list.add(Country("52", "KANADA"))
        list.add(Country("34", "KARADAG"))
        list.add(Country("94", "KATAR"))
        list.add(Country("92", "KAZAKISTAN"))
        list.add(Country("114", "KENYA"))
        list.add(Country("168", "KIRGIZISTAN"))
        list.add(Country("57", "KOLOMBIYA"))
        list.add(Country("88", "KOMORLAR"))
        list.add(Country("18", "KOSOVA"))
        list.add(Country("162", "KOSTARIKA"))
        list.add(Country("209", "KUBA"))
        list.add(Country("206", "KUDUS"))
        list.add(Country("133", "KUVEYT"))
        list.add(Country("1", "KUZEY KIBRIS"))
        list.add(Country("142", "KUZEY KORE"))
        list.add(Country("134", "LAOS"))
        list.add(Country("174", "LESOTO"))
        list.add(Country("20", "LETONYA"))
        list.add(Country("73", "LIBERYA"))
        list.add(Country("203", "LIBYA"))
        list.add(Country("38", "LIECHTENSTEIN"))
        list.add(Country("47", "LITVANYA"))
        list.add(Country("42", "LUBNAN"))
        list.add(Country("31", "LUKSEMBURG"))
        list.add(Country("7", "MACARISTAN"))
        list.add(Country("98", "MADAGASKAR"))
        list.add(Country("100", "MAKAO"))
        list.add(Country("28", "MAKEDONYA"))
        list.add(Country("55", "MALAVI"))
        list.add(Country("103", "MALDIVLER"))
        list.add(Country("107", "MALEZYA"))
        list.add(Country("152", "MALI"))
        list.add(Country("24", "MALTA"))
        list.add(Country("212", "MAN ADASI"))
        list.add(Country("87", "MARTINIK"))
        list.add(Country("164", "MAURITIUS ADASI"))
        list.add(Country("157", "MAYOTTE"))
        list.add(Country("53", "MEKSIKA"))
        list.add(Country("85", "MIKRONEZYA"))
        list.add(Country("189", "MISIR"))
        list.add(Country("60", "MOGOLISTAN"))
        list.add(Country("46", "MOLDAVYA"))
        list.add(Country("3", "MONAKO"))
        list.add(Country("147", "MONTSERRAT (U.K.)"))
        list.add(Country("106", "MORITANYA"))
        list.add(Country("151", "MOZAMBIK"))
        list.add(Country("154", "MYANMAR"))
        list.add(Country("196", "NAMIBYA"))
        list.add(Country("76", "NEPAL"))
        list.add(Country("84", "NIJER"))
        list.add(Country("127", "NIJERYA"))
        list.add(Country("141", "NIKARAGUA"))
        list.add(Country("178", "NIUE"))
        list.add(Country("36", "NORVEC"))
        list.add(Country("80", "ORTA AFRIKA CUMHURIYETI"))
        list.add(Country("131", "OZBEKISTAN"))
        list.add(Country("77", "PAKISTAN"))
        list.add(Country("149", "PALAU"))
        list.add(Country("89", "PANAMA"))
        list.add(Country("185", "PAPUA YENI GINE"))
        list.add(Country("194", "PARAGUAY"))
        list.add(Country("69", "PERU"))
        list.add(Country("183", "PITCAIRN ADASI"))
        list.add(Country("39", "POLONYA"))
        list.add(Country("45", "PORTEKIZ"))
        list.add(Country("68", "PORTO RIKO"))
        list.add(Country("112", "REUNION"))
        list.add(Country("37", "ROMANYA"))
        list.add(Country("81", "RUANDA"))
        list.add(Country("207", "RUSYA"))
        list.add(Country("64", "S. ARABISTAN"))
        list.add(Country("198", "SAMOA"))
        list.add(Country("102", "SENEGAL"))
        list.add(Country("138", "SEYSEL ADALARI"))
        list.add(Country("210", "SIERRA LEONE"))
        list.add(Country("200", "SILI"))
        list.add(Country("179", "SINGAPUR"))
        list.add(Country("27", "SIRBISTAN"))
        list.add(Country("14", "SLOVAKYA"))
        list.add(Country("19", "SLOVENYA"))
        list.add(Country("150", "SOMALI"))
        list.add(Country("74", "SRI LANKA"))
        list.add(Country("136", "ST. LUCIA"))
        list.add(Country("129", "SUDAN"))
        list.add(Country("172", "SURINAM"))
        list.add(Country("191", "SURIYE"))
        list.add(Country("163", "SVALBARD"))
        list.add(Country("101", "TACIKISTAN"))
        list.add(Country("110", "TANZANYA"))
        list.add(Country("137", "TAYLAND"))
        list.add(Country("108", "TAYVAN"))
        list.add(Country("71", "TOGO"))
        list.add(Country("130", "TONGA"))
        list.add(Country("96", "TRINIDAT VE TOBAGO"))
        list.add(Country("118", "TUNUS"))
        list.add(Country("159", "TURKMENISTAN"))
        list.add(Country("2", "TÜRKİYE"))
        list.add(Country("75", "UGANDA"))
        list.add(Country("40", "UKRAYNA"))
        list.add(Country("29", "UKRAYNA-KIRIM"))
        list.add(Country("173", "UMMAN"))
        list.add(Country("192", "URDUN"))
        list.add(Country("201", "URUGUAY"))
        list.add(Country("56", "VANUATU"))
        list.add(Country("10", "VATIKAN"))
        list.add(Country("186", "VENEZUELA"))
        list.add(Country("135", "VIETNAM"))
        list.add(Country("148", "YEMEN"))
        list.add(Country("115", "YENI KALEDONYA"))
        list.add(Country("193", "YENI ZELANDA"))
        list.add(Country("144", "YESIL BURUN"))
        list.add(Country("22", "YUNANISTAN"))
        list.add(Country("158", "ZAMBIYA"))
        list.add(Country("216", "ZIMBABVE"))
        list.add(Country("66", "ÇURAÇAO"))
        list.sortBy { it.name }
        list
    }

    suspend fun getCities(countryId: String): List<City> = withContext(Dispatchers.IO) {
        if (countryId == "2") {
            return@withContext turkiyeSehirleri
        }
        val cities = mutableListOf<City>()
        try {
            val url = "https://namazvakitleri.diyanet.gov.tr/tr-TR/home/GetRegList?ChangeType=country&CountryId=$countryId"
            val request = Request.Builder()
                .url(url)
                .header("User-Agent", userAgent)
                .header("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,application/json,*/*;q=0.8")
                .header("Accept-Language", "tr-TR,tr;q=0.9,en-US;q=0.8,en;q=0.7")
                .build()

            client.newCall(request).execute().use { response ->
                if (response.isSuccessful) {
                    val body = response.body?.string() ?: ""
                    val json = JSONObject(body)
                    var arr = json.optJSONArray("StateList")
                    if (arr == null || arr.length() == 0) {
                        arr = json.optJSONArray("StateRegionList")
                    }
                    if (arr != null) {
                        for (i in 0 until arr.length()) {
                            val obj = arr.getJSONObject(i)
                            var name = obj.optString("SehirAdi", "")
                            if (name.isEmpty()) name = obj.optString("IlceAdi", "")
                            var id = obj.optString("SehirID", "")
                            if (id.isEmpty()) id = obj.optString("IlceID", "")
                            if (name.isNotEmpty() && id.isNotEmpty()) {
                                cities.add(City(id, name.trim()))
                            }
                        }
                    }
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
        if (cities.isEmpty()) {
            cities.add(City("1", "MERKEZ"))
        }
        cities.sortedBy { it.name }
    }

    suspend fun getDistricts(countryId: String, cityId: String, cityName: String = ""): List<District> = withContext(Dispatchers.IO) {
        val districts = mutableListOf<District>()
        try {
            val url = "https://namazvakitleri.diyanet.gov.tr/tr-TR/home/GetRegList?ChangeType=state&CountryId=$countryId&StateId=$cityId"
            val request = Request.Builder()
                .url(url)
                .header("User-Agent", userAgent)
                .header("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,application/json,*/*;q=0.8")
                .header("Accept-Language", "tr-TR,tr;q=0.9,en-US;q=0.8,en;q=0.7")
                .build()

            client.newCall(request).execute().use { response ->
                if (response.isSuccessful) {
                    val body = response.body?.string() ?: ""
                    val json = JSONObject(body)
                    var arr = json.optJSONArray("StateRegionList")
                    if (arr == null || arr.length() == 0) {
                        arr = json.optJSONArray("StateList")
                    }
                    if (arr != null) {
                        for (i in 0 until arr.length()) {
                            val obj = arr.getJSONObject(i)
                            var name = obj.optString("IlceAdi", "")
                            if (name.isEmpty()) name = obj.optString("SehirAdi", "")
                            var id = obj.optString("IlceID", "")
                            if (id.isEmpty()) id = obj.optString("SehirID", "")
                            if (name.isNotEmpty() && id.isNotEmpty()) {
                                districts.add(District(id, name.trim()))
                            }
                        }
                    }
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
        if (districts.isEmpty()) {
            districts.add(District(cityId, if (cityName.isNotEmpty()) cityName else "MERKEZ"))
        }
        districts.sortedBy { it.name }
    }

    suspend fun getDistricts(cityId: String): List<District> {
        return getDistricts("2", cityId)
    }

    private val monthMap = mapOf(
        "Ocak" to "01", "Şubat" to "02", "Mart" to "03", "Nisan" to "04",
        "Mayıs" to "05", "Haziran" to "06", "Temmuz" to "07", "Ağustos" to "08",
        "Eylül" to "09", "Ekim" to "10", "Kasım" to "11", "Aralık" to "12"
    )

    private fun parseMiladiDate(dateStr: String): String? {
        val clean = dateStr.replace(Regex("<[^>]*>"), "").trim()
        val parts = clean.split(Regex("\\s+"))
        if (parts.size >= 3) {
            val day = parts[0].padStart(2, '0')
            val month = monthMap[parts[1]] ?: return null
            val year = parts[2]
            return "$year-$month-$day"
        }
        return null
    }

    suspend fun fetchMonthlyTimes(districtId: String): List<PrayerTimes> = withContext(Dispatchers.IO) {
        val url = "https://namazvakitleri.diyanet.gov.tr/tr-TR/$districtId"
        val request = Request.Builder()
            .url(url)
            .header("User-Agent", userAgent)
            .header("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8")
            .header("Accept-Language", "tr-TR,tr;q=0.9,en-US;q=0.8,en;q=0.7")
            .build()

        val result = mutableListOf<PrayerTimes>()
        val seenDates = mutableSetOf<String>()

        try {
            client.newCall(request).execute().use { response ->
                if (response.isSuccessful) {
                    val html = response.body?.string() ?: ""

                    val trPattern = Pattern.compile("<tr[^>]*>([\\s\\S]*?)</tr>")
                    val tdPattern = Pattern.compile("<td[^>]*>([\\s\\S]*?)</td>")
                    val trMatcher = trPattern.matcher(html)

                    while (trMatcher.find()) {
                        val rowHtml = trMatcher.group(1) ?: continue
                        val tdMatcher = tdPattern.matcher(rowHtml)
                        val cells = mutableListOf<String>()
                        while (tdMatcher.find()) {
                            val cellText = (tdMatcher.group(1) ?: "")
                                .replace(Regex("<[^>]*>"), "")
                                .trim()
                            cells.add(cellText)
                        }

                        if (cells.size >= 8) {
                            val formattedDate = parseMiladiDate(cells[0]) ?: continue
                            if (seenDates.contains(formattedDate)) continue
                            seenDates.add(formattedDate)

                            val hijri = cells[1]
                            val imsak = cells[2]
                            val gunes = cells[3]
                            val ogle = cells[4]
                            val ikindi = cells[5]
                            val aksam = cells[6]
                            val yatsi = cells[7]

                            result.add(
                                PrayerTimes(
                                    date = formattedDate,
                                    hijriDate = hijri,
                                    imsak = imsak,
                                    gunes = gunes,
                                    ogle = ogle,
                                    ikindi = ikindi,
                                    aksam = aksam,
                                    yatsi = yatsi
                                )
                            )
                        } else if (cells.size == 7) {
                            val formattedDate = parseMiladiDate(cells[0]) ?: continue
                            if (seenDates.contains(formattedDate)) continue
                            seenDates.add(formattedDate)

                            result.add(
                                PrayerTimes(
                                    date = formattedDate,
                                    hijriDate = "",
                                    imsak = cells[1],
                                    gunes = cells[2],
                                    ogle = cells[3],
                                    ikindi = cells[4],
                                    aksam = cells[5],
                                    yatsi = cells[6]
                                )
                            )
                        }
                    }
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }

        result
    }
}

#include "DiyanetUlkeler.h"
#include "HttpClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString DiyanetUlkeler::BASE_URL = "https://namazvakitleri.diyanet.gov.tr";
const QString DiyanetUlkeler::REQUEST_URL = DiyanetUlkeler::BASE_URL + "/tr-TR/home/GetRegList?";

const QMap<QString, int>& DiyanetUlkeler::getUlkeler() {
    static const QMap<QString, int> ulkeler = {
        {"ABD", 33}, {"AFGANISTAN", 166}, {"ALMANYA", 13}, {"ANDORRA", 17}, {"ANGOLA", 140},
        {"ANGUILLA", 125}, {"ANTIGUA VE BARBUDA", 90}, {"ARJANTIN", 199}, {"ARNAVUTLUK", 25},
        {"ARUBA", 153}, {"ASCENSION", 213}, {"ATLANTİK OKYANUSU", 1216}, {"AVUSTRALYA", 59},
        {"AVUSTURYA", 35}, {"AZERBAYCAN", 5}, {"BAHAMALAR", 54}, {"BAHREYN", 132},
        {"BANGLADES", 177}, {"BARBADOS", 188}, {"BELARUS", 208}, {"BELCIKA", 11},
        {"BELIZE", 182}, {"BENIN", 181}, {"BERMUDA", 51}, {"BIRLESIK ARAP EMIRLIGI", 93},
        {"BOLIVYA", 83}, {"BOSNA HERSEK", 9}, {"BOTSVANA", 167}, {"BREZILYA", 146},
        {"BRUNEI", 97}, {"BULGARISTAN", 44}, {"BURKINA FASO", 91}, {"BURUNDI", 65},
        {"BUTAN", 155}, {"CAD", 156}, {"CEK CUMHURIYETI", 16}, {"CEZAYIR", 86},
        {"CIBUTI", 160}, {"CIN", 61}, {"ÇURAÇAO", 66}, {"DANIMARKA", 26},
        {"DEMOKRATIK KONGO CUMHURIYETI", 180}, {"DOGU TIMOR", 176}, {"DOMINIK", 123},
        {"DOMINIK CUMHURIYETI", 72}, {"EKVATOR", 139}, {"EKVATOR GINESI", 63},
        {"EL SALVADOR", 165}, {"ENDONEZYA", 117}, {"ERITRE", 175}, {"ERMENISTAN", 104},
        {"ESTONYA", 6}, {"ESWATINI", 170}, {"ETYOPYA", 95}, {"FAS", 145}, {"FIJI", 197},
        {"FILDISI SAHILI", 120}, {"FILIPINLER", 126}, {"FILISTIN", 204}, {"FINLANDIYA", 41},
        {"FRANSA", 21}, {"GABON", 79}, {"GAMBIYA", 109}, {"GANA", 143}, {"GINE", 111},
        {"GRENADA", 58}, {"GRONLAND", 48}, {"GUADELOPE", 171}, {"GUAM ADASI", 169},
        {"GUATEMALA", 99}, {"GUNEY AFRIKA", 67}, {"GUNEY KORE", 128}, {"GUNEY SUDAN", 214},
        {"GURCISTAN", 62}, {"GUYANA", 82}, {"HAITI", 70}, {"HINDISTAN", 187},
        {"HIRVATISTAN", 30}, {"HOLLANDA", 4}, {"HONDURAS", 105}, {"HONG KONG", 113},
        {"INGILTERE", 15}, {"IRAK", 124}, {"IRAN", 202}, {"IRLANDA", 32}, {"ISPANYA", 23},
        {"ISRAIL", 205}, {"ISVEC", 12}, {"ISVICRE", 49}, {"ITALYA", 8}, {"IZLANDA", 122},
        {"JAMAIKA", 119}, {"JAPONYA", 116}, {"KAMBOCYA", 161}, {"KAMERUN", 184},
        {"KANADA", 52}, {"KARADAG", 34}, {"KATAR", 94}, {"KAZAKISTAN", 92}, {"KENYA", 114},
        {"KIRGIZISTAN", 168}, {"KOLOMBIYA", 57}, {"KOMORLAR", 88}, {"KOSOVA", 18},
        {"KOSTARIKA", 162}, {"KUBA", 209}, {"KUDUS", 206}, {"KUVEYT", 133},
        {"KUZEY KIBRIS", 1}, {"KUZEY KORE", 142}, {"LAOS", 134}, {"LESOTO", 174},
        {"LETONYA", 20}, {"LIBERYA", 73}, {"LIBYA", 203}, {"LIECHTENSTEIN", 38},
        {"LITVANYA", 47}, {"LUBNAN", 42}, {"LUKSEMBURG", 31}, {"MACARISTAN", 7},
        {"MADAGASKAR", 98}, {"MAKAO", 100}, {"MAKEDONYA", 28}, {"MALAVI", 55},
        {"MALDIVLER", 103}, {"MALEZYA", 107}, {"MALI", 152}, {"MALTA", 24},
        {"MAN ADASI", 212}, {"MARTINIK", 87}, {"MAURITIUS ADASI", 164}, {"MAYOTTE", 157},
        {"MEKSIKA", 53}, {"MIKRONEZYA", 85}, {"MISIR", 189}, {"MOGOLISTAN", 60},
        {"MOLDAVYA", 46}, {"MONAKO", 3}, {"MONTSERRAT (U.K.)", 147}, {"MORITANYA", 106},
        {"MOZAMBIK", 151}, {"MYANMAR", 154}, {"NAMIBYA", 196}, {"NEPAL", 76},
        {"NIJER", 84}, {"NIJERYA", 127}, {"NIKARAGUA", 141}, {"NIUE", 178},
        {"NORVEC", 36}, {"ORTA AFRIKA CUMHURIYETI", 80}, {"OZBEKISTAN", 131},
        {"PAKISTAN", 77}, {"PALAU", 149}, {"PANAMA", 89}, {"PAPUA YENI GINE", 185},
        {"PARAGUAY", 194}, {"PERU", 69}, {"PITCAIRN ADASI", 183}, {"POLONYA", 39},
        {"PORTEKIZ", 45}, {"PORTO RIKO", 68}, {"REUNION", 112}, {"ROMANYA", 37},
        {"RUANDA", 81}, {"RUSYA", 207}, {"S. ARABISTAN", 64}, {"SAMOA", 198},
        {"SENEGAL", 102}, {"SEYSEL ADALARI", 138}, {"SIERRA LEONE", 210}, {"SILI", 200},
        {"SINGAPUR", 179}, {"SIRBISTAN", 27}, {"SLOVAKYA", 14}, {"SLOVENYA", 19},
        {"SOMALI", 150}, {"SRI LANKA", 74}, {"ST. LUCIA", 136}, {"SUDAN", 129},
        {"SURINAM", 172}, {"SURIYE", 191}, {"SVALBARD", 163}, {"TACIKISTAN", 101},
        {"TANZANYA", 110}, {"TAYLAND", 137}, {"TAYVAN", 108}, {"TOGO", 71},
        {"TONGA", 130}, {"TRINIDAT VE TOBAGO", 96}, {"TUNUS", 118}, {"TÜRKİYE", 2},
        {"TURKMENISTAN", 159}, {"UGANDA", 75}, {"UKRAYNA", 40}, {"UKRAYNA-KIRIM", 29},
        {"UMMAN", 173}, {"URDUN", 192}, {"URUGUAY", 201}, {"VANUATU", 56},
        {"VATIKAN", 10}, {"VENEZUELA", 186}, {"VIETNAM", 135}, {"YEMEN", 148},
        {"YENI KALEDONYA", 115}, {"YENI ZELANDA", 193}, {"YESIL BURUN", 144},
        {"YUNANISTAN", 22}, {"ZAMBIYA", 158}, {"ZIMBABVE", 216}
    };
    return ulkeler;
}

const QMap<QString, int>& DiyanetUlkeler::getTurkiyeSehirler() {
    static const QMap<QString, int> sehirler = {
        {"ADANA", 500}, {"ADIYAMAN", 501}, {"AFYONKARAHİSAR", 502}, {"AĞRI", 503}, {"AKSARAY", 504},
        {"AMASYA", 505}, {"ANKARA", 506}, {"ANTALYA", 507}, {"ARDAHAN", 508}, {"ARTVİN", 509},
        {"AYDIN", 510}, {"BALIKESİR", 511}, {"BARTIN", 512}, {"BATMAN", 513}, {"BAYBURT", 514},
        {"BİLECİK", 515}, {"BİNGÖL", 516}, {"BİTLİS", 517}, {"BOLU", 518}, {"BURDUR", 519},
        {"BURSA", 520}, {"ÇANAKKALE", 521}, {"ÇANKIRI", 522}, {"ÇORUM", 523}, {"DENİZLİ", 524},
        {"DİYARBAKIR", 525}, {"DÜZCE", 526}, {"EDİRNE", 527}, {"ELAZIĞ", 528}, {"ERZİNCAN", 529},
        {"ERZURUM", 530}, {"ESKİŞEHİR", 531}, {"GAZİANTEP", 532}, {"GİRESUN", 533}, {"GÜMÜŞHANE", 534},
        {"HAKKARİ", 535}, {"HATAY", 536}, {"IĞDIR", 537}, {"ISPARTA", 538}, {"İSTANBUL", 539},
        {"İZMİR", 540}, {"KAHRAMANMARAŞ", 541}, {"KARABÜK", 542}, {"KARAMAN", 543}, {"KARS", 544},
        {"KASTAMONU", 545}, {"KAYSERİ", 546}, {"KİLİS", 547}, {"KIRIKKALE", 548}, {"KIRKLARELİ", 549},
        {"KIRŞEHİR", 550}, {"KOCAELİ", 551}, {"KONYA", 552}, {"KÜTAHYA", 553}, {"MALATYA", 554},
        {"MANİSA", 555}, {"MARDİN", 556}, {"MERSİN", 557}, {"MUĞLA", 558}, {"MUŞ", 559},
        {"NEVŞEHİR", 560}, {"NİĞDE", 561}, {"ORDU", 562}, {"OSMANİYE", 563}, {"RİZE", 564},
        {"SAKARYA", 565}, {"SAMSUN", 566}, {"ŞANLIURFA", 567}, {"SİİRT", 568}, {"SİNOP", 569},
        {"ŞIRNAK", 570}, {"SİVAS", 571}, {"TEKİRDAĞ", 572}, {"TOKAT", 573}, {"TRABZON", 574},
        {"TUNCELİ", 575}, {"UŞAK", 576}, {"VAN", 577}, {"YALOVA", 578}, {"YOZGAT", 579},
        {"ZONGULDAK", 580}
    };
    return sehirler;
}

QMap<QString, int> DiyanetUlkeler::sehirleriAl(const QString& ulke, QSettings* settings) {
    QMap<QString, int> sonuc;
    if (ulke == "TÜRKİYE") {
        sonuc = getTurkiyeSehirler();
    } else {
        int ulkeId = getUlkeler().value(ulke, -1);
        if (ulkeId > 0) {
            QString url = QString("%1ChangeType=country&CountryId=%2").arg(REQUEST_URL).arg(ulkeId);
            bool ok = false;
            QByteArray raw = HttpClient::get(url, 8000, &ok);
            if (ok && !raw.isEmpty()) {
                QJsonDocument doc = QJsonDocument::fromJson(raw);
                if (doc.isObject()) {
                    QJsonObject root = doc.object();
                    QJsonArray arr = root.value("StateList").toArray();
                    if (arr.isEmpty()) {
                        arr = root.value("StateRegionList").toArray();
                    }
                    for (const QJsonValue& val : arr) {
                        QJsonObject obj = val.toObject();
                        QString ad = obj.value("SehirAdi").toString().trimmed();
                        if (ad.isEmpty()) {
                            ad = obj.value("IlceAdi").toString().trimmed();
                        }
                        int id = obj.value("SehirID").toString().toInt();
                        if (id == 0) id = obj.value("SehirID").toInt();
                        if (id == 0) id = obj.value("IlceID").toString().toInt();
                        if (id == 0) id = obj.value("IlceID").toInt();
                        if (!ad.isEmpty() && id > 0) {
                            sonuc.insert(ad, id);
                        }
                    }
                }
            }
        }
    }

    if (settings) {
        QString cacheKey = QString("Diyanet/sehirler_%1").arg(ulke);
        if (!sonuc.isEmpty()) {
            QJsonObject cacheObj;
            for (auto it = sonuc.begin(); it != sonuc.end(); ++it) {
                cacheObj.insert(it.key(), it.value());
            }
            settings->setValue(cacheKey, QString::fromUtf8(QJsonDocument(cacheObj).toJson(QJsonDocument::Compact)));
        } else {
            QString cached = settings->value(cacheKey).toString();
            if (cached.isEmpty() && ulke == "TÜRKİYE") {
                cached = settings->value("Diyanet/sehirler_json").toString();
            }
            if (!cached.isEmpty()) {
                QJsonDocument doc = QJsonDocument::fromJson(cached.toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    for (auto it = obj.begin(); it != obj.end(); ++it) {
                        sonuc.insert(it.key(), it.value().toInt());
                    }
                }
            }
        }
    }

    return sonuc;
}

IlceBilgiPaketi DiyanetUlkeler::ilceleriAl(const QMap<QString, int>& sehirler, const QString& ulke, const QString& sehir, QSettings* settings) {
    IlceBilgiPaketi paket;
    int ulkeId = getUlkeler().value(ulke, -1);
    int sehirId = sehirler.value(sehir, -1);

    if (ulkeId > 0 && sehirId > 0) {
        QString url = QString("%1ChangeType=state&CountryId=%2&StateId=%3").arg(REQUEST_URL).arg(ulkeId).arg(sehirId);
        bool ok = false;
        QByteArray raw = HttpClient::get(url, 8000, &ok);
        if (ok && !raw.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(raw);
            if (doc.isObject()) {
                QJsonObject root = doc.object();
                QJsonArray arr = root.value("StateRegionList").toArray();
                if (arr.isEmpty()) {
                    arr = root.value("StateList").toArray();
                }
                for (const QJsonValue& val : arr) {
                    QJsonObject obj = val.toObject();
                    QString ad = obj.value("IlceAdi").toString().trimmed();
                    if (ad.isEmpty()) {
                        ad = obj.value("SehirAdi").toString().trimmed();
                    }
                    int id = obj.value("IlceID").toString().toInt();
                    if (id == 0) id = obj.value("IlceID").toInt();
                    if (id == 0) id = obj.value("SehirID").toString().toInt();
                    if (id == 0) id = obj.value("SehirID").toInt();
                    QString ilceUrl = obj.value("IlceUrl").toString();
                    if (!ad.isEmpty() && id > 0) {
                        paket.ilceler.insert(ad, id);
                        paket.linkler.insert(ad, ilceUrl);
                    }
                }
            }
        }
    }

    if (paket.ilceler.isEmpty() && sehirId > 0 && !sehir.isEmpty()) {
        paket.ilceler.insert(sehir, sehirId);
        paket.linkler.insert(sehir, QString("/tr-TR/%1/%2-icin-namaz-vakti").arg(sehirId).arg(sehir.toLower()));
    }

    if (settings) {
        QString cacheIlceKey = QString("Diyanet/ilceler_%1_%2").arg(ulke, sehir);
        QString cacheLinkKey = QString("Diyanet/linkler_%1_%2").arg(ulke, sehir);
        if (!paket.ilceler.isEmpty()) {
            QJsonObject ilceObj, linkObj;
            for (auto it = paket.ilceler.begin(); it != paket.ilceler.end(); ++it) {
                ilceObj.insert(it.key(), it.value());
            }
            for (auto it = paket.linkler.begin(); it != paket.linkler.end(); ++it) {
                linkObj.insert(it.key(), it.value());
            }
            settings->setValue(cacheIlceKey, QString::fromUtf8(QJsonDocument(ilceObj).toJson(QJsonDocument::Compact)));
            settings->setValue(cacheLinkKey, QString::fromUtf8(QJsonDocument(linkObj).toJson(QJsonDocument::Compact)));
        } else {
            QString cachedIlce = settings->value(cacheIlceKey).toString();
            QString cachedLink = settings->value(cacheLinkKey).toString();
            if (cachedIlce.isEmpty() && ulke == "TÜRKİYE") {
                cachedIlce = settings->value("Diyanet/ilceler_json").toString();
                cachedLink = settings->value("Diyanet/linkler_json").toString();
            }
            if (!cachedIlce.isEmpty()) {
                QJsonObject obj = QJsonDocument::fromJson(cachedIlce.toUtf8()).object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    paket.ilceler.insert(it.key(), it.value().toInt());
                }
            }
            if (!cachedLink.isEmpty()) {
                QJsonObject obj = QJsonDocument::fromJson(cachedLink.toUtf8()).object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    paket.linkler.insert(it.key(), it.value().toString());
                }
            }
        }
    }

    return paket;
}

#include "Calverter.h"
#include <algorithm>

Calverter::Calverter() {
}

bool Calverter::leapGregorian(int year) const {
    return ((year % 4) == 0) && (!(((year % 100) == 0) && ((year % 400) != 0)));
}

double Calverter::gregorianToJd(int year, int month, int day) const {
    double tm = (month <= 2) ? 0.0 : (leapGregorian(year) ? -1.0 : -2.0);
    return (GREGORIAN_EPOCH - 1.0) +
           (365.0 * (year - 1)) +
           std::floor((year - 1) / 4.0) +
           (-std::floor((year - 1) / 100.0)) +
           std::floor((year - 1) / 400.0) +
           std::floor((((367.0 * month) - 362.0) / 12.0) + tm + day);
}

std::tuple<int, int, int> Calverter::jdToGregorian(double jd) const {
    double wjd = std::floor(jd - 0.5) + 0.5;
    double depoch = wjd - GREGORIAN_EPOCH;
    double quadricent = std::floor(depoch / 146097.0);
    double dqc = std::fmod(depoch, 146097.0);
    if (dqc < 0) dqc += 146097.0;
    double cent = std::floor(dqc / 36524.0);
    double dcent = std::fmod(dqc, 36524.0);
    if (dcent < 0) dcent += 36524.0;
    double quad = std::floor(dcent / 1461.0);
    double dquad = std::fmod(dcent, 1461.0);
    if (dquad < 0) dquad += 1461.0;
    double yindex = std::floor(dquad / 365.0);
    int year = static_cast<int>((quadricent * 400.0) + (cent * 100.0) + (quad * 4.0) + yindex);
    if (!(cent == 4.0 || yindex == 4.0)) {
        year += 1;
    }
    double yearday = wjd - gregorianToJd(year, 1, 1);
    double leapadj = (wjd < gregorianToJd(year, 3, 1)) ? 0.0 : (leapGregorian(year) ? 1.0 : 2.0);
    int month = static_cast<int>(std::floor((((yearday + leapadj) * 12.0) + 373.0) / 367.0));
    int day = static_cast<int>(wjd - gregorianToJd(year, month, 1)) + 1;
    return {year, month, day};
}

bool Calverter::leapIslamic(int year) const {
    return (((year * 11) + 14) % 30) < 11;
}

double Calverter::islamicToJd(int year, int month, int day) const {
    return (day + std::ceil(29.5 * (month - 1)) + (year - 1) * 354.0 +
            std::floor((3.0 + (11.0 * year)) / 30.0) + ISLAMIC_EPOCH) - 1.0;
}

std::tuple<int, int, int> Calverter::jdToIslamic(double jd) const {
    double cleanJd = std::floor(jd) + 0.5;
    int year = static_cast<int>(std::floor(((30.0 * (cleanJd - ISLAMIC_EPOCH)) + 10646.0) / 10631.0));
    int month = static_cast<int>(std::min(12.0, std::ceil((cleanJd - (29.0 + islamicToJd(year, 1, 1))) / 29.5) + 1.0));
    int day = static_cast<int>(cleanJd - islamicToJd(year, month, 1)) + 1;
    return {year, month, day};
}


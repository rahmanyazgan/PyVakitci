#ifndef CALVERTER_H
#define CALVERTER_H

#include <cmath>
#include <tuple>

class Calverter {
public:
    Calverter();

    static constexpr double GREGORIAN_EPOCH = 1721425.5;
    static constexpr double ISLAMIC_EPOCH   = 1948439.5;

    bool leapGregorian(int year) const;
    double gregorianToJd(int year, int month, int day) const;
    std::tuple<int, int, int> jdToGregorian(double jd) const;

    bool leapIslamic(int year) const;
    double islamicToJd(int year, int month, int day) const;
    std::tuple<int, int, int> jdToIslamic(double jd) const;
};

#endif // CALVERTER_H


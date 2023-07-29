
#ifndef PRICING_AGGRESSIVITY_H
#define PRICING_AGGRESSIVITY_H


#include <vector>
#include <ctime>
#include <chrono>

#include "../Basic/BasicStruct.h"

using namespace std::literals;
using Clock = std::chrono::system_clock;

struct AggressivityParam {
    AggressivityParam() = default;

    ~AggressivityParam() = default;

    AggressivityParam(double pct, nc::PriceMode mode)
            : pm(mode), pctTime(pct) {}

    AggressivityParam(double pct, double limit)
            : pm(nc::PriceMode::LIMIT), pctTime(pct), limitPrice(limit) {}

    nc::PriceMode pm{nc::PriceMode::NONE};
    double pctTime{1}; //100%
    double limitPrice{0.0};
};

struct Aggressivity {
    Aggressivity() = default;

    ~Aggressivity() = default;

    nc::PriceMode pm{nc::PriceMode::PEG};
    int startSec{0};
    int endSec{0};
    Clock::time_point startTime = std::chrono::system_clock::now();
    Clock::time_point endTime = std::chrono::system_clock::now();

    double limitPrice{0.0};
};

struct AggressivityParamSet {
    std::vector<AggressivityParam> m_vectParam;
};


#endif //PRICING_AGGRESSIVITY_H

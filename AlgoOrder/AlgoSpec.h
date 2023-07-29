
#ifndef PRICING_ALGOSPEC_H
#define PRICING_ALGOSPEC_H

#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>

#include "../SpreadTable.h"
#include "../Api/FutuAPIWrapper.h"
#include "../Basic/BasicStruct.h"
#include "Aggressivity.h"
#include "TWAPSlice.h"
#include "TargetPrice.h"
#include "Intention.h"


struct AlgoSpec {
    AlgoSpec() = default;

    virtual ~AlgoSpec() = default;
};

struct TWAPSpec : AlgoSpec {
    TWAPSpec() = default;

    TWAPSpec(nc::Instrument *inst, nc::OrderSide os, unsigned int mos, double min, unsigned int t)
            : instrument(inst), side(os), min_order_size(mos), minToGo(min), target(t) {};

    virtual ~TWAPSpec() = default;

    nc::Instrument *instrument{nullptr};
    nc::OrderSide side{nc::OrderSide::buy};
    unsigned int min_order_size{};
    double minToGo{};
    unsigned int target{};
};

struct VolOrderSpec : TWAPSpec {
    VolOrderSpec() = default;

    VolOrderSpec(nc::Instrument *ul, nc::Warrant *warrant, nc::OrderSide os, nc::PricingSide ps, unsigned int mos,
                 double min, unsigned int t)
            : TWAPSpec(warrant, os, mos, min, t), underlying(ul), pricingSide(ps) {};

    virtual ~VolOrderSpec() = default;

    nc::Instrument *underlying{nullptr};
    nc::PricingSide pricingSide{nc::PricingSide::BID};
};


#endif //PRICING_ALGOSPEC_H

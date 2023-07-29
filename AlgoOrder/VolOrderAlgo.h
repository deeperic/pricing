
#ifndef PRICING_VOLORDERALGO_H
#define PRICING_VOLORDERALGO_H

#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>

#include "../AlgoOrder/TWAPAlgo.h"

class VolOrderAlgo : public TWAPAlgo {
public:
    VolOrderAlgo(std::size_t id, std::unique_ptr<SpreadTable> st, const AlgoSpec *spec, const AggressivityParamSet &ap,
                 std::unique_ptr<InstrumentOperation> marketDataUpdater);

    virtual ~VolOrderAlgo() = default;

    void InitializeTargetPrice() noexcept override;

private:
    nc::Instrument *m_underlying{nullptr};
    nc::PricingSide m_pricingSide{
            nc::PricingSide::BID}; //the side where the pricing is based e.g. Buy order pricing at Sell side (Ask price at IV(ask))

};


#endif //PRICING_VOLORDERALGO_H

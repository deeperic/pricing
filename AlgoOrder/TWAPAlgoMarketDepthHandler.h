
#ifndef PRICING_TWAPALGOMARKETDEPTHHANDLER_H
#define PRICING_TWAPALGOMARKETDEPTHHANDLER_H


#include "Api/FutuAPIWrapper.h"
#include "Api/FutuEventHandler.h"

class TWAPAlgo;

class TWAPAlgoMarketDepthHandler : public FutuMarketDepthEventHandler {
public:
    explicit TWAPAlgoMarketDepthHandler(TWAPAlgo *algo) :
            m_algo(algo) {};

    virtual ~TWAPAlgoMarketDepthHandler() = default;

    void OnPush_MarketData(const std::string &instrument, MarketDataPriceDepth &mdpd) override;

private:
    TWAPAlgo *m_algo{nullptr};
};


#endif //PRICING_TWAPALGOMARKETDEPTHHANDLER_H

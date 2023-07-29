//
// Created by ceric on 3/21/23.
//

#ifndef PRICING_TWAPALGOMARKETDATAHANDLER_H
#define PRICING_TWAPALGOMARKETDATAHANDLER_H

#include "Api/FutuAPIWrapper.h"
#include "Api/FutuEventHandler.h"

class TWAPAlgo;

class TWAPAlgoMarketDataHandler : public FutuMarketDataEventHandler {
public:

    explicit TWAPAlgoMarketDataHandler(TWAPAlgo *algo) :
            m_algo(algo) {};

    virtual ~TWAPAlgoMarketDataHandler() = default;

    void OnApiQuoteConnect(bool success, Futu::i64_t nErrCode, const char *strDesc) override;

    void
    OnPush_MarketData(const std::string &instrument, double bid, int64_t bvol, int64_t bnum, double ask, int64_t avol,
                      int64_t anum) override;

private:

    TWAPAlgo *m_algo{nullptr};
};


#endif //PRICING_TWAPALGOMARKETDATAHANDLER_H

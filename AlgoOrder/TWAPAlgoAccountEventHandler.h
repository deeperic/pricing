
#ifndef PRICING_TWAPALGOACCOUNTEVENTHANDLER_H
#define PRICING_TWAPALGOACCOUNTEVENTHANDLER_H

#include "Api/FutuAPIWrapper.h"
#include "Api/FutuEventHandler.h"

class TWAPAlgo;

class TWAPAlgoAccountEventHandler : public FutuAccountEventHandler {
public:
    explicit TWAPAlgoAccountEventHandler(TWAPAlgo *algo) :
            m_algo(algo) {};

    virtual ~TWAPAlgoAccountEventHandler() = default;

    void OnAccountFund(const double &cash, const double &totalAssets, const double &marketValue) override;

    void OnPositions(const std::vector<nc::Position> &v) override;

private:
    TWAPAlgo *m_algo{nullptr};
};


#endif //PRICING_TWAPALGOACCOUNTEVENTHANDLER_H

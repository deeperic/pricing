
#ifndef PRICING_INSTRUMENTOPERATION_H
#define PRICING_INSTRUMENTOPERATION_H

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


struct InstrumentOperation {
    InstrumentOperation() = default;

    virtual ~InstrumentOperation() = default;

    virtual void
    UpdateBBO(const std::string &instrument, double bid, int64_t bvol, int64_t bnum, double ask, int64_t avol,
              int64_t anum) = 0;

    virtual void UpdateMarketDepth(const std::string &instrument, MarketDataPriceDepth &mdpd) = 0;

    virtual std::vector<std::string> GetInstrumentsForMarketData() = 0;

    virtual bool CheckStandingOut(double desiredPrice, double desiredSize,
                                  TWAPSlice *pCurrentSlice, nc::OrderSide side, nc::Instrument *m_instrument) = 0;

    virtual bool CheckDifferentFromDesired(double desiredPrice, double desiredSize,
                                           nc::OrderSide side, nc::Instrument *instrument) = 0;
};

struct SimpleInstrumentOperation : InstrumentOperation {
    explicit SimpleInstrumentOperation(nc::Instrument *i)
            : instrument(i) {};

    virtual ~SimpleInstrumentOperation() = default;

    void UpdateBBO(const std::string &inst, double bid, int64_t bvol, int64_t bnum, double ask, int64_t avol,
                   int64_t anum) override;

    void UpdateMarketDepth(const std::string &instrument, MarketDataPriceDepth &mdpd) override;

    std::vector<std::string> GetInstrumentsForMarketData() override;

    bool CheckStandingOut(double desiredPrice, double desiredSize,
                          TWAPSlice *pCurrentSlice, nc::OrderSide side, nc::Instrument *m_instrument) override;

    bool CheckDifferentFromDesired(double desiredPrice, double desiredSize,
                                   nc::OrderSide side, nc::Instrument *instrument) override;

    nc::Instrument *instrument{nullptr};
};

struct WarrantInstrumentOperation : InstrumentOperation {

    WarrantInstrumentOperation(nc::Instrument *i, nc::StockWarrantPtr w)
            : instrument(i), warrant(w) {};

    virtual ~WarrantInstrumentOperation() = default;

    void UpdateBBO(const std::string &inst, double bid, int64_t bvol, int64_t bnum, double ask, int64_t avol,
                   int64_t anum) override;

    void UpdateMarketDepth(const std::string &instrument, MarketDataPriceDepth &mdpd) override;

    std::vector<std::string> GetInstrumentsForMarketData() override;

    bool CheckStandingOut(double desiredPrice, double desiredSize,
                          TWAPSlice *pCurrentSlice, nc::OrderSide side, nc::Instrument *m_instrument) override;

    bool CheckDifferentFromDesired(double desiredPrice, double desiredSize,
                                   nc::OrderSide side, nc::Instrument *instrument) override;

    nc::Instrument *instrument{nullptr};
    nc::StockWarrantPtr warrant{nullptr};
};


#endif //PRICING_INSTRUMENTOPERATION_H

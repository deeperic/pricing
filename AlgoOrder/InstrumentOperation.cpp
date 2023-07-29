#include "InstrumentOperation.h"


void SimpleInstrumentOperation::UpdateBBO(const std::string &inst, double bid, int64_t bvol, int64_t bnum, double ask,
                                          int64_t avol, int64_t anum) {
    std::cout << " SimpleMarketDataUpdater::UpdateBBO " << std::endl;
    if (inst == ncutil::RemovePrefix(instrument->GetCode())) {
        instrument->UpdateBBO(bid, bvol, bnum, ask, avol, anum);
        instrument->SetMarketDataReady();
    }
}

void SimpleInstrumentOperation::UpdateMarketDepth(const std::string &inst, MarketDataPriceDepth &mdpd) {
    std::cout << " SimpleMarketDataUpdater::UpdateMarketDepth " << std::endl;
    if (inst == ncutil::RemovePrefix(instrument->GetCode())) {
        instrument->UpdateMarketDataPriceDepth(mdpd);
        instrument->UpdateMarketDataPriceDepthWithoutMe(mdpd);
    }
}


std::vector<std::string> SimpleInstrumentOperation::GetInstrumentsForMarketData() {
    std::cout << " SimpleInstrumentOperation:: GetInstrumentsForMarketData " << instrument->GetCode() << std::endl;
    std::vector<std::string> v;
    v.push_back(ncutil::RemovePrefix(instrument->GetCode()));
    return v;
}

bool SimpleInstrumentOperation::CheckStandingOut(double desiredPrice, double desiredSize,
                                                 TWAPSlice *pCurrentSlice, nc::OrderSide side,
                                                 nc::Instrument *instrument) {

    std::cout << " SimpleInstrumentOperation:: CheckStandingOut " << instrument->GetCode() << std::endl;

    Clock::time_point timenow = std::chrono::system_clock::now();
    nc::PriceMode pm;
    double limit{0.0};
    bool rv = pCurrentSlice->GetCurrentPriceMode(timenow, pm, limit);

    if (rv) {
        if (pm == nc::PriceMode::PEG) {
            if (side == nc::OrderSide::buy) {
                //check buy
                if (instrument->GetBBO().bid.price == desiredPrice and instrument->GetBBO().bid.number_of_broker == 1) {
                    std::cout << instrument->GetCode() << " bid side only me" << std::endl;
                    return true;
                }
            } else {
                if (instrument->GetBBO().ask.price == desiredPrice and instrument->GetBBO().ask.number_of_broker == 1) {
                    std::cout << instrument->GetCode() << " ask side only me" << std::endl;
                    return true;
                }
            }
        }
    }

    return false;
}

bool SimpleInstrumentOperation::CheckDifferentFromDesired(double desiredPrice, double desiredSize,
                                                          nc::OrderSide side, nc::Instrument *instrument) {

    std::cout << " SimpleInstrumentOperation:: CheckDifferentFromDesired " << instrument->GetCode() << std::endl;

    if (side == nc::OrderSide::buy) {
        //check buy
        if (!compare_double(instrument->GetBBO().bid.price, desiredPrice)) {
            std::cout << " bid side price different" << std::endl;
            return true;
        }
    } else {
        if (!compare_double(instrument->GetBBO().ask.price, desiredPrice)) {
            std::cout << " ask side price different" << std::endl;
            return true;
        }
    }

    return false;
}


void WarrantInstrumentOperation::UpdateMarketDepth(const std::string &inst, MarketDataPriceDepth &mdpd) {
    if (inst == ncutil::RemovePrefix(instrument->GetCode())) {
        instrument->UpdateMarketDataPriceDepth(mdpd);
        instrument->UpdateMarketDataPriceDepthWithoutMe(mdpd);
    } else if (inst == ncutil::RemovePrefix(warrant->GetCode())) {
        instrument->UpdateMarketDataPriceDepth(mdpd);
    }
}


void WarrantInstrumentOperation::UpdateBBO(const std::string &inst, double bid, int64_t bvol, int64_t bnum, double ask,
                                           int64_t avol, int64_t anum) {
    if (inst == ncutil::RemovePrefix(instrument->GetCode())) {
        instrument->UpdateBBO(bid, bvol, bnum, ask, avol, anum);
        instrument->SetMarketDataReady();
    } else if (inst == ncutil::RemovePrefix(warrant->GetCode())) {
        warrant->UpdateBBO(bid, bvol, bnum, ask, avol, anum);
        warrant->SetMarketDataReady();
    }
}


std::vector<std::string> WarrantInstrumentOperation::GetInstrumentsForMarketData() {
    std::vector<std::string> v;
    v.push_back(ncutil::RemovePrefix(instrument->GetCode()));
    v.push_back(ncutil::RemovePrefix(warrant->GetCode()));
    return v;
}

bool WarrantInstrumentOperation::CheckStandingOut(double desiredPrice, double desiredSize,
                                                  TWAPSlice *pCurrentSlice, nc::OrderSide side,
                                                  nc::Instrument *m_instrument) {

    return false;
}

bool WarrantInstrumentOperation::CheckDifferentFromDesired(double desiredPrice, double desiredSize,
                                                           nc::OrderSide side, nc::Instrument *instrument) {
    return false;
}

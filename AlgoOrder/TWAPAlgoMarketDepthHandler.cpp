
#include "TWAPAlgoMarketDepthHandler.h"
#include "TWAPAlgo.h"

void TWAPAlgoMarketDepthHandler::OnPush_MarketData(const std::string &instrument, MarketDataPriceDepth &mdpd) {
    m_algo->GetInstrumentOperation()->UpdateMarketDepth(instrument, mdpd);
}


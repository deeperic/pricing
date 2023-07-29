
#include "TWAPAlgoMarketDataHandler.h"
#include "TWAPAlgo.h"

void TWAPAlgoMarketDataHandler::OnApiQuoteConnect(bool success, Futu::i64_t nErrCode, const char *strDesc) {
    std::cout << "on api connect: " << success << " " << nErrCode << " " << strDesc << std::endl;

    std::cout << m_algo->GetID() << " " << "TWAPAlgo " << __func__ << endl;

    if (success) {
        m_algo->SubscribeMarketDataForInstruments();
    }
}


void
TWAPAlgoMarketDataHandler::OnPush_MarketData(const std::string &instrument, double bid, int64_t bvol, int64_t bnum,
                                             double ask, int64_t avol,
                                             int64_t anum) {
    cout << m_algo->GetID() << " " << "TWAPAlgoMarketDataHandler " << __func__
         << " " << instrument << " " << " bid: " << bid << "/" << bvol << "/" << bnum << " ask: " << ask << "/" << avol
         << "/" << anum << std::endl;

    try {
        m_algo->GetInstrumentOperation()->UpdateBBO(instrument, bid, bvol, bnum, ask, avol, anum);

        //check if need to cancel intention now
        Clock::time_point timenow = std::chrono::system_clock::now();
        TWAPSlice *pCurrentSlice = m_algo->GetCurrentSlice(timenow);

        bool standingOut{false};
        if (m_algo->CheckforCancelIntention(pCurrentSlice->GetCurrentPriceMode(timenow), standingOut)) {
            std::cout << m_algo->GetID() << " " << "CheckforCancelIntention true, cancelling..." << std::endl;
            m_algo->GetIntention().CancelIntention();

            if (standingOut) {
                m_algo->SetupSuspend();
            }
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}


#ifndef PRICING_TWAPALGO_H
#define PRICING_TWAPALGO_H

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
#include "AlgoSpec.h"
#include "InstrumentOperation.h"

#include "TWAPAlgoAccountEventHandler.h"
#include "TWAPAlgoMarketDataHandler.h"
#include "TWAPAlgoOrderEventHandler.h"
#include "TWAPAlgoMarketDepthHandler.h"

//input lot size, target, duration in mins

constexpr int SEC_PER_MIN = 60;

struct FutuTWAP {
    //futu api
    std::shared_ptr<TWAPAlgoAccountEventHandler> m_accountHandler{nullptr};
    std::shared_ptr<TWAPAlgoMarketDataHandler> m_marketDataHandler{nullptr};
    std::shared_ptr<TWAPAlgoMarketDepthHandler> m_marketDepthHandler{nullptr};
    std::shared_ptr<TWAPAlgoOrderEventHandler> m_orderEventHandler{nullptr};
    std::shared_ptr<FutuAPIWrapper> m_fufuApi{nullptr};
    std::unordered_set<uint64_t> orderidSet;
};

struct TWAPInfo {
    nc::OrderSide m_side;
    int m_minOrderSize{0};
    int m_target{0};
    int m_done{0};
    int m_remaining{0};
    int m_durationSeconds;
    Clock::time_point m_startTime = std::chrono::system_clock::now();
    Clock::time_point m_endTime = std::chrono::system_clock::now();
};

class TWAPAlgo {
public:
    TWAPAlgo(std::size_t id, std::unique_ptr<SpreadTable> st, const AlgoSpec *spec, const AggressivityParamSet &ap,
             std::unique_ptr<InstrumentOperation> marketDataUpdater);

    virtual ~TWAPAlgo();

    auto &GetID() const noexcept { return m_id; }

    void ComputeTarget() noexcept;

    void ComputeUpdatedTarget() noexcept;

    void AddTWAPSlice(const TWAPSlice &t) noexcept;

    void AddDone(int d) noexcept { m_twapinfo.m_done += d; }

    bool AllDone() const noexcept { return m_twapinfo.m_done == m_twapinfo.m_target; }

    void UpdateFilled(int size) noexcept;

    bool IsRunning() const noexcept { return m_isRunning; }

    void Stop() noexcept { m_isRunning = false; }

    void Initialize(bool dryrun) noexcept;

    void OnTimer() noexcept;

    void SetupCurrent() noexcept;

    auto &GetBBO() noexcept { return m_instrument->GetBBO(); }

    std::optional<double> GetTargetPrice(const nc::PriceMode &pm, double limit) const noexcept;

    TWAPSlice *GetCurrentSlice(const Clock::time_point &tp) noexcept;

    int GetUndonePreviously(const Clock::time_point &tp) const noexcept;

    virtual bool CheckStandingOut(double desiredPrice, double desiredSize) const noexcept;

    virtual bool CheckDifferentFromDesired(double desiredPrice, double desiredSize) const noexcept;

    bool CheckforCancelIntention(nc::PriceMode pm, bool& standingOut) const noexcept;

    auto &GetInstrumentOperation() noexcept { return m_instrumentOperation; }

    auto &GetIntention() noexcept { return m_intention; }

    auto &GetInstrumentCode()const noexcept { return m_instrument->GetCode(); }

    auto &GetOrderIDSet() noexcept { return m_futu.orderidSet; }

    auto &GetInstrument() noexcept { return m_instrument; }

    void InitQuote() const noexcept;

    void SubscribeMarketDataForInstruments() noexcept;

    virtual void InitializeTargetPrice() noexcept ;

    void TimerThreadEnded() const noexcept;

    void SetDoneCallBack(std::function<void(const std::size_t &)> f);

    void SetupSuspend(){m_suspendSeconds = 5;}

protected:
    std::size_t m_id{0};

    FutuTWAP m_futu;

    //algo related
    AggressivityParamSet m_aggressivityParamSet;
    std::vector<TWAPSlice> m_vectSlice;
    TWAPSlice *m_pCurrentSlice; //point to current slice

    nc::Instrument *m_instrument{nullptr};

    TWAPInfo m_twapinfo;

    std::jthread m_timerThread;
    bool m_isRunning{true}; //false to exit threads/algo

    std::unique_ptr<TargetPrice> m_targetPrice{nullptr};

    Intention m_intention;
    FutuOrder m_futuOrder;

    std::unique_ptr<SpreadTable> m_spreadTable{nullptr};

    std::unique_ptr<InstrumentOperation> m_instrumentOperation{nullptr};

    int m_suspendSeconds{0};

    //callback on algo status
    std::function<void(const std::size_t &)> doneCallBack{nullptr};

};


#endif //PRICING_TWAPALGO_H

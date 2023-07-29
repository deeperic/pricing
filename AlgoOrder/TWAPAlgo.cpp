#include <iostream>
#include <iomanip>

#include "../Api/FutuAPIDefine.h"
#include "TWAPAlgo.h"
#include "TWAPTimer.h"

TWAPAlgo::TWAPAlgo(std::size_t id, std::unique_ptr<SpreadTable> st, const AlgoSpec *spec,
                   const AggressivityParamSet &ap, std::unique_ptr<InstrumentOperation> marketDataUpdater)
        : m_id(id), m_spreadTable(std::move(st)), m_aggressivityParamSet(ap),
          m_instrumentOperation(std::move(marketDataUpdater)) {
    auto ts = dynamic_cast<const TWAPSpec *>(spec);

    m_instrument = ts->instrument;

    m_twapinfo.m_side = ts->side;
    m_twapinfo.m_minOrderSize = ts->min_order_size;
    m_twapinfo.m_target = (ts->target / ts->min_order_size) * ts->min_order_size;
    m_twapinfo.m_durationSeconds = (int) ts->minToGo * SEC_PER_MIN;
    m_futu.orderidSet.reserve(1000);

    std::cout << m_id << " " << " TWAPAlgo ctor: " << this
              << " " << m_instrument->GetCode() << " " << m_instrument
              << " twap target: " << m_twapinfo.m_target << std::endl;

    m_futu.m_accountHandler = std::make_shared<TWAPAlgoAccountEventHandler>(this);
    m_futu.m_marketDataHandler = std::make_shared<TWAPAlgoMarketDataHandler>(this);
    m_futu.m_marketDepthHandler = std::make_shared<TWAPAlgoMarketDepthHandler>(this);
    m_futu.m_orderEventHandler = std::make_shared<TWAPAlgoOrderEventHandler>(this);
    m_futu.m_fufuApi = std::make_shared<FutuAPIWrapper>("127.0.0.1", 11111, m_futu.m_marketDataHandler.get());
    m_futu.m_fufuApi->SetAccountEventHandler(m_futu.m_accountHandler.get());
    m_futu.m_fufuApi->SetOrderEventHandler(m_futu.m_orderEventHandler.get());
    m_futu.m_fufuApi->SetMarketDepthEventHandler(m_futu.m_marketDepthHandler.get());

}

TWAPAlgo::~TWAPAlgo() {
    std::cout << m_id << " " << "twap algo dtor: " << this << std::endl;

    if (m_timerThread.joinable()) {
        m_timerThread.join();
    }

    std::cout << m_id << " " << "twap algo end" << std::endl;
}

void TWAPAlgo::InitializeTargetPrice() noexcept {
    std::cout << m_id << " " << " TWAPAlgo::InitializeTargetPrice " << std::endl;
    m_targetPrice = CreateTargetPriceFactory::CreateTargetPrice(m_spreadTable.get(), m_twapinfo.m_side);
}


void TWAPAlgo::ComputeTarget() noexcept {
    m_vectSlice.clear();

    m_twapinfo.m_remaining = m_twapinfo.m_target - m_twapinfo.m_done;
    int noOfSliceRequired = m_twapinfo.m_remaining / m_twapinfo.m_minOrderSize;
    if (!noOfSliceRequired) {
        std::cout << m_id << " " << "nothing to do" << std::endl;
        return;
    }
    int noOfSecondOfEachSlice = m_twapinfo.m_durationSeconds / noOfSliceRequired;
    int targetEachSlice = m_twapinfo.m_remaining / noOfSliceRequired;


    std::cout
            << m_id << " " << " No. of slice needed: " << noOfSliceRequired << std::endl
            << m_id << " " << " Each slice takes: " << noOfSecondOfEachSlice << " seconds" << std::endl;


    Clock::time_point now = std::chrono::system_clock::now();
    m_twapinfo.m_startTime = now; //algo start time

    for (int i = 0; i < noOfSliceRequired; ++i) {
        int sliceStart = i * noOfSecondOfEachSlice;
        int sliceEnd = ((i + 1) * noOfSecondOfEachSlice); //end 1s earlier
        Clock::time_point clockStart = now + std::chrono::seconds(sliceStart);
        Clock::time_point clockEnd = now + std::chrono::seconds(sliceEnd);

        m_twapinfo.m_endTime = clockEnd;//algo end time

        const std::time_t start = std::chrono::system_clock::to_time_t(clockStart);
        const std::time_t end = std::chrono::system_clock::to_time_t(clockEnd);


        std::cout
                << m_id << " " << " slice#: " << i << "/"
                << " size: " << targetEachSlice << "/"
                << " start sec: " << sliceStart << "/"
                << " end sec: " << sliceEnd << "/"
                << " start time: " << std::put_time(std::localtime(&start), "%F %T") << "/"
                << " end time: " << std::put_time(std::localtime(&end), "%F %T") << std::endl;


        TWAPSlice slice{i, this, targetEachSlice, sliceStart, sliceEnd, clockStart, clockEnd};
        slice.ComputeAggressivity(m_aggressivityParamSet);

        AddTWAPSlice(slice);
    }

    SetupCurrent();


}

void TWAPAlgo::Initialize(bool dryrun) noexcept {

    if (dryrun) {
        std::cout << m_id << " " << " ************************ TWAP DRY RUN ********************** " << std::endl;
    }

    InitializeTargetPrice();

    m_futuOrder.InitAPI(m_futu.m_fufuApi);
    m_intention.Init(m_id, ncutil::RemovePrefix(m_instrument->GetCode()), m_instrument->GetCode(), m_twapinfo.m_side,
                     &m_futuOrder, dryrun);

    //compute
    ComputeTarget();

    //init trading interface and market data
    m_futu.m_fufuApi->ConnectFutu("127.0.0.1", 11111, FUTU_TRADING_PW, FUTU_SEC_FIRM);

    m_timerThread = std::jthread{TWAPTimer, this};

}


void TWAPAlgo::SubscribeMarketDataForInstruments() noexcept {
    std::vector<std::string> v = m_instrumentOperation->GetInstrumentsForMarketData();
    m_futu.m_fufuApi->SubscribeMarketDataForInstruments(v);
}

bool TWAPAlgo::CheckStandingOut(double desiredPrice, double desiredSize) const noexcept {
    std::cout << "TWAPAlgo::CheckStandingOut" << std::endl;

    if (m_instrumentOperation->CheckStandingOut(desiredPrice, desiredSize, m_pCurrentSlice, m_twapinfo.m_side,
                                                m_instrument)) {
        return true;
    }

    return false;
}

bool TWAPAlgo::CheckDifferentFromDesired(double desiredPrice, double desiredSize) const noexcept {
    std::cout << "TWAPAlgo::CheckDifferentFromDesired" << std::endl;

    if (m_instrumentOperation->CheckDifferentFromDesired(desiredPrice, desiredSize, m_twapinfo.m_side, m_instrument)) {
        std::cout << m_id << " price different" << std::endl;
        return true;
    }

    return false;
}

bool TWAPAlgo::CheckforCancelIntention(nc::PriceMode pm, bool &standingOut) const noexcept {
    //if need to cancel intention in any case due to market condition, return true
    const auto order = m_intention.GetOrder();
    if (order == nullptr) {
        return false;
    }
    if (order->orderID > 0) {
        double current_price = order->price;
        double current_size = order->size;

        std::cout << m_id << " order current price: " << current_price << " current size: " << current_size
                  << std::endl;

        if (CheckStandingOut(current_price, current_size)) {
            standingOut = true;
            return true;
        }

        if (pm == nc::PriceMode::PEG and CheckDifferentFromDesired(current_price, current_size)) {
            std::cout << "CheckforCancelIntention desired different from current. cancelling..." << std::endl;
            return true;
        }

        if (!m_isRunning) {
            return true;
        }
    }
    return false;
}

void TWAPAlgo::UpdateFilled(int size) noexcept {
    Clock::time_point timenow = std::chrono::system_clock::now();
    std::cout << m_id << " " << " time now: " << ncutil::GetTimeFromSteadyClock(timenow) << " done: " << size
              << std::endl;


    if (TWAPSlice *pCurrentSlice = GetCurrentSlice(timenow); pCurrentSlice) {
        std::cout << m_id << " " << "slice " << pCurrentSlice->GetIndex() << " old done: " << pCurrentSlice->GetDone()
                  << std::endl;

        pCurrentSlice->AddDone(size);
        std::cout << m_id << " " << "new done: " << pCurrentSlice->GetDone() << std::endl;
    } else {
        std::cout << m_id << " " << "can't get slice to update done" << std::endl;
    }

    //update main size
    AddDone(size);
    std::cout << m_id << " " << "twap total done size: " << m_twapinfo.m_done << std::endl;
    if (AllDone()) {
        std::cout << m_id << " " << "twap all done, stopping" << std::endl;
        Stop();
    }

}

void TWAPAlgo::ComputeUpdatedTarget() noexcept {
    Clock::time_point timenow = std::chrono::system_clock::now();
    TWAPSlice *pCurrentSlice = GetCurrentSlice(timenow);
    std::cout << m_id << " " << " ComputeUpdatedTarget time now: " << ncutil::GetTimeFromSteadyClock(timenow)
              << std::endl;

    if (timenow > m_twapinfo.m_endTime) { //no need run anymore
        std::cout << m_id << " " << "time exceeds end time, stopping..." << std::endl;
        Stop();
        return;
    }

    if (!m_isRunning) {
        std::cout << m_id << " " << "it's not running, no process... " << std::endl;
        return;
    }

    int sumRemaining = GetUndonePreviously(timenow);

    if (pCurrentSlice) {
        std::cout << m_id << " " << " current slice index#: " << pCurrentSlice->GetIndex() << std::endl;
        nc::PriceMode pm;
        double limit{0.0};
        bool rv = pCurrentSlice->GetCurrentPriceMode(timenow, pm, limit);
        auto target_price = GetTargetPrice(pm, limit);
        if (target_price.has_value()) {
            std::cout << m_id << "target_price: " << target_price.value() << std::endl;
            int target_size = pCurrentSlice->GetRemaining() + sumRemaining; //current planned size + previous undone
            if (target_size < 0) {
                std::cout << m_id << " " << " critical error for slice#: " << pCurrentSlice->GetIndex()
                          << " target size: " << target_size << std::endl;
                exit(1);
            }
            double rounded = m_spreadTable->RoundPrice(target_price.value(), m_twapinfo.m_side);
            std::cout << m_id << " " << "slice price/size: " << pm << "/" << rounded << "/" << target_size << std::endl;

            //added price for PriceDepth without me
            m_instrument->GetPriceDepthWithoutMe().AddOrder(m_twapinfo.m_side, target_size, rounded);

            if (!m_suspendSeconds) {
                m_intention.UpdateIntention(target_size, rounded);
            } else {
                --m_suspendSeconds;
            }
        } else {
            //NONE mode goes here?
            std::cout << m_id << " " << "slice cancelling. target_price has no value." << std::endl;
            m_instrument->GetPriceDepthWithoutMe().RemoveOrder();
            m_intention.CancelIntention();
        }

    } else {
        std::cout << m_id << " " << " can't get current slice " << std::endl;
    }
}

void TWAPAlgo::OnTimer() noexcept{
    cout << m_id << " " << "TWAPAlgo " << __func__ << std::endl;

    if (!m_instrument->GetMarketDataReady()) {
        auto p = m_instrument;
        cout << m_id << " " << "TWAPAlgo not yet ready " << m_instrument->GetCode() << " " << p << " " << __func__
             << std::endl;
        return;
    }

    if (!m_isRunning) {
        cout << m_id << " " << "TWAPAlgo not running " << __func__ << std::endl;
        return;
    }

    ComputeUpdatedTarget();
}

void TWAPAlgo::SetupCurrent() noexcept{
    if (m_vectSlice.empty()) {
        TWAPSlice &slice = m_vectSlice[0];
        m_pCurrentSlice = &slice;
    }
}

void TWAPAlgo::AddTWAPSlice(const TWAPSlice &t) noexcept{
    m_vectSlice.emplace_back(t);
}


std::optional<double> TWAPAlgo::GetTargetPrice(const nc::PriceMode &pm, double limit) const noexcept{
    auto d = m_targetPrice->GetPrice(pm, m_instrument->GetBBO(), m_instrument->GetPriceDepthWithoutMe(), limit);
    return d;
}

TWAPSlice *TWAPAlgo::GetCurrentSlice(const Clock::time_point &tp) noexcept{
    TWAPSlice *p{nullptr};
    for (int i = 0; i < m_vectSlice.size(); i++) {
        auto &slice = m_vectSlice[i];//start from the beginning
        if (slice.IsWithin(tp)) {
            p = &slice;
            std::cout << m_id << " " << "Current Slice# " << i << std::endl;
            break;
        }
    }
    return p;
}

int TWAPAlgo::GetUndonePreviously(const Clock::time_point &tp) const noexcept{
    int sum = 0;

    for (int i = 0; i < m_vectSlice.size(); i++) {
        const auto &slice = m_vectSlice[i];//start from the beginning
        if (slice.GetEndTime() < tp) {
            sum += slice.GetRemaining();
        }
    }

    std::cout << m_id << " " << "undone previously: " << sum << std::endl;

    return sum;
}

void TWAPAlgo::InitQuote() const noexcept{
    m_futu.m_fufuApi->InitQot("127.0.0.1", 11111);
}

void TWAPAlgo::TimerThreadEnded() const noexcept {
    std::cout << "TimerThreadEnded start: " << m_id << std::endl;

    if (doneCallBack) {
        doneCallBack(m_id);
    }

    std::cout << "TimerThreadEnded end" << std::endl;
}

void TWAPAlgo::SetDoneCallBack(std::function<void(const std::size_t &)> f) {
    std::cout << "SetDoneCallBack called: " << std::endl;
    doneCallBack = f;
}


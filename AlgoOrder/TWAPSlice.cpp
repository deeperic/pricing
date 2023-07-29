#include "TWAPSlice.h"
#include "TWAPAlgo.h"
#include "../Util/NCUtil.h"

void TWAPSlice::ComputeAggressivity(const AggressivityParamSet &param) noexcept {
    Aggressivity avty;

    int duration = (m_endSec - m_startSec) + 1;
    int startSec = m_startSec;
    int endSec = m_endSec;
    Clock::time_point clockStart = m_startTime;

    int i = 0;
    for (auto &p: param.m_vectParam) {
        avty.limitPrice = p.limitPrice;
        avty.pm = p.pm;

        int sec = duration * (int) p.pctTime;

        endSec = startSec + sec;
        Clock::time_point aggressivityStart = clockStart;
        Clock::time_point aggressivityEnd = aggressivityStart + std::chrono::seconds(sec); //end 1s earlier

        avty.startSec = startSec;
        avty.endSec = (endSec - 1);
        avty.startTime = aggressivityStart;
        avty.endTime = aggressivityEnd;

        std::cout
                << m_algo->GetID() << " " << " \t aggressivity#: " << avty.pm << "/"
                << " start sec: " << startSec << "/"
                << " end sec: " << endSec << "/"
                << " start time: " << ncutil::GetTimeFromSteadyClock(avty.startTime) << "/"
                << " end time: " << ncutil::GetTimeFromSteadyClock(avty.endTime) << std::endl;

        m_vectAggressivity.emplace_back(avty);

        clockStart = aggressivityEnd;
        startSec = endSec;
        ++i;

    }


}

bool TWAPSlice::GetCurrentPriceMode(const Clock::time_point &timenow, nc::PriceMode &pm, double &limit) const noexcept {
    //get current price mode
    nc::PriceMode result{nc::PriceMode::NONE};
    for (const auto &a: m_vectAggressivity) {
        if (IsWithin(timenow, a.startTime, a.endTime)) {
            std::cout << m_algo->GetID() << " " << " got current pm: " << a.pm << std::endl;
            if (a.pm == nc::PriceMode::LIMIT) {
                std::cout << m_algo->GetID() << " " << " got current limit: " << a.limitPrice << std::endl;
            }
            pm = a.pm;
            limit = a.limitPrice;
            return true;
        }
    }
    std::cout << m_algo->GetID() << " " << " can't get current pm " << std::endl;
    pm = result;
    return false;
}

nc::PriceMode TWAPSlice::GetCurrentPriceMode(Clock::time_point &timenow) const noexcept{

    //get current price mode
    nc::PriceMode pm{nc::PriceMode::NONE};
    for (const auto &a: m_vectAggressivity) {
        if (IsWithin(timenow, a.startTime, a.endTime)) {
            std::cout << m_algo->GetID() << " " << " got current pm: " << a.pm << std::endl;
            return a.pm;
        }
    }
    std::cout << m_algo->GetID() << " " << " INVALID pm: " << pm << std::endl;
    return pm;
}

bool TWAPSlice::IsWithin(const Clock::time_point &tp) const noexcept {
    return IsWithin(tp, m_startTime, m_endTime);
}

bool
TWAPSlice::IsWithin(const Clock::time_point &tp, const Clock::time_point &start, const Clock::time_point &end) const noexcept{
    if (tp >= start and tp < end) {
        return true;
    }
    return false;
}

void TWAPSlice::AddDone(int done) noexcept{
    m_done += done;
    std::cout << m_algo->GetID() << " " << "slice# " << m_index << " add done: " << done << " total done: " << m_done
              << std::endl;
}

void TWAPSlice::SetDone(int done) noexcept{
    m_done = done;
    std::cout << m_algo->GetID() << " " << "slice# " << m_index << " set done: " << done << std::endl;
}

int TWAPSlice::GetRemaining() const noexcept {
    auto r = m_target - m_done;
    std::cout << m_algo->GetID() << " " << "slice# " << m_index
              << " target: " << m_target
              << " done: " << m_done
              << " remaining: " << r << std::endl;
    return r;
}

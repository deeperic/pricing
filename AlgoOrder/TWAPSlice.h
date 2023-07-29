
#ifndef PRICING_TWAPSLICE_H
#define PRICING_TWAPSLICE_H


#include "Aggressivity.h"

class TWAPAlgo;

class TWAPSlice {
public:
    TWAPSlice(int i, TWAPAlgo *algo, int target, int start, int end, const Clock::time_point &st,
              const Clock::time_point &et)
            : m_index(i), m_algo(algo), m_target(target), m_startSec(start), m_endSec(end), m_startTime(st),
              m_endTime(et) {
        m_vectAggressivity.reserve(20);
    }

    ~TWAPSlice() = default;

    auto &GetIndex() { return m_index; }

    void ComputeAggressivity(const AggressivityParamSet &param) noexcept;

    nc::PriceMode GetCurrentPriceMode(Clock::time_point &timenow) const noexcept;

    bool GetCurrentPriceMode(const Clock::time_point &timenow, nc::PriceMode &pm, double &limit) const noexcept;

    auto GetDone() const noexcept { return m_done; }

    void AddDone(int done) noexcept;

    void SetDone(int done) noexcept;

    int GetRemaining() const noexcept;

    void SetPreviousRemaining(int i) noexcept {
        m_previousRemaining = i;
    }

    auto &GetStartTime() const noexcept { return m_startTime; }

    auto &GetEndTime() const noexcept { return m_endTime; }

    bool IsWithin(const Clock::time_point &tp) const noexcept;

    bool
    IsWithin(const Clock::time_point &tp, const Clock::time_point &start, const Clock::time_point &end) const noexcept;

private:
    int m_index{0};
    TWAPAlgo *m_algo{nullptr};
    int m_previousRemaining{0};
    int m_target{0};
    int m_done{0};
    int m_startSec{0};
    int m_endSec{0};
    Clock::time_point m_startTime{std::chrono::system_clock::now()};
    Clock::time_point m_endTime{std::chrono::system_clock::now()};

    std::vector<Aggressivity> m_vectAggressivity;
};


#endif //PRICING_TWAPSLICE_H

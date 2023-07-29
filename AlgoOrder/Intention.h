#ifndef PRICING_INTENTION_H
#define PRICING_INTENTION_H

#include <string>
#include <iostream>

#include "../Basic/BasicStruct.h"
#include "../IntentionStateMachine/IntentionFSM.h"
#include "../Api/FutuOrder.h"

//Intention is the price/size i wanted to have now
//1 intention => N orders

// IntentionStateMachine
// Futu Interface
// Futu callback

class Intention {
public:
    Intention() = default;

    Intention(int id, std::string_view futu_instrument, std::string_view system_instrument, nc::OrderSide side,
              int desired_size, double desired_price);

    ~Intention() = default;

    void Init(int id, std::string_view futu_instrument, std::string_view system_instrument, nc::OrderSide side,
              FutuOrder *api, bool dryrun) noexcept;

    void UpdateIntention(int desired_size, double desired_price) noexcept;

    void UpdateIntentionPrice(double desired_price) noexcept;

    void CancelIntention() noexcept;

    void GenerateAction() noexcept;

    auto &GetStateContext() noexcept { return m_stateContext; }

    auto &GetDesired() const noexcept { return m_desired; }

    auto &GetCurrent() const noexcept { return m_current; }

    auto GetOrder() const noexcept { return m_stateContext.GetOrder(); }

private:
    int m_id{0};
    IntentionStateContext m_stateContext;
    FutuOrder *m_futuOrder{nullptr};

    nc::IntentionState m_state{nc::IntentionState::NOT_PENDING};
    std::string m_futu_instrument;
    std::string m_system_instrument;
    nc::OrderSide m_side;
    nc::SizePrice m_desired;
    nc::SizePrice m_current;

    bool m_dryRun{true};


};


#endif //PRICING_INTENTION_H

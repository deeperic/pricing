
#include "Intention.h"

Intention::Intention(int id, std::string_view futu_instrument, std::string_view system_instrument, nc::OrderSide side,
                     int desired_size, double desired_price)
        : m_id(id), m_futu_instrument(futu_instrument), m_system_instrument(system_instrument), m_side(side),
          m_desired(desired_size, desired_price) {}

void Intention::Init(int id, std::string_view futu_instrument, std::string_view system_instrument, nc::OrderSide side,
                     FutuOrder *api, bool dryrun) noexcept {
    m_id = id;
    m_futu_instrument = futu_instrument;
    m_system_instrument = system_instrument;
    m_side = side;
    m_futuOrder = api;
    m_dryRun = dryrun;

    m_stateContext.Init(m_futuOrder);
}

void Intention::CancelIntention() noexcept {
    if (!m_dryRun) {
        m_stateContext.SendCancelOrder();
    }

}

void Intention::UpdateIntentionPrice(double desired_price) noexcept {
    m_desired.price = desired_price;
    GenerateAction();
}

void Intention::UpdateIntention(int desired_size, double desired_price) noexcept {
    m_state = nc::IntentionState::NEW;
    m_desired.size = desired_size;
    m_desired.price = desired_price;
    GenerateAction();
}

void Intention::GenerateAction() noexcept {
    if (m_desired.size == 0) {
        return;
    }

    auto order = std::make_unique<nc::Order>(m_futu_instrument, m_system_instrument, m_side, m_desired.size,
                                             m_desired.price, nc::OrderType::limit);

    std::cout << m_id << " " << __FUNCTION__ << " " << m_futu_instrument << "/"
              << m_side << "/"
              << m_desired.price << "/"
              << m_desired.size << "/"
              << std::endl;

    if (!m_dryRun) {
        m_stateContext.SendNewOrder(std::move(order));
    }

}
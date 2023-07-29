
#include "VolOrderAlgo.h"

VolOrderAlgo::VolOrderAlgo(std::size_t id, std::unique_ptr<SpreadTable> st, const AlgoSpec *spec,
                           const AggressivityParamSet &ap, std::unique_ptr<InstrumentOperation> marketDataUpdater)
        : TWAPAlgo(id, std::move(st), spec, ap, std::move(marketDataUpdater)) {
    auto vos = dynamic_cast<const VolOrderSpec *>(spec);
    m_underlying = vos->underlying;
    m_pricingSide = vos->pricingSide;

    auto p = m_underlying;
    std::cout << GetID() << " VolOrderAlgo ctor: " << m_underlying->GetCode() << " " << p << std::endl;
}

void VolOrderAlgo::InitializeTargetPrice() noexcept {
    std::cout << " VolOrderAlgo::InitializeTargetPrice " << std::endl;
    m_targetPrice = CreateTargetPriceFactory::CreateVolOrderTargetPrice(m_spreadTable.get(), m_pricingSide);

    auto tpw = dynamic_cast<TargetPriceWarrant *>(m_targetPrice.get());
    auto warrant = dynamic_cast<nc::Warrant *>(m_instrument);
    tpw->SetWarrant(m_underlying, warrant);
}



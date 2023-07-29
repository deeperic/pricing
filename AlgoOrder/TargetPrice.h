
#ifndef PRICING_TARGETPRICE_H
#define PRICING_TARGETPRICE_H

#include <iostream>
#include <optional>

#include "../Basic/BasicStruct.h"
#include "../PricingModel/PricingLib.h"

class TargetPrice {
public:
    TargetPrice(SpreadTable *st, nc::OrderSide side);

    virtual ~TargetPrice();

    virtual std::optional<double>
    GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo, const nc::MarketDataPriceDepthWithoutMe &withoutMe,
             double limit) = 0;

    auto &GetSpreadTable() const { return m_st; }

private:
    nc::OrderSide m_side{nc::OrderSide::buy};
    SpreadTable *m_st{nullptr};
};

//for twap
class TargetPriceSell : public TargetPrice {
public:
    explicit TargetPriceSell(SpreadTable *st)
            : TargetPrice(st, nc::OrderSide::sell) {}

    virtual ~TargetPriceSell() = default;

    std::optional<double>
    GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo, const nc::MarketDataPriceDepthWithoutMe &withoutMe,
             double limit) override;
};

class TargetPriceBuy : public TargetPrice {
public:
    explicit TargetPriceBuy(SpreadTable *st)
            : TargetPrice(st, nc::OrderSide::buy) {}

    virtual ~TargetPriceBuy() = default;

    std::optional<double>
    GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo, const nc::MarketDataPriceDepthWithoutMe &withoutMe,
             double limit) override;
};

//for vol order

class TargetPriceWarrant : public TargetPrice {
public:
    using TargetPrice::TargetPrice;

    virtual ~TargetPriceWarrant() = default;

    void SetWarrant(const nc::Instrument *ul, const nc::Warrant *w);

    auto &GetWarrant() { return m_warrant; }

    auto &GetUnderlying() { return m_ul; }

    auto &GetPricing() { return m_pricing; }

private:
    const nc::Warrant *m_warrant{nullptr};
    const nc::Instrument *m_ul{nullptr};
    PricingLib m_pricing;
};

class TargetVolOrderPriceSell : public TargetPriceWarrant {
public:
    explicit TargetVolOrderPriceSell(SpreadTable *st)
            : TargetPriceWarrant(st, nc::OrderSide::sell) {}

    virtual ~TargetVolOrderPriceSell() = default;

    std::optional<double>
    GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo, const nc::MarketDataPriceDepthWithoutMe &withoutMe,
             double limit) override;

};

class TargetVolOrderPriceBuy : public TargetPriceWarrant {
public:
    TargetVolOrderPriceBuy(SpreadTable *st)
            : TargetPriceWarrant(st, nc::OrderSide::buy) {}

    virtual ~TargetVolOrderPriceBuy() = default;

    std::optional<double>
    GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo, const nc::MarketDataPriceDepthWithoutMe &withoutMe,
             double limit) override;

};

class CreateTargetPriceFactory {
public:
    static std::unique_ptr<TargetPrice> CreateTargetPrice(SpreadTable *st, nc::OrderSide side);

    static std::unique_ptr<TargetPrice> CreateVolOrderTargetPrice(SpreadTable *st, nc::PricingSide side);
};


#endif //PRICING_TARGETPRICE_H

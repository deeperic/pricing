#include "TargetPrice.h"

TargetPrice::TargetPrice(SpreadTable *st, nc::OrderSide side)
        : m_st(st), m_side(side) {
    m_side = side;
}

TargetPrice::~TargetPrice() {

}


std::optional<double> TargetPriceSell::GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo,
                                                const nc::MarketDataPriceDepthWithoutMe &withoutMe, double limit) {
    switch (pm) {
        case nc::PriceMode::NONE:
            return std::nullopt;
            break;
        case nc::PriceMode::LIMIT:
            return limit;
            break;
        case nc::PriceMode::PEG:
            return bbo.ask.price;
            break;
        case nc::PriceMode::MID: {
            double mid = (withoutMe.bids[0].price + withoutMe.asks[0].price) / 2;
            //align to tick size
            double tick = GetSpreadTable()->GetSpread(mid, nc::OrderSide::buy);
            double rounded = round_down(mid, tick);
            return rounded;

            break;
        }
        case nc::PriceMode::HIT:
            return bbo.bid.price;
            break;
        default:
            return bbo.ask.price;
            break;
    }
    return bbo.ask.price * 100;
}


std::optional<double> TargetPriceBuy::GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo,
                                               const nc::MarketDataPriceDepthWithoutMe &withoutMe, double limit) {
    switch (pm) {
        case nc::PriceMode::NONE:
            return std::nullopt;
            break;
        case nc::PriceMode::LIMIT:
            return limit;
            break;
        case nc::PriceMode::PEG:
            std::cout << " TargetPriceBuy::GetPriceAtIndex peg: " << bbo.bid.price << std::endl;
            return bbo.bid.price;
            break;
        case nc::PriceMode::MID: {
            double mid = (withoutMe.bids[0].price + withoutMe.asks[0].price) / 2;
            std::cout << " TargetPriceBuy::GetPriceAtIndex "
                      << " wo me bid: " << withoutMe.bids[0].price
                      << " wo me ask: " << withoutMe.asks[0].price
                      << " mid: " << mid
                      << std::endl;
            //align to tick size

            double tick = GetSpreadTable()->GetSpread(mid, nc::OrderSide::buy);
            double rounded = round_down(mid, tick);
            return rounded;

            break;
        }
        case nc::PriceMode::HIT:
            std::cout << " TargetPriceBuy::GetPriceAtIndex hit: " << bbo.ask.price << std::endl;
            return bbo.ask.price;
            break;
        default:
            std::cout << " TargetPriceBuy::GetPriceAtIndex default: " << bbo.bid.price << std::endl;
            return bbo.bid.price;
            break;
    }
    return bbo.bid.price * 100;
}


void TargetPriceWarrant::SetWarrant(const nc::Instrument *ul, const nc::Warrant *w) {
    m_ul = ul;
    m_warrant = w;
}


std::optional<double> TargetVolOrderPriceSell::GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo,
                                                        const nc::MarketDataPriceDepthWithoutMe &withoutMe,
                                                        double limit) {
    switch (pm) {
        case nc::PriceMode::NONE:
            return std::nullopt;

        case nc::PriceMode::LIMIT: {
            double r = 0.008;
            double vol = limit / 100.0; //volatility = 66 = 66% =  0.66

            nc::PricingSpec ps{GetWarrant()->GetCallPut(), GetUnderlying()->GetBBO().ask.price, GetWarrant()->GetK(),
                               GetWarrant()->GetExpiry(), vol, r, GetWarrant()->GetConversionRatio()};
            nc::Greek greek;
            double fair{};
            GetPricing().ComputeWarrantFair(ps, fair, greek);

            double tick = GetSpreadTable()->GetSpread(fair, nc::OrderSide::sell);
            double rounded = round_up(fair, tick);
            return rounded;
        }

        case nc::PriceMode::PEG:
            return bbo.ask.price;

        case nc::PriceMode::MID: {
            double mid = (bbo.ask.price + bbo.bid.price) / 2;
            //align to tick size
            return mid;

        }
        case nc::PriceMode::HIT:
            return bbo.bid.price;

        default:
            return bbo.ask.price;

    }

}


std::optional<double> TargetVolOrderPriceBuy::GetPrice(nc::PriceMode pm, const nc::MarketDataBBO &bbo,
                                                       const nc::MarketDataPriceDepthWithoutMe &withoutMe,
                                                       double limit) {
    switch (pm) {
        case nc::PriceMode::NONE:
            return std::nullopt;

        case nc::PriceMode::LIMIT: {
            double r = 0.008;
            double vol = limit / 100.0; //volatility = 66 = 66% =  0.66

            nc::PricingSpec ps{GetWarrant()->GetCallPut(), GetUnderlying()->GetBBO().bid.price, GetWarrant()->GetK(),
                               GetWarrant()->GetExpiry(), vol, r, GetWarrant()->GetConversionRatio()};
            nc::Greek greek;
            double fair{};
            GetPricing().ComputeWarrantFair(ps, fair, greek);

            double tick = GetSpreadTable()->GetSpread(fair, nc::OrderSide::buy);
            double rounded = round_down(fair, tick);
            return rounded;
        }

        case nc::PriceMode::PEG:
            std::cout << " TargetVolOrderPriceBuy::GetPriceAtIndex peg: " << bbo.bid.price << std::endl;
            return bbo.bid.price;

        case nc::PriceMode::MID: {
            double mid = (bbo.ask.price + bbo.bid.price) / 2;
            std::cout << " TargetVolOrderPriceBuy::GetPriceAtIndex mid: " << mid << std::endl;
            //align to tick size
            return mid;

        }
        case nc::PriceMode::HIT:
            std::cout << " TargetVolOrderPriceBuy::GetPriceAtIndex hit: " << bbo.ask.price << std::endl;
            return bbo.ask.price;

        default:
            std::cout << " TargetVolOrderPriceBuy::GetPriceAtIndex default: " << bbo.bid.price << std::endl;
            return bbo.bid.price;

    }

}


std::unique_ptr<TargetPrice> CreateTargetPriceFactory::CreateTargetPrice(SpreadTable *st, nc::OrderSide side) {
    switch (side) {
        case nc::OrderSide::buy:
            return std::make_unique<TargetPriceBuy>(st);
            break;
        case nc::OrderSide::sell:
            return std::make_unique<TargetPriceSell>(st);
            break;
    }
    return nullptr;
}

std::unique_ptr<TargetPrice>
CreateTargetPriceFactory::CreateVolOrderTargetPrice(SpreadTable *st, nc::PricingSide side) {
    switch (side) {
        case nc::PricingSide::BID:
            return std::make_unique<TargetVolOrderPriceBuy>(st);
            break;
        case nc::PricingSide::ASK:
            return std::make_unique<TargetVolOrderPriceSell>(st);
            break;
    }
    return nullptr;
}

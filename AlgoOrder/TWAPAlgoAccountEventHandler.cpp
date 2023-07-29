
#include "TWAPAlgoAccountEventHandler.h"

void
TWAPAlgoAccountEventHandler::OnAccountFund(const double &cash, const double &totalAssets, const double &marketValue) {
    std::cout << "cash: " << cash
              << " totalAssets: " << totalAssets
              << " marketValue: " << marketValue
              << std::endl;
}

void TWAPAlgoAccountEventHandler::OnPositions(const std::vector<nc::Position> &v) {
    for (auto &p: v) {
        double pnl = p.size * (p.nowPrice - p.costPrice);
        std::cout << "code: " << p.instrument
                  << " size: " << p.size
                  << " cost: " << p.costPrice
                  << " current price: " << p.nowPrice
                  << " pnl: " << pnl << std::endl;
    }
}



#ifndef PRICING_TWAPALGOORDEREVENTHANDLER_H
#define PRICING_TWAPALGOORDEREVENTHANDLER_H

#include "Api/FutuAPIWrapper.h"
#include "Api/FutuEventHandler.h"

class TWAPAlgo;

class TWAPAlgoOrderEventHandler : public FutuOrderEventHandler {
public:
    TWAPAlgoOrderEventHandler(TWAPAlgo *algo) :
            m_algo(algo) {};

    virtual ~TWAPAlgoOrderEventHandler() = default;

    void OnApiTradeConnect(bool success, Futu::i64_t nErrCode, const char *strDesc) override;

    void OnApiGetAccount(bool success, const std::string &msg) override;

    void OnUnlockTrade(bool success, const std::string &msg) override;

    void OnReply_PlaceOrder(Futu::u32_t serial, std::string_view instrument, bool success, const uint64_t &orderid,
                            int retType, int errorCode,
                            const std::string &errorMsg) override;

    void OnReply_ModifyOrder(Futu::u32_t serial, bool success, const uint64_t &orderid, int retType, int errorCode,
                             const std::string &errorMsg) override;

    void
    OnPush_UpdateOrder(const uint64_t &orderID, bool orderOpen, const std::string &instrument, double orderQty,
                       double orderPrice, double filledQty, double filledAvgPrice,
                       const std::string &updateTime, const std::string &remark) override;

    void
    OnPush_UpdateOrderFill(const std::string &instrument, const uint64_t &orderID, const uint64_t &nOrderFillID,
                           Trd_Common::TrdSide tradeSide, double filledQty, double filledPrice,
                           Trd_Common::OrderFillStatus fillStatus,
                           int counterBrokerID, const std::string &counterBrokerName) override;

private:
    TWAPAlgo *m_algo{nullptr};
};


#endif //PRICING_TWAPALGOORDEREVENTHANDLER_H

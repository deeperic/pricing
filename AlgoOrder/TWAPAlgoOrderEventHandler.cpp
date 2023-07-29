

#include "TWAPAlgoOrderEventHandler.h"
#include "TWAPAlgo.h"

void TWAPAlgoOrderEventHandler::OnApiTradeConnect(bool success, Futu::i64_t nErrCode, const char *strDesc) {}

void TWAPAlgoOrderEventHandler::OnApiGetAccount(bool success, const std::string &msg) {}

void TWAPAlgoOrderEventHandler::OnUnlockTrade(bool success, const std::string &msg) {
    cout << m_algo->GetID() << " " << "TWAPAlgo " << __func__ << endl;

    if (success) {
        m_algo->InitQuote();
    }
}

void TWAPAlgoOrderEventHandler::OnReply_PlaceOrder(Futu::u32_t serial, std::string_view instrument, bool success,
                                                   const uint64_t &orderid, int retType, int errorCode,
                                                   const std::string &errorMsg) {

    std::cout << m_algo->GetID() << " " << __FUNCTION__
              << m_algo->GetInstrumentCode()
              << " " << success
              << "/" << orderid
              << "/" << retType
              << "/" << errorCode
              << "/" << errorMsg
              << std::endl;

    if (success) {
        m_algo->GetOrderIDSet().insert(orderid);
        m_algo->GetIntention().GetStateContext().NewOrderOK(orderid);
    } else {
        m_algo->GetIntention().GetStateContext().NewOrderFail();
    }
}

void
TWAPAlgoOrderEventHandler::OnReply_ModifyOrder(Futu::u32_t serial, bool success, const uint64_t &orderid, int retType,
                                               int errorCode,
                                               const std::string &errorMsg) {

    std::cout << m_algo->GetID() << " " << __FUNCTION__
              << "/" << m_algo->GetInstrumentCode()
              << "/" << success
              << "/" << orderid
              << "/" << retType
              << "/" << errorCode
              << "/" << errorMsg
              << std::endl;

    if (!m_algo->GetOrderIDSet().contains(orderid)) {
        //order id not found
        return;
    }

    if (success)
        m_algo->GetIntention().GetStateContext().CancelOrderOK(orderid);
    else
        m_algo->GetIntention().GetStateContext().CancelOrderFail(orderid);
}

void
TWAPAlgoOrderEventHandler::OnPush_UpdateOrder(const uint64_t &orderID, bool orderOpen, const std::string &instrument,
                                              double orderQty,
                                              double orderPrice, double filledQty, double filledAvgPrice,
                                              const std::string &updateTime, const std::string &remark) {

    std::cout << m_algo->GetID() << " " << __FUNCTION__
              << " " << orderID
              << "/" << orderOpen
              << "/" << instrument
              << "/" << orderQty
              << "/" << orderPrice
              << "/" << filledQty
              << "/" << filledAvgPrice
              << std::endl;

    if (!m_algo->GetOrderIDSet().contains(orderID))
        return;

    if (!orderOpen) {
        //close the order
        std::cout << "TWAPAlgo::OnPush_UpdateOrder: set order closed " << orderID << std::endl;
        m_algo->GetIntention().GetStateContext().CloseOrder(orderID);
        m_algo->GetInstrument()->GetPriceDepthWithoutMe().RemoveOrder();
    }
}

void
TWAPAlgoOrderEventHandler::OnPush_UpdateOrderFill(const std::string &instrument, const uint64_t &orderID,
                                                  const uint64_t &nOrderFillID,
                                                  Trd_Common::TrdSide tradeSide, double filledQty, double filledPrice,
                                                  Trd_Common::OrderFillStatus fillStatus,
                                                  int counterBrokerID, const std::string &counterBrokerName) {
    if (!m_algo->GetOrderIDSet().contains(orderID))
        return;

    std::cout << m_algo->GetID() << " " << __FUNCTION__
              << instrument << "/"
              << orderID << "/"
              << nOrderFillID << "/"
              << tradeSide << "/"
              << filledQty << "/"
              << filledPrice << "/"
              << fillStatus << "/"
              << counterBrokerID << "/"
              << counterBrokerName << std::endl;

    m_algo->GetIntention().GetStateContext().UpdateOrderFilled(filledQty, filledPrice);

    m_algo->UpdateFilled(filledQty);
}
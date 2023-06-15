#include "order.h"
#include "server.h" // LatinexSessionServer

namespace latinex
{

Order::Order() : logger(Logger::getInstance())
{
}

Order::Order(const FIX8::TEX::NewOrderSingle& in) : Order::Order()
{
    in.copy_legal(this);
    FIX8::TEX::ClOrdID clOrdId;
    if (in.get(clOrdId))
        this->clOrdId = clOrdId.get();
    FIX8::TEX::SecondaryClOrdID secondaryClOrdId;
    if (in.get(secondaryClOrdId))
        this->secondaryClOrdId = secondaryClOrdId.get();
}

Order::~Order()
{
    if (server_ != nullptr)
        server_->unsubscribe_from_fix_events(this);
}

std::string Order::symbol() const { return get<FIX8::TEX::Symbol>()->get(); }
liquibook::book::Price Order::price() const { return get<FIX8::TEX::Price>()->get(); }
liquibook::book::Price Order::stop_price() const { return 0; } // not used
liquibook::book::Quantity Order::order_qty() const { return get<FIX8::TEX::OrderQty>()->get(); }
liquibook::book::Quantity Order::leaves_qty() const { return leaves_qty_; }
bool Order::is_buy() const { return get<FIX8::TEX::Side>()->get() == FIX8::TEX::Side_BUY; }
std::string Order::order_id() const { return orderId; }
std::string Order::cl_ord_id() const { return clOrdId; }
std::string Order::secondary_cl_ord_id() const { return secondaryClOrdId; }

const std::vector<Order::StateChange>& Order::history() const { return history_; }
const Order::StateChange& Order::current_state() const { return history_.back(); }

void Order::on_fix_server_changed(LatinexSessionServer* server) 
{ 
    if (server_ != nullptr)
        server_->unsubscribe_from_fix_events(this);
    server_ = server; 
    if (server_ != nullptr)
        server_->subscribe_to_fix_events(this);
}

FIX8::TEX::ExecutionReport* Order::build_execution_report()
{
    FIX8::TEX::ExecutionReport* er = new FIX8::TEX::ExecutionReport;
    copy_legal(er);
    (*er) << new FIX8::TEX::LeavesQty(leaves_qty_)
            << new FIX8::TEX::OrderID(orderId)
            << new FIX8::TEX::SecondaryOrderID(clOrdId)
            << new FIX8::TEX::SecondaryClOrdID(secondaryClOrdId);
    if (server_ != nullptr) // just to be sure
        (*er) << new FIX8::TEX::ExecID(server_->next_exec_id());
    else
        (*er) << new FIX8::TEX::ExecID("0");
    return er;
}

void Order::on_submitted()
{
    logger->debug("Order", "on_submitted: Received event from market, sending Execution report");
    std::string msg;
    msg += (is_buy() ? "BUY " : "SELL ");
    msg += std::to_string(order_qty());
    msg += " " + symbol() + " @";
    msg += (price() == 0 ? "MKT" : std::to_string(price()));
    history_.emplace_back(State::Submitted, msg);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_NEW) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_PENDING_NEW)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_submitted: Unable to send Execution report");
    }
}

void Order::on_accepted()
{
    logger->debug("Order", "on_accepted: Received event from market, sending Execution report");
    history_.emplace_back(State::Accepted);
    if (server_ != nullptr)
    {
        //TODO: send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_PENDING_NEW) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_NEW)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_accepted: Unable to send Execution report");
    }
}

void Order::on_rejected(const char* reason)
{
    logger->debug("Order", "on_rejected: Received event from market, sending Execution report");
    history_.emplace_back(State::Rejected, reason);
    leaves_qty_ = 0;
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_REJECTED) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_REJECTED)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_rejected: Unable to send Execution report");
    }
}

void Order::on_filled(liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost)
{
    logger->debug("Order", "on_filled: Received event from market, sending Execution report");
    this->leaves_qty_ -= fill_qty;
    std::string msg = std::to_string(fill_qty) + " for " + std::to_string(fill_cost);
    history_.emplace_back(State::Filled, msg);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_TRADE) 
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (leaves_qty_ > 0)
            *er << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_PARTIALLY_FILLED);
        else
            *er << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_FILLED);

        if (!server_->send(er, true))
            logger->debug("Order", "on_filled: Unable to send Execution report");
    }
}

void Order::on_cancel_requested()
{
    logger->debug("Order", "on_cancel_requested: Received event from market, sending Execution report");
    history_.emplace_back(State::CancelRequested);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_PENDING_CANCEL) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_PENDING_CANCEL)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", ":on_cancel_requested: Unable to send Execution report");
    }
}

void Order::on_cancelled()
{
    logger->debug("Order", "on_cancelled: Received event from market, sending Execution report");
    this->leaves_qty_ = 0;
    history_.emplace_back(State::Cancelled);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_CANCELED) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_CANCELED)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_cancelled: Unable to send Execution report");
    }
}

void Order::on_cancel_rejected(const char* reason)
{
    logger->debug("Order", "on_cancel_rejected: Received event from market, sending Execution report");
    history_.emplace_back(State::CancelRejected, reason);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_REJECTED) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_REJECTED)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_cancel_rejected: Unable to send Execution report");
    }
}

void Order::on_replace_requested(const int32_t& size_delta, liquibook::book::Price new_price)
{
    logger->debug("Order", "on_replace_requested: Received event from market, sending Execution report");
    std::string msg;
    if (size_delta != liquibook::book::SIZE_UNCHANGED)
        msg += "Quantity change: " + std::to_string(size_delta) + " ";
    if (new_price != liquibook::book::PRICE_UNCHANGED)
        msg += "New Price " + std::to_string(new_price);
    history_.emplace_back(State::ModifyRequested, msg);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_PENDING_REPLACE) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_PENDING_REPLACE)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_replace_requested: Unable to send Execution report");
    }
}

void Order::on_replaced(const int32_t& size_delta, liquibook::book::Price new_price)
{
    logger->debug("Order", "on_replaced: Received event from market, sending Execution report");
    std::string msg;
    if (size_delta != liquibook::book::SIZE_UNCHANGED)
    {
        (*this) << new FIX8::TEX::OrderQty(order_qty() + size_delta);
        leaves_qty_ += size_delta;
        msg += "Quantity change: " + std::to_string(size_delta) + " ";
    }
    if (new_price != liquibook::book::PRICE_UNCHANGED)
    {
        *(this) << new FIX8::TEX::Price(new_price);
        msg += "New Price " + std::to_string(new_price);
    }
    history_.emplace_back(State::Modified, msg);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_REPLACED) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_REPLACED)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_replaced: Unable to send Execution report");
    }
}

void Order::on_replace_rejected(const char* reason)
{
    logger->debug("Order", "on_replace_rejected: Received event from market, sending Execution report");
    history_.emplace_back(State::ModifyRejected, reason);
    if (server_ != nullptr)
    {
        // send execution report
        auto* er = build_execution_report();
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_REJECTED) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_REJECTED)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y');
        if (!server_->send(er, true))
            logger->debug("Order", "on_replace_rejected: Unable to send Execution report");
    }
}

} // namespace latinex

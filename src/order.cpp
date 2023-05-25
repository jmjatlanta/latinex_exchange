#include "order.h"
#include "server.h" // LatinexSessionServer
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"

namespace latinex
{

Order::Order()
{
}

Order::Order( const FIX8::TEX::NewOrderSingle& in)
{
    in.copy_legal(this);
}

Order::~Order()
{
    if (server_ != nullptr)
        server_->unsubscribe_from_fix_events(this);
}

std::string Order::symbol() const { return get<FIX8::TEX::Symbol>()->get(); }
liquibook::book::Price Order::price() const { return get<FIX8::TEX::Price>()->get(); }
liquibook::book::Price Order::stop_price() const { return 0; } // not used
liquibook::book::Quantity Order::order_qty() const { return get<FIX8::TEX::Quantity>()->get(); }
bool Order::is_buy() const { return get<FIX8::TEX::Side>()->get() == FIX8::TEX::Side_BUY; }

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

void Order::on_submitted()
{
    std::cout << "Order::on_submitted: Received event from market, sending Execution report\n";
    std::string msg;
    msg += (is_buy() ? "BUY " : "SELL ");
    msg += std::to_string(order_qty());
    msg += " " + symbol() + " @";
    msg += (price() == 0 ? "MKT" : std::to_string(price()));
    history_.emplace_back(State::Submitted, msg);
    if (server_ != nullptr)
    {
        // send execution report
        FIX8::TEX::ExecutionReport *er(new FIX8::TEX::ExecutionReport);
        copy_legal(er);
        *er << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_NEW) 
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_NEW)
            << new FIX8::TEX::LeavesQty(quantity_)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::Side(FIX8::TEX::Side_BUY)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y')
            << new FIX8::TEX::ExecID(id_);
        if (!server_->send(er, true))
            std::cout << "Order::on_submitted: Unable to send Execution report\n";
    }
}

void Order::on_accepted()
{
    std::cout << "Order::on_accepted: Received event from market, sending Execution report\n";
    quantity_remaining_ = quantity_;
    history_.emplace_back(State::Accepted);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_rejected(const char* reason)
{
    std::cout << "Order::on_rejected: Received event from market, sending Execution report\n";
    history_.emplace_back(State::Rejected, reason);
    if (server_ != nullptr)
    {
        // send execution report
        FIX8::TEX::ExecutionReport *er(new FIX8::TEX::ExecutionReport);
        *er << new FIX8::TEX::OrderID(id_)
            << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_REJECTED)
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_REJECTED)
            << new FIX8::TEX::LeavesQty(0.)
            << new FIX8::TEX::Side(FIX8::TEX::Side_BUY)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y')
            << new FIX8::TEX::ExecID(id_);
        if (!server_->send(er, true))
            std::cout << "Order::on_submitted: Unable to send Execution report\n";
    }
}

void Order::on_filled(liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost)
{
    std::cout << "Order::on_filled: Received event from market, sending Execution report\n";
    quantity_remaining_ -= fill_qty;
    fill_cost_ += fill_cost;
    std::string msg = std::to_string(fill_qty) + " for " + std::to_string(fill_cost);
    history_.emplace_back(State::Filled, msg);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_cancel_requested()
{
    std::cout << "Order::on_cancel_requested: Received event from market, sending Execution report\n";
    history_.emplace_back(State::CancelRequested);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_cancelled()
{
    std::cout << "Order::on_cancelled: Received event from market, sending Execution report\n";
    quantity_remaining_ = 0;
    history_.emplace_back(State::Cancelled);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_cancel_rejected(const char* reason)
{
    std::cout << "Order::on_cancel_rejected: Received event from market, sending Execution report\n";
    history_.emplace_back(State::CancelRejected, reason);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_replace_requested(const int32_t& size_delta, liquibook::book::Price new_price)
{
    std::cout << "Order::on_replace_requested: Received event from market, sending Execution report\n";
    std::string msg;
    if (size_delta != liquibook::book::SIZE_UNCHANGED)
        msg += "Quantity change: " + std::to_string(size_delta) + " ";
    if (new_price != liquibook::book::PRICE_UNCHANGED)
        msg += "New Price " + std::to_string(new_price);
    history_.emplace_back(State::ModifyRequested, msg);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_replaced(const int32_t& size_delta, liquibook::book::Price new_price)
{
    std::cout << "Order::on_replaced: Received event from market, sending Execution report\n";
    std::string msg;
    if (size_delta != liquibook::book::SIZE_UNCHANGED)
    {
        quantity_ += size_delta;
        quantity_remaining_ += size_delta;
        msg += "Quantity change: " + std::to_string(size_delta) + " ";
    }
    if (new_price != liquibook::book::PRICE_UNCHANGED)
    {
        price_ = new_price;
        msg += "New Price " + std::to_string(new_price);
    }
    history_.emplace_back(State::Modified, msg);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_replace_rejected(const char* reason)
{
    std::cout << "Order::on_replace_rejected: Received event from market, sending Execution report\n";
    history_.emplace_back(State::ModifyRejected, reason);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

} // namespace latinex

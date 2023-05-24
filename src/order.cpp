#include "order.h"
#include "server.h" // LatinexSessionServer
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"

namespace latinex
{

Order::Order(const std::string& id, bool buy_side, liquibook::book::Quantity quantity, const std::string& symbol,
        liquibook::book::Price price, liquibook::book::Price stopPrice, bool aon, bool ioc) :
        id_(id), buy_side_(buy_side), symbol_(symbol), quantity_(quantity), price_(price),
        stop_price_(stopPrice), ioc_(ioc), aon_(aon), quantity_filled_(0), quantity_remaining_(0),
        fill_cost_(0)
{
}

Order::~Order()
{
    if (server_ != nullptr)
        server_->unsubscribe_from_fix_events(this);
}

std::string Order::order_id() const { return id_; }
void Order::set_order_id(const std::string& in) { id_ = in; }
bool Order::is_limit() const { return price() != 0; }
bool Order::is_buy() const { return buy_side_; }
bool Order::all_or_none() const { return aon_; }
bool Order::immediate_or_cancel() const { return ioc_; }
std::string Order::symbol() const { return symbol_; }
liquibook::book::Price Order::price() const { return price_; }
liquibook::book::Price Order::stop_price() const { return stop_price_; }
liquibook::book::Quantity Order::order_qty() const { return quantity_; }
uint32_t Order::quantity_filled() const { return quantity_filled_; }
uint32_t Order::quantity_remaining() const { return quantity_remaining_; }
uint32_t Order::fill_cost() const { return fill_cost_; }
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
    msg += std::to_string(quantity_);
    msg += " " + symbol_ + " @";
    msg += (price_ == 0 ? "MKT" : std::to_string(price_));
    history_.emplace_back(State::Submitted, msg);
    if (server_ != nullptr)
    {
        // send execution report
        FIX8::TEX::ExecutionReport *er(new FIX8::TEX::ExecutionReport);
        *er << new FIX8::TEX::OrderID(id_)
            << new FIX8::TEX::ExecType(FIX8::TEX::ExecType_NEW)
            << new FIX8::TEX::OrdStatus(FIX8::TEX::OrdStatus_NEW)
            << new FIX8::TEX::LeavesQty(quantity_)
            << new FIX8::TEX::CumQty(0.)
            << new FIX8::TEX::AvgPx(0.)
            << new FIX8::TEX::LastCapacity('5')
            << new FIX8::TEX::ReportToExch('Y')
            << new FIX8::TEX::ExecID(id_);
        if (!server_->send(er, true))
            std::cout << "Order::on_submitted: Unable to send Execution report\n";
    }
}

void Order::on_accepted()
{
    quantity_remaining_ = quantity_;
    history_.emplace_back(State::Accepted);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_rejected(const char* reason)
{
    history_.emplace_back(State::Rejected, reason);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_filled(liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost)
{
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
    history_.emplace_back(State::CancelRequested);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_cancelled()
{
    quantity_remaining_ = 0;
    history_.emplace_back(State::Cancelled);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_cancel_rejected(const char* reason)
{
    history_.emplace_back(State::CancelRejected, reason);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

void Order::on_replace_requested(const int32_t& size_delta, liquibook::book::Price new_price)
{
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
    history_.emplace_back(State::ModifyRejected, reason);
    if (server_ != nullptr)
    {
        //TODO: send execution report
    }
}

} // namespace latinex

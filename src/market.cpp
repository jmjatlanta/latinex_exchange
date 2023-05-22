#include "market.h"

namespace latinex
{

Market::Market()
{
}

Market::~Market()
{
}

void Market::on_accept(const OrderPtr& order)
{
    // TODO: Notify that order was accepted
}

void Market::on_reject(const OrderPtr& order, const char* reason)
{
    // TODO: Notify that order was rejected
}

void Market::on_fill(const OrderPtr& order, const OrderPtr& matched_order,
        liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost)
{
}

void Market::on_cancel(const OrderPtr& order)
{
    //TODO: Implement
}

void Market::on_cancel_reject(const OrderPtr& order, const char* reason)
{
    //TODO: Implement
}

void Market::on_replace(const OrderPtr& order, const int64_t& size_delta, liquibook::book::Price new_price)
{
    //TODO: Implement
}

void Market::on_replace_reject(const OrderPtr& order, const char* reason)
{
    //TODO: Implement
}

void Market::on_trade(const OrderBook* book, liquibook::book::Quantity qty, liquibook::book::Cost cost)
{
    //TODO: Implement
}

void Market::on_order_book_change(const OrderBook* book)
{
    //TODO: Implement
}

void Market::on_bbo_change(const DepthOrderBook* book, const BookDepth* depth)
{
    //TODO: Implement
}

void Market::on_depth_change(const DepthOrderBook* book, const BookDepth* depth)
{
    //TODO: Implement
}

void Market::add_order(const Order& order)
{
    //TODO: Implement
}

void Market::cancel_order(const Order& order)
{
    //TODO: Implement
}

void Market::modify_order(const Order& order)
{
    //TODO: Implement
}

} // namespace latinex

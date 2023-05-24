#include "market.h"

#include <exception>

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
    order->on_accepted();
}

void Market::on_reject(const OrderPtr& order, const char* reason)
{
    order->on_rejected(reason);
}

void Market::on_fill(const OrderPtr& order, const OrderPtr& matched_order,
        liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost)
{
    order->on_filled(fill_qty, fill_cost);
    matched_order->on_filled(fill_qty, fill_cost);
}

void Market::on_cancel(const OrderPtr& order)
{
    order->on_cancelled();
}

void Market::on_cancel_reject(const OrderPtr& order, const char* reason)
{
    order->on_cancel_rejected(reason);
}

void Market::on_replace(const OrderPtr& order, const int64_t& size_delta, liquibook::book::Price new_price)
{
    order->on_replaced(size_delta, new_price);
}

void Market::on_replace_reject(const OrderPtr& order, const char* reason)
{
    order->on_replace_rejected(reason);
}

void Market::on_trade(const OrderBook* book, liquibook::book::Quantity qty, liquibook::book::Cost cost)
{
    //TODO: Implement, perhaps throw event to ticker feed
}

void Market::on_order_book_change(const OrderBook* book)
{
    //TODO: Implement, perhaps throw event to book feed
}

void Market::on_bbo_change(const DepthOrderBook* book, const BookDepth* depth)
{
    //TODO: Implement, perhaps throw event to bbo subscribers
}

void Market::on_depth_change(const DepthOrderBook* book, const BookDepth* depth)
{
    //TODO: Implement
}

void Market::add_books_as_needed(bool in) { addBooksAsNeeded = in; }

OrderBook& Market::add_book(const std::string& symbol, bool force)
{
    auto itr = books_.find(symbol);
    if (itr == books_.end())
    {
        if (!force && !addBooksAsNeeded)
            throw std::invalid_argument( std::string("Unrecognized symbol: ") + symbol);
        // try again with lock
        std::lock_guard<std::mutex> lock(books_mutex_);
        itr = books_.find(symbol);
        if (itr == books_.end())
        {
            // add the book
            auto& desired_book = books_[symbol];
            desired_book.set_order_listener(this);
            desired_book.set_trade_listener(this);
            desired_book.set_order_book_listener(this);
            book_mutexes_[symbol];
            return desired_book;
        }
    }
    // evidently someone else added it
    return (*itr).second;
}

OrderBook& Market::get_book(const std::string& symbol, bool depth_book)
{
    auto itr = books_.find(symbol);
    if (itr == books_.end())
        return add_book(symbol);
    return (*itr).second;
}

bool Market::add_order(OrderPtr order)
{
    auto& book = get_book(order->symbol(), false); // a simple, non depth book
    order->set_order_id(std::to_string(++orderIdSeed_));

    const liquibook::book::OrderConditions conditions = 
        (order->all_or_none() ? AON : NOC) | (order->immediate_or_cancel() ? IOC : NOC);

    // TODO: Add more sanity checks
    
    order->on_submitted();
    std::mutex& mut = book_mutexes_[order->symbol()];
    std::lock_guard<std::mutex> lock(mut);
    book.add(order, conditions);
    return true;
}

bool Market::cancel_order(OrderPtr order)
{
    get_book(order->symbol(), false).cancel(order);
    return true;
}

bool Market::modify_order(OrderPtr order)
{
    return get_book(order->symbol(), false).replace(order);
}

} // namespace latinex

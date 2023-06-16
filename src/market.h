#pragma once
/****
 * The main market object
 */
#include "logger.h"
#include <book/depth_order_book.h>
#include <book/types.h>

#include <memory>
#include <map>
#include <string>
#include <cstdint>
#include <atomic>
#include <mutex>

namespace latinex
{

template<typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

template <typename T>
class Market :
        public liquibook::book::OrderListener<std::shared_ptr<T>>, // when something happens to an order
        public liquibook::book::TradeListener<liquibook::book::OrderBook<std::shared_ptr<T>>>, // when something happens to a trade
        public liquibook::book::OrderBookListener<liquibook::book::OrderBook<std::shared_ptr<T>>>, // when something happens to the book
        public liquibook::book::BboListener<liquibook::book::DepthOrderBook<std::shared_ptr<T>>>, // when something happens to the BBO
        public liquibook::book::DepthListener<liquibook::book::DepthOrderBook<std::shared_ptr<T>>> // when something happens to the depth
{
    typedef std::shared_ptr<T> OrderPtr;
    typedef std::map<std::string, OrderPtr> OrderMap;
    typedef liquibook::book::OrderBook<OrderPtr> OrderBook;
    typedef std::shared_ptr<OrderBook> OrderBookPtr;
    typedef liquibook::book::DepthOrderBook<OrderPtr> DepthOrderBook;
    typedef std::shared_ptr<DepthOrderBook> DepthOrderBookPtr;
    typedef liquibook::book::Depth<> BookDepth;

public:
    Market() : logger(Logger::getInstance())
    { 
    } 

    ~Market() { }

    void add_order_listener(liquibook::book::OrderListener<std::shared_ptr<T>>* in) 
            { order_listeners_.emplace_back(in); }
    void add_trade_listener(liquibook::book::TradeListener<liquibook::book::OrderBook<std::shared_ptr<T>>>* in) 
            { trade_listeners_.emplace_back(in); }
    void add_order_book_listener(liquibook::book::OrderBookListener<liquibook::book::OrderBook<std::shared_ptr<T>>>* in) 
            { order_book_listeners_.emplace_back(in); }
    void add_bbo_listener(liquibook::book::BboListener<liquibook::book::DepthOrderBook<std::shared_ptr<T>>>* in) 
            { bbo_listeners_.emplace_back(in); }
    void add_depth_listener(liquibook::book::DepthListener<liquibook::book::DepthOrderBook<std::shared_ptr<T>>>* in) 
            { depth_listeners_.emplace_back(in); }
    
    // OrderListener interface implementation

    /****
     * @brief an order was accepted
     * @param order the order
     */
    virtual void on_accept(const OrderPtr& order)
    {
        logger->debug("Market", "onAccept called");
        order->on_accepted();
        for(auto* l : order_listeners_)
            l->on_accept(order);
    }

    virtual void on_reject(const OrderPtr& order, const char* reason)
    {
        logger->debug("Market", "on_reject called");
        order->on_rejected(reason);
        for(auto* l : order_listeners_)
            l->on_reject(order, reason);
    }

    /****
     * @brief a fill happened
     * @param order the order
     * @param matched_order the order that took the other side
     * @param fill_quty how many were filled
     * @param fill_cost the cost
     */
    virtual void on_fill(const OrderPtr& order, const OrderPtr& matched_order, 
            liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost)
    {
        logger->debug("Market", "on_fill called");
        order->on_filled(fill_qty, fill_cost);
        matched_order->on_filled(fill_qty, fill_cost);
        for(auto* l : order_listeners_)
            l->on_fill(order, matched_order, fill_qty, fill_cost);
    }

    /***
     * @brief An order was cancelled
     * @param order the order that was cancelled
     */
    virtual void on_cancel(const OrderPtr& order)
    {
        logger->debug("Market", "on_cancel called");
        order->on_cancelled();
        for(auto* l : order_listeners_)
            l->on_cancel(order);
    }

    /***
     * @brief an attempt was made to cancel an order, but the request was rejected
     * @param order the order
     * @param reason the reason
     */
    virtual void on_cancel_reject(const OrderPtr& order, const char* reason)
    {
        logger->debug("Market", "on_cancel_reject called");
        order->on_cancel_rejected(reason);
        for(auto* l : order_listeners_)
            l->on_cancel_reject(order, reason);
    }

    /****
     * @brief qty or price was adjusted on an open order
     * @param order the replaced order
     * @param size_delta the qty changed and direction
     * @param new_price the updated order price
     */
    virtual void on_replace(const OrderPtr& order, const int64_t& size_delta, liquibook::book::Price new_price)
    {
        logger->debug("Market", "on_replace called");
        order->on_replaced(size_delta, new_price);
        for(auto* l : order_listeners_)
            l->on_replace(order, size_delta, new_price);
    }

    /****
     * @brief an attempt was made to replace an order, but the request was rejected
     * @param order the order
     * @param reason the reason
     */
    virtual void on_replace_reject(const OrderPtr& order, const char* reason)
    {
        logger->debug("Market", "on_replace_reject called");
        order->on_replace_rejected(reason);
        for(auto* l : order_listeners_)
            l->on_replace_reject(order, reason);
    }

    // TradeListener interface implementation
    
    /****
     * @brief a trade happened on a particular book
     * @param book the book
     * @param qty the quantity traded
     * @param cost the trade price
     */
    virtual void on_trade(const OrderBook* book, liquibook::book::Quantity qty, liquibook::book::Cost cost)
    {
        logger->debug("Market", "on_trade called");
        for(auto* l : trade_listeners_)
            l->on_trade(book, qty, cost);
    }

    /****
     * @brief there was some change somewhere in the book
     * @param book the book
     */
    virtual void on_order_book_change(const OrderBook* book) override
    {
        logger->debug("Market", "on_order_book_change called");
        for(auto* l : order_book_listeners_)
            l->on_order_book_change(book);
    }
    
    // BboListener interface implementation

    /****
     * @brief the BBO changed
     * @param book the book that threw the event
     * @param depth the book depth
     */
    void on_bbo_change(const DepthOrderBook* book, const BookDepth* depth) override
    {
        logger->debug("Market", "on_bbo_change called");
        for(auto* l : bbo_listeners_)
            l->on_bbo_change(book, depth);
    }

    // DepthListener interface implementation

    /****
     * @brief the depth changed in a particular order book
     * @param book the book
     * @param depth the updated depth
     */
    void on_depth_change(const DepthOrderBook* book, const BookDepth* depth)
    {
        logger->debug("Market", "on_depth_change called");
        for(auto* l : depth_listeners_)
            l->on_depth_change(book, depth);
    }


    // typical input methods
    
    /****
     * @brief add an order to the book
     * @param order the new order
     * @returns true if order was submitted (only basic validation completed)
     */
    bool add_order(OrderPtr order)
    {
        logger->debug("Market", "Adding order " + order->order_id());
        try
        {
            std::string symbol = order->symbol();
            auto& book = get_book(symbol, false); // a simple, non depth book

            // TODO: Read this from incoming order
            const liquibook::book::OrderConditions conditions = NOC;

            // TODO: Add more sanity checks
        
            order->on_submitted();
            std::mutex& mut = book_mutexes_[symbol];
            std::lock_guard<std::mutex> lock(mut);
            // note: this returns true if order was immediately matched
            book.add(order, conditions);
        }
        catch (const std::invalid_argument& ia)
        {
            // we don't have a book
            order->on_rejected("Invalid Symbol");
            return false;
        }
        return true;
    }

    bool cancel_order(OrderPtr order)
    {
        get_book(order->symbol(), false).cancel(order);
        return true;
    }

    bool modify_order(OrderPtr order)
    {
        return get_book(order->symbol(), false).replace(order);
    }

    /****
     * @brief set to true to create a new book when a new symbol comes in
     * @param in the value for the flag
     */
    void add_books_as_needed(bool in) { addBooksAsNeeded = in; }

    /***
     * Add a new market book
     * @param symbol the unique symbol
     * @param force TRUE to ignore addBooksAsNeeded flag
     */
    liquibook::book::OrderBook<std::shared_ptr<T>>& add_book(const std::string& symbol, bool force = false)
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
                book_mutexes_[symbol]; // just to get the mutex into the map
                return desired_book;
            }
        }
        // evidently someone else added it
        return (*itr).second;
    }

    uint64_t get_next_order_id() { return ++orderIdSeed_; }

private:
    liquibook::book::OrderBook<std::shared_ptr<T>>& get_book(const std::string& symbol, bool depth_book)
    {
        auto itr = books_.find(symbol);
        if (itr == books_.end())
            return add_book(symbol);
        return (*itr).second;
    }
private:
    static constexpr liquibook::book::OrderConditions AON = to_underlying(liquibook::book::oc_all_or_none);
    static constexpr liquibook::book::OrderConditions IOC = to_underlying(liquibook::book::oc_immediate_or_cancel);
    static constexpr liquibook::book::OrderConditions NOC = to_underlying(liquibook::book::oc_no_conditions);
    std::atomic<uint64_t> orderIdSeed_ = 0;
    std::map<std::string, liquibook::book::OrderBook<std::shared_ptr<T>>> books_;
    std::map<std::string, std::mutex> book_mutexes_;
    bool addBooksAsNeeded = false;
    std::mutex books_mutex_;
    std::vector<liquibook::book::OrderListener<std::shared_ptr<T>>*> order_listeners_;
    std::vector<liquibook::book::TradeListener<liquibook::book::OrderBook<std::shared_ptr<T>>>*> trade_listeners_;
    std::vector<liquibook::book::OrderBookListener<liquibook::book::OrderBook<std::shared_ptr<T>>>*> order_book_listeners_;
    std::vector<liquibook::book::BboListener<liquibook::book::DepthOrderBook<std::shared_ptr<T>>>*> bbo_listeners_;
    std::vector<liquibook::book::DepthListener<liquibook::book::DepthOrderBook<std::shared_ptr<T>>>*> depth_listeners_;
    Logger* logger = nullptr;
};

} // namespace latinex

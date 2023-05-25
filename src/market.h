#pragma once
/****
 * The main market object
 */

#include "order.h"
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

class Order;

typedef std::shared_ptr<Order> OrderPtr;
typedef liquibook::book::OrderBook<OrderPtr> OrderBook;
typedef std::shared_ptr<OrderBook> OrderBookPtr;
typedef liquibook::book::DepthOrderBook<OrderPtr> DepthOrderBook;
typedef std::shared_ptr<DepthOrderBook> DepthOrderBookPtr;
typedef liquibook::book::Depth<> BookDepth;

class Market :
        public liquibook::book::OrderListener<OrderPtr>, // when something happens to an order
        public liquibook::book::TradeListener<OrderBook>, // when something happens to a trade
        public liquibook::book::OrderBookListener<OrderBook>, // when something happens to the book
        public liquibook::book::BboListener<DepthOrderBook>, // when something happens to the BBO
        public liquibook::book::DepthListener<DepthOrderBook> // when something happens to the depth
{
    typedef std::map<std::string, OrderPtr> OrderMap;

public:
    Market();
    ~Market();

    // OrderListener interface implementation

    /****
     * @brief an order was accepted
     * @param order the order
     */
    virtual void on_accept(const OrderPtr& order);
    virtual void on_reject(const OrderPtr& order, const char* reason);
    /****
     * @brief a fill happened
     * @param order the order
     * @param matched_order the order that took the other side
     * @param fill_quty how many were filled
     * @param fill_cost the cost
     */
    virtual void on_fill(const OrderPtr& order, const OrderPtr& matched_order, 
            liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost);
    /***
     * @brief An order was cancelled
     * @param order the order that was cancelled
     */
    virtual void on_cancel(const OrderPtr& order);
    /***
     * @brief an attempt was made to cancel an order, but the request was rejected
     * @param order the order
     * @param reason the reason
     */
    virtual void on_cancel_reject(const OrderPtr& order, const char* reason);
    /****
     * @brief qty or price was adjusted on an open order
     * @param order the replaced order
     * @param size_delta the qty changed and direction
     * @param new_price the updated order price
     */
    virtual void on_replace(const OrderPtr& order, const int64_t& size_delta, liquibook::book::Price new_price);
    /****
     * @brief an attempt was made to replace an order, but the request was rejected
     * @param order the order
     * @param reason the reason
     */
    virtual void on_replace_reject(const OrderPtr& order, const char* reason);

    // TradeListener interface implementation
    
    /****
     * @brief a trade happened on a particular book
     * @param book the book
     * @param qty the quantity traded
     * @param cost the trade price
     */
    virtual void on_trade(const OrderBook* book, liquibook::book::Quantity qty, liquibook::book::Cost cost);

    /****
     * @brief there was some change somewhere in the book
     * @param book the book
     */
    virtual void on_order_book_change(const OrderBook* book);

    // BboListener interface implementation

    /****
     * @brief the BBO changed
     * @param book the book that threw the event
     * @param depth the book depth
     */
    void on_bbo_change(const DepthOrderBook* book, const BookDepth* depth);

    // DepthListener interface implementation

    /****
     * @brief the depth changed in a particular order book
     * @param book the book
     * @param depth the updated depth
     */
    void on_depth_change(const DepthOrderBook* book, const BookDepth* depth);


    // typical input methods
    
    /****
     * @brief add an order to the book
     * @param order the new order
     * @returns true if order was submitted (only basic validation completed)
     */
    bool add_order(OrderPtr order);

    bool cancel_order(OrderPtr order);

    bool modify_order(OrderPtr order);

    /****
     * @brief set to true to create a new book when a new symbol comes in
     * @param in the value for the flag
     */
    void add_books_as_needed(bool in);

    /***
     * Add a new market book
     * @param symbol the unique symbol
     * @param force TRUE to ignore addBooksAsNeeded flag
     */
    OrderBook& add_book(const std::string& in, bool force = false);

private:
    OrderBook& get_book(const std::string& symbol, bool depth_book);
private:
    static constexpr liquibook::book::OrderConditions AON = to_underlying(liquibook::book::oc_all_or_none);
    static constexpr liquibook::book::OrderConditions IOC = to_underlying(liquibook::book::oc_immediate_or_cancel);
    static constexpr liquibook::book::OrderConditions NOC = to_underlying(liquibook::book::oc_no_conditions);
    std::atomic<uint32_t> orderIdSeed_ = 0;
    std::map<std::string, OrderBook> books_;
    std::map<std::string, std::mutex> book_mutexes_;
    bool addBooksAsNeeded = false;
    std::mutex books_mutex_;
};

} // namespace latinex

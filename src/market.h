#pragma once
/****
 * The main market object
 */

#include "Order.h"
#include <book/depth_order_book.h>

#include <memory>
#include <map>
#include <string>

namespace latinex
{

typedef liquibook::book::OrderBook<OrderPtr> OrderBook;
typedef std::shared_ptr<OrderBook> OrderBookPtr;
typedef liquibook::book::DepthOrderBook<OrderPtr> DepthOrderBook;
typedef std::shared_ptr<DepthORderBook> DepthOrderBookPtr;
typedef liquibook::book::Depth<> BookDepth;

class Market :
        public liquibook::book::OrderListener<OrderPtr>, // when something happens to an order
        public liquibook::book::TradeListener<OrderBook>, // when something happens to a trade
        public liquibook::book::OrderBookListener<OrderBook>, // when something happens to the book
        public liquibook::book::BboListener<DepthOrderBook>, // when something happens to the BBO
        public liquibook::book::DepthListener<DepthOrderBook> // when something happens to the depth
{
    typedef std::map<std::string, OrderPtr> OrderMap;
    typedef std::map<std::string, OrderBookPtr> SymbolToBookMap;

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
     */
    void add_order(const Order& order);

    void cancel_order(const Order& order);

    void modify_order(const Order& order);
};

} // namespace latinex

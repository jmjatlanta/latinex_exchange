#pragma once
#include "order.h"
#include "itch.h"

#include "zmq.h"
#include <thread>
#include <vector>

/***
 * A class that provides a datafeed for updates to orders, order books, and trades
 */

template <typename T>
class DataFeed :
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

    DataFeed() {
        context = zmq_init(1);
        socket = zmq_socket(context, ZMQ_PUB);
        zmq_connect(socket, "tcp://127.0.0.1:12001");
    }

    ~DataFeed()
    {
        if (socket != nullptr)
            zmq_close(socket);
        if (context != nullptr)
            zmq_term(context);
    }

    void subscribe_to_market(latinex::Market<T>* in)
    {
        in->add_order_listener(this);
        in->add_trade_listener(this);
        in->add_order_book_listener(this);
        in->add_bbo_listener(this);
        in->add_depth_listener(this);
    }
    // TODO: Implement all those interfaces

    template<typename MSGTYPE>
    bool send(const MSGTYPE& msg)
    {
        zmq_msg_t out_msg;
        size_t sz = msg.get_size();
        zmq_msg_init_size(&out_msg, sz);
        memcpy(zmq_msg_data(&out_msg), msg.get_record(), sz);
        zmq_send(socket, &out_msg, sz, 0);
        zmq_msg_close(&out_msg);
    }

    /****
     * @brief an order was accepted by the exchange
     * @param order the order
     */
    virtual void on_accept(const OrderPtr& order)
    {
        itch::add_order_with_mpid msg;
        send(msg);
    }

    virtual void on_reject(const OrderPtr& order, const char* reason)
    {
        /*
        itch::order_reject msg;
        send(msg);
        */
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
        // we may need a order_executed or order_executed_with_price if the price was different
        itch::order_executed_with_price msg;
        send(msg);
    }

    /***
     * @brief An order was cancelled
     * @param order the order that was cancelled
     */
    virtual void on_cancel(const OrderPtr& order)
    {
        itch::order_cancel msg;
        send(msg);
    }

    /***
     * @brief an attempt was made to cancel an order, but the request was rejected
     * @param order the order
     * @param reason the reason
     */
    virtual void on_cancel_reject(const OrderPtr& order, const char* reason)
    {
    }

    /****
     * @brief qty or price was adjusted on an open order
     * @param order the replaced order
     * @param size_delta the qty changed and direction
     * @param new_price the updated order price
     */
    virtual void on_replace(const OrderPtr& order, const int64_t& size_delta, liquibook::book::Price new_price)
    {
        itch::order_replace msg;
        send(msg);
    }

    /****
     * @brief an attempt was made to replace an order, but the request was rejected
     * @param order the order
     * @param reason the reason
     */
    virtual void on_replace_reject(const OrderPtr& order, const char* reason)
    {
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
        itch::trade msg;
        send(msg);
    }

    /****
     * @brief there was some change somewhere in the book
     * @param book the book
     */
    virtual void on_order_book_change(const OrderBook* book)
    {}
    
    // BboListener interface implementation

    /****
     * @brief the BBO changed
     * @param book the book that threw the event
     * @param depth the book depth
     */
    void on_bbo_change(const DepthOrderBook* book, const BookDepth* depth)
    {}

    // DepthListener interface implementation

    /****
     * @brief the depth changed in a particular order book
     * @param book the book
     * @param depth the updated depth
     */
    void on_depth_change(const DepthOrderBook* book, const BookDepth* depth)
    {}


    private:
    void *context = nullptr;
    void *socket = nullptr;
};


#pragma once
#include "order.h"
#include "itch.h"
#include "logger.h"
#include "zmq.h"
#include "latinex_config.h"
#include <thread>
#include <vector>
#include <time.h>


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

    DataFeed() : logger(latinex::Logger::getInstance()) 
    {
        context = zmq_init(1);
        if (context == nullptr)
            throw std::invalid_argument("Could not build context");
        socket = zmq_socket(context, ZMQ_PUB);
        if (socket == nullptr)
            throw std::invalid_argument("Unable to build socket");
        zmq_bind(socket, "tcp://127.0.0.1:12001");
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

    uint64_t ns_since_midnight()
    {
        timespec tp;
        if(clock_gettime(CLOCK_REALTIME, &tp) == -1)
            std::cout << "unable to get time. Errno: " << errno << "\n";
        uint64_t ns = (((uint64_t)tp.tv_sec - epoch_at_midnight_sec) * 1000000000) + tp.tv_nsec;
        if (ns > 86400000000000) // ns in a day
        {
            // get new midnight
            auto now = std::chrono::system_clock::now();
            auto today = std::chrono::floor< std::chrono::duration<int32_t,std::ratio<86400>>>(now);
            epoch_at_midnight_sec = std::chrono::duration_cast<std::chrono::seconds>(today.time_since_epoch()).count();
            return ns_since_midnight();
        }
        return ns;
    }

    template<typename MSGTYPE>
    bool send(const MSGTYPE& msg)
    {
        size_t sz = msg.get_size();
        zmq_send(socket, msg.get_record(), sz, 0);
        return true;
    }

    /****
     * @brief an order was accepted by the exchange
     * @param order the order
     */
    virtual void on_accept(const OrderPtr& order)
    {
        Quantity shares = order->order_qty();
        int32_t price = to_itch_price(order->price());
        logger->debug("ItchDataFeed", "on_accept called with price of " + std::to_string(price) 
                + " and qty of " + std::to_string(shares));
        itch::add_order_with_mpid msg;
        // STOCK_LOCATE
        // TRACKING_NUMBER
        msg.set_int(itch::add_order_with_mpid::TIMESTAMP, ns_since_midnight());
        msg.set_int(itch::add_order_with_mpid::ORDER_REFERENCE_NUMBER, strtol(order->order_id().c_str(), nullptr, 10));
        msg.set_string(itch::add_order_with_mpid::BUY_SELL_INDICATOR, order->is_buy() ? "B" : "S");
        msg.set_int(itch::add_order_with_mpid::SHARES, order->order_qty());
        msg.set_string(itch::add_order_with_mpid::STOCK, order->symbol());
        msg.set_int(itch::add_order_with_mpid::PRICE, price);
        //msg.set_string(itch::add_order_with_mpid::ATTRIBUTION, order->get_mpid());
        send(msg);
    }

    virtual void on_reject(const OrderPtr& order, const char* reason)
    {
        logger->debug("ItchDataFeed", "on_reject called");
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
        std::string debugMsg = "on_fill called with price of " + std::to_string(fill_cost)
                + " and qty of " + std::to_string(fill_qty);
        logger->debug("ItchDataFeed", debugMsg);
        // we may need a order_executed or order_executed_with_price if the price was different
        itch::order_executed_with_price msg;
        // MESSAGE_TYPE
        // STOCK_LOCATE
        // TRACKING_NUMBER
        msg.set_int(itch::order_executed_with_price::TIMESTAMP, ns_since_midnight());
        msg.set_int(itch::order_executed_with_price::ORDER_REFERENCE_NUMBER, strtol(order->order_id().c_str(), nullptr, 10));
        msg.set_int(itch::order_executed_with_price::EXECUTED_SHARES, fill_qty);
        // MATCH_NUMBER
        msg.set_string(itch::order_executed_with_price::PRINTABLE, "Y");
        msg.set_int(itch::order_executed_with_price::EXECUTION_PRICE, to_itch_price(fill_cost));
        send(msg);
    }

    /***
     * @brief An order was cancelled
     * @param order the order that was cancelled
     */
    virtual void on_cancel(const OrderPtr& order)
    {
        logger->debug("ItchDataFeed", "on_cancel called");
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
        logger->debug("ItchDataFeed", "on_cancel_reject called");
    }

    /****
     * @brief qty or price was adjusted on an open order
     * @param order the replaced order
     * @param size_delta the qty changed and direction
     * @param new_price the updated order price
     */
    virtual void on_replace(const OrderPtr& order, const int64_t& size_delta, liquibook::book::Price new_price)
    {
        logger->debug("ItchDataFeed", "on_replace called");
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
        logger->debug("ItchDataFeed", "on_replace_reject called");
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
        logger->debug("ItchDataFeed", "on_trade called");
        itch::trade msg;
        msg.set_int(itch::trade::TIMESTAMP, ns_since_midnight());
        msg.set_int(itch::trade::SHARES, qty);
        msg.set_string(itch::trade::STOCK, book->symbol());
        msg.set_int(itch::trade::PRICE, to_itch_price(cost));
        send(msg);
    }

    /****
     * @brief there was some change somewhere in the book
     * @param book the book
     */
    virtual void on_order_book_change(const OrderBook* book)
    {
        logger->debug("ItchDataFeed", "on_order_book_change called");
    }
    
    // BboListener interface implementation

    /****
     * @brief the BBO changed
     * @param book the book that threw the event
     * @param depth the book depth
     */
    void on_bbo_change(const DepthOrderBook* book, const BookDepth* depth)
    {
        logger->debug("ItchDataFeed", "on_bbo_change called");
    }

    // DepthListener interface implementation

    /****
     * @brief the depth changed in a particular order book
     * @param book the book
     * @param depth the updated depth
     */
    void on_depth_change(const DepthOrderBook* book, const BookDepth* depth)
    {
        logger->debug("ItchDataFeed", "on_depth_change called");
    }


    private:
    void *context = nullptr;
    void *socket = nullptr;
    std::atomic<uint64_t> epoch_at_midnight_sec = 0;
    latinex::Logger* logger = nullptr;
};


#include "local_book.h"
#include "../src/exchange_server.h"
#include "../src/exchange_client.h"
#include "../src/zmq_itch_feed_client.h"
#include "../src/latinex_config.h"

#include <thread>
#include <chrono>

/****
 * A simple bot that keeps the price in a range
 */

typedef LocalBook<Price, Quantity> TestBook;

class MyDataFeedClient : public ZmqItchFeedClient, public BBOListener<Price>
{
    public:
    MyDataFeedClient() : ZmqItchFeedClient() {}
    virtual ~MyDataFeedClient() {}

    virtual bool onAddOrder( const itch::add_order& in) override 
    { 
        ++num_orders_added; 
        // get the ticker
        std::string ticker = in.get_string(itch::add_order::STOCK);
        int32_t price = in.get_int(itch::add_order::PRICE);
        price *= 100;
        TestBook& book = get_book(ticker);
        std::cout << "Adding order with price of " << price << std::endl;
        if (in.get_string(itch::add_order::BUY_SELL_INDICATOR) == "B")
            book.add_bid(price, in.get_int(itch::add_order::SHARES));
        else
            book.add_ask(price, in.get_int(itch::add_order::SHARES));
        return true; 
    }

    virtual bool onAddOrderWithMpid(const itch::add_order_with_mpid& in) override 
    { 
        ++num_orders_added; 
        std::string ticker = in.get_string(itch::add_order_with_mpid::STOCK);
        TestBook& book = get_book(ticker);
        int32_t price = in.get_int(itch::add_order_with_mpid::PRICE);
        price *= 100;
        Quantity qty = in.get_int(itch::add_order_with_mpid::SHARES);
        std::cout << "Adding order_with_mpid with price of " << price << " and qty of " << qty << std::endl;
        if (in.get_string(itch::add_order_with_mpid::BUY_SELL_INDICATOR) == "B")
            book.add_bid(price, qty);
        else
            book.add_ask(price, qty);
        return true; 
    }

    void add_book(const std::string& ticker) 
    { 
        if (books.find(ticker) == books.end()) 
        {
            auto pr = books.try_emplace(ticker, ticker);
            (*pr.first).second.add_bbo_listener(this);
        }
    }

    TestBook& get_book(const std::string& ticker)
    {
        auto itr = books.find(ticker);
        if (itr == books.end())
            throw std::invalid_argument("Book not found: " + ticker);
        return (*itr).second;
    }

    Price get_bid(const std::string& ticker) 
    { 
        return bbos[ticker].first;
    }

    Price get_ask(const std::string& ticker) 
    { 
        return bbos[ticker].second;
    }
    
    size_t num_orders_added = 0;

    void on_new_best_bid(const std::string& ticker, Price price) override
    {
        std::cout << "on_new_best_bid called with price " << price << std::endl;
        bbos[ticker].first = price;
    }

    void on_new_best_ask(const std::string& ticker, Price price) override
    {
        std::cout << "on_new_best_ask called with price " << price << std::endl;
        bbos[ticker].second = price;
    }

    private:
    std::map<std::string, std::pair<Price, Price> > bbos;
    std::map<std::string, TestBook> books;
};

int main(int argc, char** argv)
{
    ExchangeClient client("../test/myfix_client.xml", "DLD1");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    MyDataFeedClient* datafeedClient = new MyDataFeedClient();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // ASSA is at 89.90
    // CANAL is at 2.50
    datafeedClient->add_book("ASSA");
    datafeedClient->add_book("CANAL");
    // add orders to make the range
    client.send_order(true, 100, "ASSA", to_price(89.65));
    client.send_order(false, 100, "ASSA", to_price(90.15));
    client.send_order(true, 100, "CANAL", to_price(2.25));
    client.send_order(false, 100, "CANAL", to_price(2.75));
    // get the current bid and ask
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    while(true)
    {
        // set a 50c range around the midpoint
        // if the current bid or ask is outside the range, add more orders    
        Price bid = datafeedClient->get_bid("ASSA");
        Price ask = datafeedClient->get_ask("ASSA");
        if (to_long_double(bid) <= 89.64)
            client.send_order(true, 100, "ASSA", to_price(89.65));
        if (to_long_double(ask) >= 90.16)
            client.send_order(true, 100, "ASSA", to_price(90.15));
        bid = datafeedClient->get_bid("CANAL");
        ask = datafeedClient->get_ask("CANAL");
        if (to_long_double(bid) <= 2.24)
            client.send_order(true, 100, "CANAL", to_price(2.25));
        if (to_long_double(ask) >= 2.76)
            client.send_order(true, 100, "CANAL", to_price(2.75));
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    

    delete datafeedClient;
}

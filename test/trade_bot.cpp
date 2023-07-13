#include "local_book.h"
#include "../src/exchange_server.h"
#include "../src/exchange_client.h"
#include "soup_itch_feed_client.h"
#include "../src/latinex_config.h"

#include <thread>
#include <chrono>

/****
 * A simple bot that keeps the price in a range
 */

typedef LocalBook<Price, Quantity> TestBook;

class MyDataFeedClient : public SoupItchFeedClient, public BBOListener<Price>
{
    public:
    MyDataFeedClient() : SoupItchFeedClient("127.0.0.1:12001") {}
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
        int32_t ask = bbos[ticker].second;
        if (ask == 0)
            ask = std::numeric_limits<int32_t>::max();
        return ask;
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

void check_orders(const std::string& ticker, long double desiredBid, long double desiredAsk, 
        MyDataFeedClient* dataFeed, ExchangeClient* client)
{
    Price bid = dataFeed->get_bid(ticker);
    Price ask = dataFeed->get_ask(ticker);
    std::cout << "For " << ticker << " Current bid is " << to_long_double(bid) 
            << " and current ask is " << to_long_double(ask) << std::endl;
    if (to_long_double(bid) < desiredBid)
    {
        std::cout << "Adding long order for 100 " << ticker << " at " << desiredBid << std::endl;
        client->send_order(true, 100, ticker, to_price(desiredBid));
    }
    if (to_long_double(ask) > desiredAsk)
    {
        std::cout << "Adding short order for 100 " << ticker << " at " << desiredAsk << std::endl;
        client->send_order(false, 100, ticker, to_price(desiredAsk));
    }
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Syntax: " << argv[0] << " client.xml DLD\n";
        return 0;
    }
    ExchangeClient client(argv[1], argv[2]);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    MyDataFeedClient* datafeedClient = new MyDataFeedClient();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // ASSA is at 89.90
    // CANAL is at 2.50
    datafeedClient->add_book("ASSA");
    datafeedClient->add_book("CANAL");
    // get the current bid and ask
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    while(true)
    {
        // set a 50c range around the midpoint
        // if the current bid or ask is outside the range, add more orders    
        check_orders("ASSA", 89.65, 90.15, datafeedClient, &client);
        check_orders("CANAL", 2.25, 2.75, datafeedClient, &client);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    

    delete datafeedClient;
}

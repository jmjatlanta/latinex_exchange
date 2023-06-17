#include <gtest/gtest.h>

#include "local_book.h"
#include "../src/exchange_server.h"
#include "../src/exchange_client.h"
#include "../src/zmq_itch_feed_client.h"
#include "../src/latinex_config.h"

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

class TestClient : public ExchangeClient
{
    public:
    TestClient() : ExchangeClient("../test/myfix_client.xml", "DLD1") {}
};

class TestServer : public ExchangeServer
{
    public:
    TestServer() : ExchangeServer("../test/myfix_server.xml") {}
};

TEST(Fix, createServer)
{
    TestServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST(Fix, createClient)
{
    TestClient myClient;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST(Fix, createClientAndServer)
{
    TestServer myServer;
    {
        TestClient myClient;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    // give time to log out
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

TEST(Fix, SendNewOrderSingleNoBook)
{
    TestServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    TestClient myClient;
    // give it a sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(myClient.send_order(true, 100, "ABC", 100));
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Fix, SendNewOrderSingleWithBook)
{
    TestServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(myServer.add_book("ABC"));
    TestClient myClient;
    // give it a sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(myClient.send_order(true, 100, "ABC", 100));
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Fix, OrderMatch)
{
    TestServer myServer;
    EXPECT_TRUE(myServer.add_book("ABC"));
    TestClient myClient;
    MyDataFeedClient dataFeedClient;
    // give it a sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(dataFeedClient.is_connected());
    EXPECT_TRUE(myClient.send_order(true, 100, "ABC", 100));
    EXPECT_TRUE(myClient.send_order(false, 100, "ABC", 100));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(dataFeedClient.num_orders_added, 2);
}

TEST(Fix, MultipleOrders)
{
    std::shared_ptr<TestServer> myServer = std::make_shared<TestServer>();
    EXPECT_TRUE(myServer->add_book("ABC"));

    std::shared_ptr<TestClient> client1 = std::make_shared<TestClient>();
    std::shared_ptr<MyDataFeedClient> datafeedClient1 = std::make_shared<MyDataFeedClient>();
    datafeedClient1->add_book("ABC");
    std::shared_ptr<TestClient> client2 = std::make_shared<TestClient>();
    std::shared_ptr<MyDataFeedClient> datafeedClient2 = std::make_shared<MyDataFeedClient>();
    datafeedClient2->add_book("ABC");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(datafeedClient1->is_connected());
    EXPECT_TRUE(datafeedClient2->is_connected());

    Price bidPrice = to_price(10.0);
    Price askPrice = to_price(10.5);
    std::cout << "Sending orders with bid/ask of " << bidPrice << "/" << askPrice << std::endl;
    client1->send_order(true, 100, "ABC", bidPrice);
    client1->send_order(false, 100, "ABC", askPrice);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // client 2 should know the spread
    EXPECT_EQ(datafeedClient2->get_bid("ABC"), bidPrice);
    EXPECT_EQ(datafeedClient2->get_ask("ABC"), askPrice);

    datafeedClient2 = nullptr;
    datafeedClient1 = nullptr;

    client1 = nullptr;
    client2 = nullptr;

    myServer = nullptr;
}

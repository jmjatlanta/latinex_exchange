#include <gtest/gtest.h>

#include "../src/exchange_server.h"
#include "../src/exchange_client.h"
#include "../src/zmq_itch_feed_client.h"

class MyDataFeedClient : public ZmqItchFeedClient
{
    public:
    MyDataFeedClient() : ZmqItchFeedClient() {}

    virtual bool onAddOrder( const itch::add_order& in) override 
    { 
        ++num_orders_added; 
        return true; 
    }
    virtual bool onAddOrderWithMpid(const itch::add_order_with_mpid& in) override 
    { 
        ++num_orders_added; 
        return true; 
    }

    size_t num_orders_added = 0;

};

class TestClient : public ExchangeClient
{
    public:
    TestClient() : ExchangeClient("../test/myfix_client.xml", "DLD1") {}
};

TEST(Fix, createServer)
{
    ExchangeServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST(Fix, createClient)
{
    TestClient myClient;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST(Fix, createClientAndServer)
{
    ExchangeServer myServer;
    {
        TestClient myClient;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    // give time to log out
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

TEST(Fix, SendNewOrderSingleNoBook)
{
    ExchangeServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    TestClient myClient;
    // give it a sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(myClient.send_order(true, 100, "ABC", 100));
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Fix, SendNewOrderSingleWithBook)
{
    ExchangeServer myServer;
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
    ExchangeServer myServer;
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


#include <gtest/gtest.h>

#include "../src/server.h"
#include "../src/client.h"
#include "../src/Myfix_types.hpp"
#include "../src/Myfix_classes.hpp"

#include <thread>
#include <vector>


class MyServer
{
    public:
    MyServer() : server( new FIX8::ServerSession<LatinexSessionServer>(
                FIX8::TEX::ctx(), "../test/myfix_server.xml", "TEX1")),
                market_(std::make_shared<latinex::Market<latinex::Order>>())
    {
        message_thread = std::thread(&MyServer::run, this);
    }
    virtual ~MyServer()
    {
        shutting_down = true;
        message_thread.join();
    }
    void run()
    {
        while(!shutting_down)
        {
            if (!server->poll())
                continue;
            if (!shutting_down)
            {
                server_process(server.get(), ++scnt, false);
            }
        }
    }
    void server_process(FIX8::ServerSessionBase* srv, int scnt, bool ismulti)
    {
        std::unique_ptr<FIX8::SessionInstanceBase> inst(srv->create_server_instance());
        LatinexSessionServer& session_server = static_cast<LatinexSessionServer&>( *inst->session_ptr() );
        session_server.market_ = market_;
        const FIX8::ProcessModel pm(srv->get_process_model(srv->_ses));
        inst->start(pm == FIX8::pm_pipeline, 0, 0);
        if (pm != FIX8::pm_pipeline)
            while(!inst->session_ptr()->is_shutdown())
                FIX8::hypersleep<FIX8::h_milliseconds>(100);
        // the session is finished
        std::cout << "MyServer::server_process: Connection to client " << scnt << " finished.\n";
        inst->stop(); 
        std::cout << "MyServer::server_process: Connection to client " << scnt << " stopped.\n";
    }
    bool add_book(const std::string& symbol)
    {
        market_->add_book(symbol, true);
        return true;
    }
    std::shared_ptr<latinex::Market<latinex::Order>> market_ = nullptr;
    std::thread message_thread;
    std::unique_ptr<FIX8::ServerSessionBase> server = nullptr;
    int scnt = 0;
    bool shutting_down = false;
};

class MyClient
{
    public:
    MyClient() : session_(new FIX8::ReliableClientSession<LatinexSessionClient>(
                FIX8::TEX::ctx(), "../test/myfix_client.xml", "DLD1"))
    {
        // single session reliable client
        session_->start(false, 0, 0);
        message_thread = std::thread(&MyClient::client_process, this);
    }
    virtual ~MyClient()
    {
        shutting_down = true;
        message_thread.join();
    }
    void client_process()
    {
        while (!shutting_down)
        {
            // do something
            std::this_thread::sleep_for( std::chrono::milliseconds(100) );
        }
        // do not explicitly call stop() with reliable sessions before checking if it is already shutdown
        if (!session_->session_ptr()->is_shutdown())
        {
            std::shared_ptr<FIX8::TEX::Logout> msg = std::make_shared<FIX8::TEX::Logout>();
            std::cout << "MyClient: Sending Logout\n";
            if (!send(msg.get()))
                std::cout << "MyClient::client_process: Logout failed to send.\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            if (!session_->session_ptr()->is_shutdown())
            {
                std::cout << "MyCLient::client_process: stopping\n";
                session_->session_ptr()->stop();
            }
        } 
    }
    bool send(FIX8::Message* msg)
    {
        FIX8::ClientSessionBase& clientSessionBase = *session_.get();
        LatinexSessionClient& myClient = static_cast<LatinexSessionClient&>(*clientSessionBase.session_ptr());
        return myClient.send(msg, false);
    }
    bool send_order(bool buy_side, int size, const std::string& symbol, int price)
    {
        // Create a NewOrderSingle and transmit
        std::unique_ptr<FIX8::TEX::NewOrderSingle> msg(new FIX8::TEX::NewOrderSingle);
        *(msg.get()) << new FIX8::TEX::TransactTime
            << new FIX8::TEX::OrderQty(size)
            << new FIX8::TEX::Price(price)
            << new FIX8::TEX::ClOrdID("myorderid")
            << new FIX8::TEX::Symbol(symbol)
            << new FIX8::TEX::HandlInst(FIX8::TEX::HandlInst_AUTOMATED_EXECUTION_ORDER_PRIVATE_NO_BROKER_INTERVENTION)
            << new FIX8::TEX::OrdType(FIX8::TEX::OrdType_LIMIT)
            << new FIX8::TEX::Side( (buy_side ? FIX8::TEX::Side_BUY : FIX8::TEX::Side_SELL) )
            << new FIX8::TEX::TimeInForce(FIX8::TEX::TimeInForce_FILL_OR_KILL);

        return send(msg.get());
    }
    std::unique_ptr<FIX8::ClientSessionBase> session_ = nullptr;
    bool shutting_down = false;
    std::thread message_thread;
};

TEST(Fix, createServer)
{
    MyServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST(Fix, createClient)
{
    MyClient myClient;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST(Fix, createClientAndServer)
{
    MyServer myServer;
    {
        MyClient myClient;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    // give time to log out
    std::cout << "createClientAndServer:: waiting for logout\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "createClientAndServer:: logout wait is over\n";
}

TEST(Fix, SendNewOrderSingleNoBook)
{
    MyServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    MyClient myClient;
    // give it a sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(myClient.send_order(true, 100, "ABC", 100));
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Fix, SendNewOrderSingleWithBook)
{
    MyServer myServer;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(myServer.add_book("ABC"));
    MyClient myClient;
    // give it a sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(myClient.send_order(true, 100, "ABC", 100));
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Fix, OrderMatch)
{
    MyServer myServer;
    EXPECT_TRUE(myServer.add_book("ABC"));
    MyClient myClient;
    // give it a sec
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(myClient.send_order(true, 100, "ABC", 100));
    EXPECT_TRUE(myClient.send_order(false, 100, "ABC", 100));
    std::this_thread::sleep_for(std::chrono::seconds(3));
}


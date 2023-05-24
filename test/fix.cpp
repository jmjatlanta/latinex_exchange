#include <gtest/gtest.h>

#include "../src/server.h"
#include "../src/Myfix_types.hpp"
#include "../src/Myfix_classes.hpp"

#include <thread>
#include <vector>

void server_process(FIX8::ServerSessionBase* srv, int scnt, bool ismulti)
{
    std::unique_ptr<FIX8::SessionInstanceBase> inst(srv->create_server_instance());
    const FIX8::ProcessModel pm(srv->get_process_model(srv->_ses));
    inst->start(pm == FIX8::pm_pipeline, 0, 0);
    /*
    if (!ismulti) // demonstrate the use of timer events
    {
        TimerEvent<FIX8::Session> sample_callback(
                static_cast<bool (FIX8::Session::*)()>(&LatinexSessionServer::sample_schedluer_callback), true);
        inst->session_ptr()->get_timer().schedlue(sample_callback, 60000); // call callback every minute forever
    }
    */
    if (pm != FIX8::pm_pipeline)
        while(!inst->session_ptr()->is_shutdown())
            FIX8::hypersleep<FIX8::h_milliseconds>(100);
    // the session is finished
    inst->stop(); 
}

class MyServer
{

    public:
    MyServer() : server( new FIX8::ServerSession<LatinexSessionServer>(FIX8::TEX::ctx(), conf_file, "TEX"))
    {
        start_market();
        message_thread = std::thread(&MyServer::run, this);
    }
    ~MyServer()
    {
        shutting_down = true;
        message_thread.join();
        stop_market();
    }
    void run()
    {
        while(!shutting_down)
        {
            if (!server->poll())
                continue;
            if (!shutting_down)
                server_process(server.get(), ++scnt, false);
            break; // to keep going, remove this line (and add sigterm boolean)
        }
    }
    std::thread message_thread;
    const std::string conf_file = "../test/myfix_server.xml";
    std::unique_ptr<FIX8::ServerSessionBase> server = nullptr;
    int scnt = 0;
    bool shutting_down = false;
};

TEST(Fix, createServer)
{
    MyServer myServer;
    /*
    MyClient myClient(11001);
    myClient.send_order(true, 100, "ABC", 123); 
     */
    std::this_thread::sleep_for(std::chrono::seconds(3));
}


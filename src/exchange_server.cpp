#include "exchange_server.h"

ExchangeServer::ExchangeServer() : server( new FIX8::ServerSession<LatinexSessionServer>(
                FIX8::TEX::ctx(), "../test/myfix_server.xml", "TEX1")),
                market_(std::make_shared<latinex::Market<latinex::Order>>()),
                exec_id_counter_(std::make_shared<std::atomic<uint64_t>>())
{
    dataFeed.subscribe_to_market(&(*market_));
    message_thread = std::thread(&ExchangeServer::run, this);
}

ExchangeServer::~ExchangeServer()
{
    shutting_down = true;
    message_thread.join();
}

void ExchangeServer::run()
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

void ExchangeServer::server_process(FIX8::ServerSessionBase* srv, int scnt, bool ismulti)
{
    std::unique_ptr<FIX8::SessionInstanceBase> inst(srv->create_server_instance());
    LatinexSessionServer& session_server = static_cast<LatinexSessionServer&>( *inst->session_ptr() );
    session_server.market_ = market_;
    session_server.exec_id_counter_ = exec_id_counter_;
    const FIX8::ProcessModel pm(srv->get_process_model(srv->_ses));
    inst->start(pm == FIX8::pm_pipeline, 0, 0);
    if (pm != FIX8::pm_pipeline)
        while(!inst->session_ptr()->is_shutdown())
            FIX8::hypersleep<FIX8::h_milliseconds>(100);
    // the session is finished
    std::cout << "ExchangeServer::server_process: Connection to client " << scnt << " finished.\n";
    inst->stop(); 
    std::cout << "ExchangeServer::server_process: Connection to client " << scnt << " stopped.\n";
}

bool ExchangeServer::add_book(const std::string& symbol)
{
    market_->add_book(symbol, true);
    return true;
}


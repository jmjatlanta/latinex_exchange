#include "exchange_server.h"

ExchangeServer::ExchangeServer(const std::string& xml_file) : server( new FIX8::ServerSession<LatinexSessionServer>(
                FIX8::TEX::ctx(), xml_file, "TEX1")),
                logger(latinex::Logger::getInstance()),
                market_(std::make_shared<latinex::Market<latinex::Order>>()),
                exec_id_counter_(std::make_shared<std::atomic<uint64_t>>())
{
    market_->add_books_as_needed(true);
    dataFeed.subscribe_to_market(&(*market_));
    message_thread = std::thread(&ExchangeServer::run, this);
}

ExchangeServer::~ExchangeServer()
{
    shutting_down = true;
    std::cerr << "Joining connection threads" << std::endl;
    for(auto& t : connectionThreads)
        t.join();
    std::cerr << "Joining message thread" << std::endl;
    message_thread.join();
    std::cerr << "ExchangeServer::dtor complete\n";
}

void ExchangeServer::run()
{
    while(!shutting_down)
    {
        if (!server->poll())
            continue;
        if (!shutting_down)
        {
            connectionThreads.emplace_back(&ExchangeServer::server_process, this, server.get(), ++scnt, false);
        }
    }
}

/***
 * This gets called when a new session connects
 * @param srv
 * @param scnt
 * @param ismulti
 */
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
    logger->debug("ExchangeServer", "server_process: Connection to client " + std::to_string(scnt) + " finished.");
    inst->stop(); 
    logger->debug("ExchangeServer", "server_process: Connection to client " + std::to_string(scnt) + " stopped.");
}

bool ExchangeServer::add_book(const std::string& symbol)
{
    market_->add_book(symbol, true);
    return true;
}


#pragma once
#include "server.h"
#include "client.h"
#include "data_feed.h"
#include "logger.h"
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"

#include <thread>
#include <vector>

/***
 * This class acts as an exchange
 * It has books
 * It has a FIX front-end for OMS
 * It transmits an ITCH feed for market data
 */

class ExchangeServer
{
    public:
    ExchangeServer(const std::string& xml_file);
    virtual ~ExchangeServer();
    bool add_book(const std::string& symbol);

    private:
    void run();
    /***
     * This gets called when a new session connects
     * @param srv
     * @param scnt
     * @param ismulti
     */
    void server_process(FIX8::ServerSessionBase* srv, int scnt, bool ismulti);

    public:
    std::shared_ptr<latinex::Market<latinex::Order>> market_ = nullptr; // the market this server is taking care of
    std::shared_ptr<std::atomic<uint64_t>> exec_id_counter_ = nullptr; // a singleton of a counter
    std::shared_ptr<std::atomic<uint64_t>> tracking_counter_ = nullptr; // NASDAQ tracking number

    private:
    std::thread message_thread; // handles new sessions
    std::unique_ptr<FIX8::ServerSessionBase> server = nullptr; // the FIX8 server (this class could be subclass of?)
    std::atomic<uint32_t> scnt = 0; // session counter
    bool shutting_down = false;
    DataFeed<latinex::Order> dataFeed;
    latinex::Logger* logger = nullptr;
    std::vector<std::thread> connectionThreads;
};


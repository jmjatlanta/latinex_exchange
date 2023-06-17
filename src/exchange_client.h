#pragma once
#include "server.h"
#include "client.h"
#include "latinex_config.h"
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"

#include <thread>
#include <vector>

class ExchangeClient : public FIX8::ReliableClientSession<LatinexSessionClient>
{
    public:
    ExchangeClient(const std::string& client_xml, const std::string& dld_name);
    virtual ~ExchangeClient();
    bool send(FIX8::Message* msg);
    bool send_order(bool buy_side, int size, const std::string& symbol, Price price);
    bool is_logged_in() { return (connection == nullptr ? false : connection->is_logged_in()); }

    private:
    void client_process();

    private:
    bool shutting_down = false;
    std::thread message_thread;
    LatinexSessionClient* connection = nullptr;
};


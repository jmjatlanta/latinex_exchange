#pragma once
#include "server.h"
#include "client.h"
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"

#include <thread>
#include <vector>

class ExchangeClient
{
    public:
    ExchangeClient(const std::string& client_xml, const std::string& dld_name);
    virtual ~ExchangeClient();
    bool send(FIX8::Message* msg);
    bool send_order(bool buy_side, int size, const std::string& symbol, int price);

    public:
    std::unique_ptr<FIX8::ClientSessionBase> session_ = nullptr;

    private:
    void client_process();

    private:
    bool shutting_down = false;
    std::thread message_thread;
};


#include "server.h"
#include "helper.h"
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"

LatinexSessionServer::~LatinexSessionServer()
{
    // make a copy of the vector so modifications don't affect iteration
    std::vector<latinex::Order*> cp = subscribed_orders_;
    // go through market orders and unset this server
    for(auto* o : cp)
        o->on_fix_server_changed(nullptr);
}

bool LatinexSessionServer::handle_application(const unsigned seqnum, const FIX8::Message *&msg)
{
    std::cout << "LatinexSessionServer::handle_application called with seqnum [" << seqnum << "] and a message\n";
    if (enforce(seqnum, msg))
        return false;

    if (!msg->process(router_)) // false means I have taken ownership of the message
    {
        std::cout << "LatinexSessionServer::handle_application: ownership of message taken\n";
        detach(msg);
    }
    else
    {
        std::cout << "LatinexSessionServer::handle_application: ownership of message not taken\n";
    }
    return true;
}

void LatinexSessionServer::state_change(const FIX8::States::SessionStates before, const FIX8::States::SessionStates after)
{
    //TODO: Do something
    std::cout << "LatinexSessionServer::state_change called. Old status: [" << to_string(before) 
        << "] New status: [" << to_string(after) << "]\n";
}

bool LatinexSessionServer::sample_scheduler_callback()
{
    return true;
}

bool TexRouterServer::operator() (const FIX8::TEX::Logout* msg) const
{
    std::cout << "TexRouterServer: Received Logoff\n";
    return true;
}

/***
 * @brief handle an incoming NewOrderSingle
 * @param msg the message
 * @returns ??
 */
bool TexRouterServer::operator() (const FIX8::TEX::NewOrderSingle *msg) const
{
    std::cout << "TexRouterServer: Received NewOrderSingle\n";
    // get the basic values needed to validate and send to the exchange
    std::string id;
    bool buy_side;
    liquibook::book::Quantity quantity;
    std::string symbol;
    liquibook::book::Price price;
    liquibook::book::Price stopPrice;
    bool aon = false;
    bool ioc = false;
    // parse the message
    {
        if (msg->has<FIX8::TEX::OrderQty>())
            quantity = msg->get<FIX8::TEX::OrderQty>()->get();
        if (msg->has<FIX8::TEX::Price>())
            price = msg->get<FIX8::TEX::Price>()->get();
        if (msg->has<FIX8::TEX::Symbol>())
            symbol = msg->get<FIX8::TEX::Symbol>()->get();
    }
    // after some validation, we should submit a new FixOrder to the exchange
    std::shared_ptr<latinex::Order> ord = std::make_shared<latinex::Order>(*msg);
    LatinexSessionServer& server = static_cast<LatinexSessionServer&>(session_);
    ord->on_fix_server_changed(&server);
    if (server.market_ != nullptr && !server.market_->add_order(ord))
    {
        ord->on_fix_server_changed(nullptr);
        std::cout << "TexRouterServer::NewOrderSingle: Unable to add order to market.\n";
    }
    return true;
}


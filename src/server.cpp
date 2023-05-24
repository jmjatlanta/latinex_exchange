#include "server.h"

#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"

std::shared_ptr<latinex::Market> market;

void start_market()
{
    market = std::make_shared<latinex::Market>();
}
void stop_market()
{
    market = nullptr;
}

bool LatinexSessionServer::handle_application(const unsigned seqnum, const FIX8::Message *&msg)
{
    if (enforce(seqnum, msg))
        return false;

    if (!msg->process(router_)) // false means I have taken ownership of the message
        detach(msg);
    return true;
}

void LatinexSessionServer::state_change(const FIX8::States::SessionStates before, const FIX8::States::SessionStates after)
{
    //TODO: Do something
}

bool LatinexSessionServer::sample_scheduler_callback()
{
    return true;
}

/***
 * @brief handle an incoming NewOrderSingle
 * @param msg the message
 * @returns ??
 */
bool TexRouterServer::operator() (const FIX8::TEX::NewOrderSingle *msg) const
{
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
    //Order(const std::string& id, bool buy_side, liquibook::book::Quantity quantity, const std::string& symbol,
    //             liquibook::book::Price price, liquibook::book::Price stopPrice, bool aon, bool ioc);
    std::shared_ptr<latinex::Order> ord = std::make_shared<latinex::Order>(id, buy_side, quantity, symbol,
           price, stopPrice, aon, ioc );
    ord->set_fix_server(&session_);
    if (market == nullptr)
        throw std::invalid_argument("No market");
    market->add_order(ord);
    return true;
}


#include "exchange_client.h"

std::atomic<uint16_t> ExchangeClient::next_connection_id = 1;

ExchangeClient::ExchangeClient(const std::string& xml_filename, const std::string& dld_name) 
            : FIX8::ReliableClientSession<LatinexSessionClient>( FIX8::TEX::ctx(), xml_filename, dld_name),
            logger(latinex::Logger::getInstance())
{
    logger->debug("ExchangeClient", "In ctor");
    // single session reliable client
    start(false, 0, 0);
    message_thread = std::thread(&ExchangeClient::client_process, this, ++next_connection_id);
}

ExchangeClient::~ExchangeClient()
{
    shutting_down = true;
    message_thread.join();
    std::cerr << "ExchangeClient::dtor completed\n";
}

void ExchangeClient::client_process(uint16_t conn_id)
{
    logger->debug("ExchangeClient", "In client_process with id of " + std::to_string(conn_id));
    while (!shutting_down)
    {
        if (connection == nullptr && session_ptr() != nullptr)
        {
            connection = session_ptr();
            logger->debug("ExchangeClient", "set session_ptr for connection " + std::to_string(conn_id));
        }
        // do something
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    }
    logger->debug("ExchangeClient", "shutting down connection " + std::to_string(conn_id));
    // do not explicitly call stop() with reliable sessions before checking if it is already shutdown
    if (!session_ptr()->is_shutdown())
    {
        FIX8::TEX::Logout msg;
        logger->debug("ExchangeClient", "Sending Logout for connection " + std::to_string(conn_id));
        try {
            if (!send(&msg))
                logger->debug("ExchangeClient", "client_process: Logout failed to send for connection"
                        + std::to_string(conn_id));
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            if (!session_ptr()->is_shutdown())
            {
                logger->debug("ExchangeClient", "client_process: stopping" + std::to_string(conn_id));
                session_ptr()->stop();
        }
        } catch (...) {
            logger->error("ExchangeClient", "client_process caught unknown error for connection" 
                    + std::to_string(conn_id));
        }
    } 
}

bool ExchangeClient::send(FIX8::Message* msg)
{
    LatinexSessionClient* myClient = static_cast<LatinexSessionClient*>(session_ptr());
    if (myClient != nullptr)
        return myClient->send(msg, false);
    return false;
}

bool ExchangeClient::send_order(bool buy_side, int size, const std::string& symbol, Price price)
{
    // Create a NewOrderSingle and transmit
    std::unique_ptr<FIX8::TEX::NewOrderSingle> msg(new FIX8::TEX::NewOrderSingle);
    *(msg.get()) << new FIX8::TEX::TransactTime
        << new FIX8::TEX::OrderQty(size)
        << new FIX8::TEX::Price(to_long_double(price))
        << new FIX8::TEX::ClOrdID("myorderid")
        << new FIX8::TEX::Symbol(symbol)
        << new FIX8::TEX::HandlInst(FIX8::TEX::HandlInst_AUTOMATED_EXECUTION_ORDER_PRIVATE_NO_BROKER_INTERVENTION)
        << new FIX8::TEX::OrdType(FIX8::TEX::OrdType_LIMIT)
        << new FIX8::TEX::Side( (buy_side ? FIX8::TEX::Side_BUY : FIX8::TEX::Side_SELL) )
        << new FIX8::TEX::TimeInForce(FIX8::TEX::TimeInForce_FILL_OR_KILL);

    return send(msg.get());
}

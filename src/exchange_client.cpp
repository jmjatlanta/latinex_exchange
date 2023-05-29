#include "exchange_client.h"

ExchangeClient::ExchangeClient() : session_(new FIX8::ReliableClientSession<LatinexSessionClient>(
            FIX8::TEX::ctx(), "../test/myfix_client.xml", "DLD1"))
{
    // single session reliable client
    session_->start(false, 0, 0);
    message_thread = std::thread(&ExchangeClient::client_process, this);
}

ExchangeClient::~ExchangeClient()
{
    shutting_down = true;
    message_thread.join();
}

void ExchangeClient::client_process()
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
        std::cout << "ExchangeClient: Sending Logout\n";
        if (!send(msg.get()))
            std::cout << "ExchangeClient::client_process: Logout failed to send.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        if (!session_->session_ptr()->is_shutdown())
        {
            std::cout << "ExchangeCLient::client_process: stopping\n";
            session_->session_ptr()->stop();
        }
    } 
}

bool ExchangeClient::send(FIX8::Message* msg)
{
    FIX8::ClientSessionBase& clientSessionBase = *session_.get();
    LatinexSessionClient& myClient = static_cast<LatinexSessionClient&>(*clientSessionBase.session_ptr());
    return myClient.send(msg, false);
}

bool ExchangeClient::send_order(bool buy_side, int size, const std::string& symbol, int price)
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

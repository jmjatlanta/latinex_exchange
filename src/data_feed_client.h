#pragma once

#include "itch.h"
#include "zmq.h"
#include <thread>

class DataFeedClient
{
    public:
    DataFeedClient() {
        context = zmq_init(1);
        socket = zmq_socket(context, ZMQ_SUB);
        if (zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0) != 0)
            throw std::invalid_argument("Invalid socket option");
        if (zmq_connect(socket, "tcp://127.0.0.1:12001") != 0)
            throw std::invalid_argument("Unable to connect to data server");
        msg_thread = std::thread(&DataFeedClient::run, this);
    }

    ~DataFeedClient()
    {
        shutting_down = true;
        msg_thread.join();
        if (socket != nullptr)
            zmq_close(socket);
        if (context != nullptr)
            zmq_term(context);
    }

    virtual bool is_connected() const { return socket != nullptr; }
    virtual bool onSystemEvent(const itch::system_event& in) { return true; }
    virtual bool onStockDirectory(const itch::stock_directory& in) { return true; }
    virtual bool onStockTradingAction(const itch::stock_trading_action& in) { return true; }
    virtual bool onRegShoRestriction(const itch::reg_sho_restriction& in) { return true; }
    virtual bool onMarketParticipantPosition(const itch::market_participant_position& in) { return true; }
    virtual bool onMwcpDeclineLevel(const itch::mwcp_decline_level& in) { return true; }
    virtual bool onMwcpStatus(const itch::mwcp_status& in) { return true; }
    virtual bool onIpoQuotingPeriodUpdate(const itch::ipo_quoting_period_update& in) { return true; }
    virtual bool onLuldAuctionCollar(const itch::luld_auction_collar& in) { return true; }
    virtual bool onOperationalHalt(const itch::operational_halt& in) { return true; }
    virtual bool onAddOrder( const itch::add_order& in) { return true; }
    virtual bool onAddOrderWithMpid(const itch::add_order_with_mpid& in) { return true; }
    virtual bool onOrderExecuted(const itch::order_executed& in) { return true; }
    virtual bool onOrderExecutedWithPrice(const itch::order_executed_with_price& in) { return true; }
    virtual bool onOrderCancel(const itch::order_cancel& in) { return true; }
    virtual bool onOrderDelete(const itch::order_delete& in) { return true; }
    virtual bool onOrderReplace(const itch::order_replace& in) { return true; }
    virtual bool onTrade(const itch::trade& in) { return true; }
    virtual bool onTradeNonCross(const itch::trade_non_cross& in) { return true; }
    virtual bool onBrokenTrade(const itch::broken_trade& in) { return true; }
    virtual bool onNoii(const itch::noii& in) { return true; }
    virtual bool onRpii( const itch::rpii& in) { return true; }
    virtual bool onDirectListingWithCapitalRaisePriceDiscovery(
            const itch::direct_listing_with_capital_raise_price_discovery& in) { return true; }

    protected:
    void run()
    {
        while(!shutting_down)
        {
            zmq_msg_t in_msg;
            zmq_msg_init(&in_msg);
            int ret_val = zmq_recv(socket, &in_msg, 0, ZMQ_NOBLOCK);
            if (ret_val > 0)
            {
                std::cout << "DataFeedClient::run First message length is " << ret_val << "\n";
                int size = zmq_msg_size(&in_msg);
                std::vector<uint8_t> bytes(size);
                memcpy((char*)&bytes[0], zmq_msg_data(&in_msg), size); 
                if(!onMessage(bytes))
                    std::cout << "DataFeedClient::run onMessage call returned false\n";
            }
            else
            {
                if (ret_val < 0 && errno != EAGAIN)
                {
                    std::cout << "DataFeedClient::run Uh Oh! ret_val was " << ret_val 
                        << " and errno was " << errno << "\n";
                    break;
                }
                else
                {
                    std::cout << "DataFeedClient::run Again! ret_val was " << ret_val << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
        }
        std::cout << "DataFeedClient::run completed.\n";
    }
    virtual bool onMessage(const std::vector<uint8_t>& bytes)
    {
        // NOTE: This blocks the receiver until processing is complete
        const unsigned char* record = bytes.data();
        std::cout << "DataFeedClient::onMessage received a record of type " << (char)bytes[0] << "\n";
        switch(bytes[0]) {
            case 'S':
                return onSystemEvent( itch::system_event(record) );
            case 'R':
                return onStockDirectory( itch::stock_directory(record));
            case 'H':
                return onStockTradingAction( itch::stock_trading_action(record) );
            case 'Y':
                return onRegShoRestriction( itch::reg_sho_restriction(record) );
            case 'L':
                return onMarketParticipantPosition( itch::market_participant_position(record) );
            case 'V':
                return onMwcpDeclineLevel( itch::mwcp_decline_level(record) );
            case 'W':
                return onMwcpStatus( itch::mwcp_status(record) );
            case 'K':
                return onIpoQuotingPeriodUpdate( itch::ipo_quoting_period_update(record) );
            case 'J':
                return onLuldAuctionCollar( itch::luld_auction_collar(record) );
            case 'h':
                return onOperationalHalt( itch::operational_halt( record ) );
            case 'A':
                return onAddOrder( itch::add_order( record ) );
            case 'F':
                return onAddOrderWithMpid( itch::add_order_with_mpid(record) );
            case 'E':
                return onOrderExecuted( itch::order_executed(record) );
            case 'C':
                return onOrderExecutedWithPrice( itch::order_executed_with_price(record) );
            case 'X':
                return onOrderCancel( itch::order_cancel(record) );
            case 'D':
                return onOrderDelete( itch::order_delete(record) );
            case 'U':
                return onOrderReplace( itch::order_replace(record) );
            case 'P':
                return onTrade( itch::trade(record) );
            case 'Q':
                return onTradeNonCross( itch::trade_non_cross(record) );
            case 'B':
                return onBrokenTrade( itch::broken_trade(record) );
            case 'I':
                return onNoii( itch::noii(record) );
            case 'N':
                return onRpii( itch::rpii(record) );
            case 'O':
                return onDirectListingWithCapitalRaisePriceDiscovery( 
                        itch::direct_listing_with_capital_raise_price_discovery(record) );
            default:
                return false;
        }
        return false;
    }

    std::thread msg_thread;
    bool shutting_down = false;
    void* context = nullptr;
    void* socket = nullptr;
};

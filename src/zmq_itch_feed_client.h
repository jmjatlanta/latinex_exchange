#pragma once

#include "itch.h"
#include "zmq.h"
#include <thread>

/****
 * Reads ITCH over ZMQ
 */

class ZmqItchFeedClient
{
    public:
    ZmqItchFeedClient() {
        context = zmq_init(1);
        socket = zmq_socket(context, ZMQ_SUB);
        if (zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0) != 0)
            throw std::invalid_argument("Invalid socket option");
        if (zmq_connect(socket, "tcp://127.0.0.1:12001") != 0)
            throw std::invalid_argument("Unable to connect to data server");
        msg_thread = std::thread(&ZmqItchFeedClient::run, this);
    }

    ~ZmqItchFeedClient()
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
    virtual bool onCrossTrade(const itch::cross_trade& in) { return true; }
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
            size_t buf_size = 10000;
            char buf[10000];
            int ret_val = zmq_recv(socket, buf, buf_size, ZMQ_NOBLOCK);
            if (ret_val > 0)
            {
                std::vector<uint8_t> bytes(ret_val);
                memcpy((char*)&bytes[0], buf, ret_val); 
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
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        }
    }
    virtual bool onMessage(const std::vector<uint8_t>& bytes)
    {
        // NOTE: This blocks the receiver until processing is complete
        const unsigned char* record = bytes.data();
        switch(bytes[0]) {
            case 'S':
                return onSystemEvent( itch::system_event(record) ); // system events
            case 'R':
                return onStockDirectory( itch::stock_directory(record)); // at beginning of day, status of stocks
            case 'H':
                return onStockTradingAction( itch::stock_trading_action(record) ); // trading status of a stock, assume H (halted)
            case 'Y':
                return onRegShoRestriction( itch::reg_sho_restriction(record) ); // Reg SHO toggle (off, on, still on)
            case 'L':
                return onMarketParticipantPosition( itch::market_participant_position(record) ); // info about market makers
            case 'V':
                return onMwcpDeclineLevel( itch::mwcp_decline_level(record) ); // what the marketwide circuit breaker is for the day
            case 'W':
                return onMwcpStatus( itch::mwcp_status(record) ); // used when an MWCB has been reached
            case 'K':
                return onIpoQuotingPeriodUpdate( itch::ipo_quoting_period_update(record) ); // IPO
            case 'J':
                return onLuldAuctionCollar( itch::luld_auction_collar(record) ); // Limit Up / Limit Down collar info
            case 'h':
                return onOperationalHalt( itch::operational_halt( record ) ); // operational halt
            case 'A':
                return onAddOrder( itch::add_order( record ) ); // order
            case 'F':
                return onAddOrderWithMpid( itch::add_order_with_mpid(record) ); // order with MPID
            case 'E':
                return onOrderExecuted( itch::order_executed(record) ); // order executed
            case 'C':
                return onOrderExecutedWithPrice( itch::order_executed_with_price(record) ); // price differs from original order
            case 'X':
                return onOrderCancel( itch::order_cancel(record) ); // order cancelled
            case 'D':
                return onOrderDelete( itch::order_delete(record) ); // order deleted
            case 'U':
                return onOrderReplace( itch::order_replace(record) ); // order replaced
            case 'P':
                return onTrade( itch::trade(record) ); // a trade happened with no maching order (order was non displayable)
            case 'Q':
                return onCrossTrade( itch::cross_trade(record) ); // a trade happened, not sure when this gets sent out
            case 'B':
                return onBrokenTrade( itch::broken_trade(record) ); // broken trade
            case 'I':
                return onNoii( itch::noii(record) ); // Net Order Imbalance Indicator
            case 'N': 
                return onRpii( itch::rpii(record) ); // Retail Price Improvement Indicator
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

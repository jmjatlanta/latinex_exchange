#pragma once

#include "soup_bin_connection.h"
#include "itch.h"
#include "logger.h"
#include <thread>
#include <vector>
#include <stdexcept>

/****
 * Reads ITCH over SoupBinTCP
 */

class SoupItchFeedClient : public SoupBinConnection
{
    public:

    SoupItchFeedClient(const std::string url) 
            : SoupBinConnection(url, "username", "password"), 
            logger(latinex::Logger::getInstance())
    {
    }

    virtual ~SoupItchFeedClient()
    {
    }

    virtual void on_sequenced_data(const soupbintcp::sequenced_data& in) override
    {
        onMessage(in.get_message());
    }

    virtual bool is_connected() const { return status == Status::CONNECTED; }
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

    latinex::Logger* logger;
};

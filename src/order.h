#pragma once

#include <book/depth_order_book.h>
#include <string>
#include <vector>
#include <memory>

class LatinexSessionServer;

namespace latinex
{

class Order
{
public:
    enum class State {
        Submitted,
        Rejected,
        Accepted,
        ModifyRequested,
        ModifyRejected,
        Modified,
        PartiallyFilled,
        Filled,
        CancelRequested,
        CancelRejected,
        Cancelled,
        Unknown
    };

    struct StateChange
    {
        State state_;
        std::string description_;
        StateChange() : state_(State::Unknown) {}
        StateChange(State state, const std::string& desc = "") : state_(state), description_(desc) {}
    };

    /***
     * @param id a unique identifier
     * @param buy_side true for buy, false for sell
     * @param quantity the number of shares
     * @param symbol the ticker symbol
     * @param price the desired price
     * @param stop_Price the desired stop price (not used)
     * @param aon true if order is all or nothing
     * @param ioc true if immediate or cancel
     */
    Order(const std::string& id, bool buy_side, liquibook::book::Quantity quantity, const std::string& symbol,
            liquibook::book::Price price, liquibook::book::Price stopPrice, bool aon, bool ioc);

    // getters/setters

    bool is_limit() const;
    bool is_buy() const;
    liquibook::book::Price price() const;
    liquibook::book::Price stop_price() const;
    liquibook::book::Quantity order_qty() const;
    virtual bool all_or_none() const;
    virtual bool immediate_or_cancel() const;
    std::string symbol() const;
    std::string order_id() const;
    void set_order_id(const std::string& in);
    void set_fix_server(LatinexSessionServer* server);
    uint32_t quantity_filled() const;
    uint32_t quantity_remaining() const;
    uint32_t fill_cost() const;
    const std::vector<StateChange>& history() const;
    const StateChange& current_state() const;

    // order lifecycle events
    virtual void on_submitted();
    virtual void on_accepted();
    virtual void on_rejected(const char* reason);
    virtual void on_filled(liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost);
    virtual void on_cancel_requested();
    virtual void on_cancelled();
    virtual void on_cancel_rejected(const char* reason);
    virtual void on_replace_requested(const int32_t& size_delta, liquibook::book::Price new_price);
    virtual void on_replaced(const int32_t& size_delta, liquibook::book::Price new_price);
    virtual void on_replace_rejected(const char* reason);

private:
    std::string id_;
    bool buy_side_;
    std::string symbol_;
    liquibook::book::Quantity quantity_;
    liquibook::book::Price price_;
    liquibook::book::Price stop_price_;
    bool aon_;
    bool ioc_;
    liquibook::book::Quantity quantity_filled_;
    int32_t quantity_remaining_;
    uint32_t fill_cost_;

    std::vector<StateChange> history_;
    LatinexSessionServer *server_ = nullptr;
};

} // namespace latinex


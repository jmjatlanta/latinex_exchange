#pragma once

#include <string>
#include <vector>

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
        StateChange() : state_(Unknown) {}
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
    uint32_t quantityFilled() const;
    uint32_t quantityRemaining() const;
    uint32_t fillCost() const;
    const std::vector<StateChange>& history() const;
    const StateChange& currentState() const;

    // order lifecycle events
    void onSubmitted();
    void onAccepted();
    void onRejected(const char* reason);
    void onFilled(liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost);
    void onCancelRequested();
    void onCancelled();
    void onCancelRejected(const char* reason);
    void onReplaceRequested(const int32_t& size_delta, liquibook::book::Price new_price);

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
    int32_t quantity_remaining_
    uint32_t fill_cost_;

    std::vector<StateChange> history_;
};

} // namespace latinex


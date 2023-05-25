#pragma once

#include "server.h"
#include <fix8/f8includes.hpp>
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"
#include <book/depth_order_book.h>
#include <string>
#include <vector>
#include <memory>

class LatinexSessionServer;

namespace latinex
{

class Order : public FIX8::TEX::NewOrderSingle
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

    Order();
    /**
     * Create order from FIX NewOrderSingle
     */
    Order(const FIX8::TEX::NewOrderSingle& orig);

    ~Order();

    // getters
    std::string symbol() const;
    liquibook::book::Price price() const;
    liquibook::book::Price stop_price() const;
    liquibook::book::Quantity order_qty() const;
    liquibook::book::Quantity leaves_qty() const;
    bool is_buy() const;
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
    virtual void on_fix_server_changed(LatinexSessionServer* server);

private:
    liquibook::book::Quantity quantity_;
    liquibook::book::Quantity leaves_qty_;
    std::vector<StateChange> history_;
    LatinexSessionServer *server_ = nullptr;
};

} // namespace latinex


#pragma once

#include "server.h"
#include "logger.h"
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

    /**
     * Create order from FIX NewOrderSingle
     */
    Order(const FIX8::TEX::NewOrderSingle& orig, uint64_t orderId);

    ~Order();

    // getters
    std::string symbol() const;
    liquibook::book::Price price() const;
    liquibook::book::Price stop_price() const;
    liquibook::book::Quantity order_qty() const;
    liquibook::book::Quantity leaves_qty() const;
    std::string order_id() const;
    std::string cl_ord_id() const;
    std::string secondary_cl_ord_id() const;
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

protected:
    Order();
private:
    FIX8::TEX::ExecutionReport* build_execution_report();
private:
    std::string orderId; // assigned by the exchange
    std::string clOrdId; // assigned by the broker
    std::string secondaryClOrdId; // assigned by broker's client
    liquibook::book::Quantity quantity_ = 0;
    liquibook::book::Quantity leaves_qty_ = 0;
    std::vector<StateChange> history_;
    LatinexSessionServer *server_ = nullptr;
    Logger* logger = nullptr;
};

} // namespace latinex


#include <gtest/gtest.h>

#include "../src/market.h"
#include "../src/order.h"
#include "../src/latinex_config.h"
#include <memory>

template<typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

class MyOrder : public latinex::Order
{
    public:

    enum class Func {
        Submitted,
        Accepted,
        Rejected,
        Filled,
        CancelRequested,
        Cancelled,
        CancelRejected,
        ReplaceRequested,
        Replaced,
        ReplaceRejected,
    };

    MyOrder() : Order()
    {
    }
    virtual void on_submitted() { callCount[to_underlying(Func::Submitted)]++; }
    virtual void on_accepted() { callCount[to_underlying(Func::Accepted)]++; }
    virtual void on_rejected(const char* reason) { callCount[to_underlying(Func::Rejected)]++; }
    virtual void on_filled(liquibook::book::Quantity fill_qty, liquibook::book::Cost fill_cost)
            { callCount[to_underlying(Func::Filled)]++; }
    virtual void on_cancel_requested() { callCount[to_underlying(Func::CancelRequested)]++; }
    virtual void on_cancelled() { callCount[to_underlying(Func::Cancelled)]++; }
    virtual void on_cancel_rejected(const char* reason) { callCount[to_underlying(Func::CancelRejected)]++; }
    virtual void on_replace_requested(const int32_t& size_delta, liquibook::book::Price new_price)
            { callCount[to_underlying(Func::ReplaceRequested)]++; }
    virtual void on_replaced(const int32_t& size_delta, liquibook::book::Price new_price)
            { callCount[to_underlying(Func::Replaced)]++; }
    virtual void on_replace_rejected(const char* reason) { callCount[to_underlying(Func::ReplaceRejected)]++; }

    std::map<int, int> callCount;

};

std::shared_ptr<MyOrder> make_order(const std::string& symbol, bool buy, uint32_t price = 0, uint32_t qty = 100)
{
    auto ord = std::make_shared<MyOrder>();
    *(ord) << new FIX8::TEX::Symbol(symbol)
        << new FIX8::TEX::Side( buy ? FIX8::TEX::Side_BUY : FIX8::TEX::Side_SELL)
        << new FIX8::TEX::Price(to_long_double(price))
        << new FIX8::TEX::QtyType(FIX8::TEX::QtyType_UNITS)
        << new FIX8::TEX::OrderQty(qty)
        ;
    return ord;
}

TEST(exchange, market_tests)
{
    latinex::Market<MyOrder> market;

    std::shared_ptr<MyOrder> order = make_order("ABC", true, 10000);
    EXPECT_FALSE(market.add_order(order));
    market.add_books_as_needed(true);
    market.add_order(order);
    EXPECT_EQ(order->callCount[to_underlying(MyOrder::Func::Submitted)], 1);
    EXPECT_EQ(order->callCount[to_underlying(MyOrder::Func::Accepted)], 1);
    EXPECT_EQ(order->callCount[to_underlying(MyOrder::Func::Filled)], 0);
    // match
    auto match = make_order("ABC", false, 10000);
    EXPECT_TRUE( market.add_order(match) );
    EXPECT_EQ(order->callCount[to_underlying(MyOrder::Func::Submitted)], 1);
    EXPECT_EQ(order->callCount[to_underlying(MyOrder::Func::Accepted)], 1);
    EXPECT_EQ(order->callCount[to_underlying(MyOrder::Func::Filled)], 1);
    EXPECT_EQ(match->callCount[to_underlying(MyOrder::Func::Submitted)], 1);
    EXPECT_EQ(match->callCount[to_underlying(MyOrder::Func::Accepted)], 1);
    EXPECT_EQ(match->callCount[to_underlying(MyOrder::Func::Filled)], 1);
}


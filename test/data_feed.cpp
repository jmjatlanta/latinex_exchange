#include<gtest/gtest.h>

#include "../src/data_feed.h"
#include <string>

class MockOrder
{
    public:
    bool is_buy() { return false; }
    uint32_t order_qty() { return 0; }
    uint32_t price() { return 0; }
    std::string symbol() { return ""; }
    std::string order_id() { return "1"; }
};

TEST(DataFeed, midnight)
{
    DataFeed<MockOrder> df;

    auto test1 = df.ns_since_midnight();
    auto test2 = df.ns_since_midnight();
    EXPECT_LT(test1, test2);
}

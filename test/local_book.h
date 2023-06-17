#pragma once

#include <map>
#include <vector>
#include <mutex>

/****
 * A local in-memory order book
 * add / remove bids and asks, report on changes to BBO
 */

template<class Price>
class BBOListener
{
    public:
    virtual void on_new_best_bid(const std::string& symbol, Price in) = 0;
    virtual void on_new_best_ask(const std::string& symbol, Price in) = 0;
};

template<class Price>
class TickListener
{
    public:
    virtual void on_tick(const std::string& symbol, Price in) = 0;
};

/****
 * This book does not do any matching. Bid and Ask values can cross. It is up
 * to external forces to correct such a (hopefully temporary) situation
 */
template<class Price, class Quantity>
class LocalBook
{
    public:
    LocalBook(const std::string& symbol) : symbol(symbol) {}

    bool add_bbo_listener(BBOListener<Price>* in) 
    { 
        std::scoped_lock lock(bbo_listeners_mutex);
        bboListeners.emplace_back(in); 
        return true; 
    }
    bool remove_bbo_listener(BBOListener<Price>* in) 
    { 
        std::scoped_lock lock(bbo_listeners_mutex);
        for(auto it = bboListeners.begin(); it != bboListeners.end(); ++it)
        {
            if ( (*it) == in )
            {
                bboListeners.erase(it);
                return true;
            }
        }
        return false;
    }
    bool add_tick_listener(TickListener<Price>* in)
    {
        std::scoped_lock lock(tick_listeners_mutex);
        tickListeners.emplace_back(in);;
        return true;
    }
    bool remove_tick_listener(TickListener<Price>* in)
    {
        std::scoped_lock lock(tick_listeners_mutex);
        for(auto it = tickListeners.begin(); it != tickListeners.end(); ++it)
        {
            if ( (*it) == in)
            {
                tickListeners.erase(it);
                return true;
            }
        }
        return false;
    }

    bool add_bid(Price p, Quantity q)
    {
        std::scoped_lock lock(bid_mutex);
        auto it = bids.find(p);
        if (it == bids.end()) 
        {
            auto it2 = bids.emplace(p, q);
            if (it2.second && it2.first == bids.begin())
                notify_bbo_listeners(true, p);
        }
        else
        {
            // simply add quantity
            (*it).second += q;
        }
        return true;
    }

    bool remove_bid(Price p, Quantity q)
    {
        std::scoped_lock lock(bid_mutex);
        auto it = bids.find(p);
        if (it == bids.end())
            return false;
        if ( (*it).second < q )
            (*it).second = 0;
        else
            (*it).second -= q;
        // if it is zero, remove it and possibly alert to new best (lower) bid
        if ((*it).second == 0)
        {
            if (it == bids.begin())
            {
                bids.erase(it);
                it = bids.begin();
                notify_bbo_listeners(true, (*it).first);
            }
            else
                bids.erase(it);
        }
        return true;
    }

    bool add_ask(Price p, Quantity q)
    {
        std::scoped_lock lock(ask_mutex);
        auto it = asks.find(p);
        if (it == asks.end()) 
        {
            auto it2 = asks.emplace(p, q);
            if (it2.second && it2.first == asks.begin())
                notify_bbo_listeners(false, p);
        }
        else
        {
            // simply add quantity
            (*it).second += q;
        }
        return true;
    }

    bool remove_ask(Price p, Quantity q)
    {
        std::scoped_lock lock(ask_mutex);
        auto it = asks.find(p);
        if (it == asks.end())
            return false;
        if ( (*it).second < q )
            (*it).second = 0;
        else
            (*it).second -= q;
        // if it is zero, remove it and possibly alert to new best (lower) bid
        if ((*it).second == 0)
        {
            if (it == asks.begin())
            {
                asks.erase(it);
                it = asks.begin();
                notify_bbo_listeners(false, (*it).first);
            }
            else
                asks.erase(it);
        }
        return true;
    }

    private:

    void notify_bbo_listeners(bool new_bid, Price new_price)
    {
        std::scoped_lock lock(bbo_listeners_mutex);
        if (new_bid)
        {
            for (auto l : bboListeners)
                l->on_new_best_bid(symbol, new_price);
        } else {
            for (auto l : bboListeners)
                l->on_new_best_ask(symbol, new_price);
        }

    }

    std::string symbol;
    std::map<Price, Quantity, std::greater<Price> > bids; // highest bid on top
    std::mutex bid_mutex;
    std::map<Price, Quantity> asks;
    std::mutex ask_mutex;
    std::vector<BBOListener<Price>*> bboListeners;
    std::mutex bbo_listeners_mutex;
    std::vector<TickListener<Price>*> tickListeners;
    std::mutex tick_listeners_mutex;
};


#pragma once

#include <fix8/f8includes.hpp>
#include "Myfix_router.hpp" // NOTE: must include f8includes first

#include "order.h"
#include "market.h"

#include <vector>
#include <memory>

/****
 * A FIX8 server and its classes
 */

class LatinexSessionServer; // declared further down
namespace latinex { class Order; }
/***
 * @brief A message router
 * @note Myfix_router, as it is in the TEX namespace, probably means it was generated
 */
class TexRouterServer : public FIX8::TEX::Myfix_Router 
{
    LatinexSessionServer& session_;

public:
    TexRouterServer(LatinexSessionServer& session) : session_(session) {}

    // NewOrderSingle message handler
    virtual bool operator()(const FIX8::TEX::NewOrderSingle* msg) const;
    virtual bool operator()(const FIX8::TEX::Logout* msg) const;
};

class LatinexSessionServer : public FIX8::Session
{
    TexRouterServer router_;
public:
    LatinexSessionServer(const FIX8::F8MetaCntx& ctx, const FIX8::sender_comp_id& sci,
            FIX8::Persister *persist = nullptr, FIX8::Logger *logger = nullptr, FIX8::Logger *plogger = nullptr) 
            : Session(ctx, sci, persist, logger, plogger), router_(*this) {}
    ~LatinexSessionServer();

    /***
     * @brief called by the framework when an application message has been received and decoded
     * @param seqnum
     * @param msg
     */
    bool handle_application(const unsigned seqnum, const FIX8::Message *&msg) override;

    /***
     * @brief example scheduler callback function
     * @return true if ok
     */
    bool sample_scheduler_callback();

    /****
     * @brief called whenever a session state changes
     * @param before the previous state
     * @param after the current state
     */
    void state_change(const FIX8::States::SessionStates before, const FIX8::States::SessionStates after);

    void subscribe_to_fix_events(latinex::Order* in)
    {
        subscribed_orders_.emplace_back(in);
    }

    void unsubscribe_from_fix_events(latinex::Order* in)
    {
        for(auto itr = subscribed_orders_.begin(); itr != subscribed_orders_.end(); ++itr)
        {
            if ( (*itr) == in )
            {
                subscribed_orders_.erase(itr);
                break;
            }
        }
    }

    std::string next_exec_id()
    { 
        return  exec_id_counter_ == nullptr ? "Blah" : std::to_string(++(*exec_id_counter_)); 
    }

    std::shared_ptr<std::atomic<uint64_t>> exec_id_counter_ = nullptr;
    std::shared_ptr<latinex::Market<latinex::Order>> market_ = nullptr;
    std::vector<latinex::Order*> subscribed_orders_;
};


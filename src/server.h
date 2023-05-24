#pragma once

#include "market.h"

#include <fix8/f8includes.hpp>
#include "Myfix_router.hpp" // NOTE: must include f8includes first

extern std::shared_ptr<latinex::Market> market;

/****
 * A FIX8 server and its classes
 */

class LatinexSessionServer; // declared further down

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
};

class LatinexSessionServer : public FIX8::Session
{
    TexRouterServer router_;
public:
    LatinexSessionServer(const FIX8::F8MetaCntx& ctx, const FIX8::sender_comp_id& sci,
            FIX8::Persister *persist = nullptr, FIX8::Logger *logger = nullptr, FIX8::Logger *plogger = nullptr) 
            : Session(ctx, sci, persist, logger, plogger), router_(*this) {}

    /***
     * @brief called by the framework when an application message has been received and decoded
     * @param seqnum
     * @param msg
     */
    bool handle_application(const unsigned seqnum, const FIX8::Message *&msg);

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
};

void start_market();
void stop_market();
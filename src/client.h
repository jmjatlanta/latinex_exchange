#pragma once

#include <fix8/f8includes.hpp>
#include "Myfix_router.hpp" // NOTE: must include f8includes first

class LatinexSessionClient;

class tex_router_client : public FIX8::TEX::Myfix_Router
{
    LatinexSessionClient& session_;
public:
    tex_router_client(LatinexSessionClient& session) : session_(session) {}

    virtual bool operator() (const FIX8::TEX::ExecutionReport *msg) const;
};

class LatinexSessionClient : public FIX8::Session
{
    tex_router_client router_;
public:
    LatinexSessionClient(const FIX8::F8MetaCntx& ctx, const FIX8::SessionID& sid, FIX8::Persister *persist = nullptr,
            FIX8::Logger *logger = nullptr, FIX8::Logger *plogger = nullptr) 
            : Session(ctx, sid, persist, logger, plogger), router_(*this) {}
    bool handle_application(const unsigned seqnum, const FIX8::Message *&msg) override;
    void state_change(const FIX8::States::SessionStates before, const FIX8::States::SessionStates after) override;
};

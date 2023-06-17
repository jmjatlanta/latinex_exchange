#include "client.h"
#include "helper.h"
#include "Myfix_types.hpp"
#include "Myfix_classes.hpp"


bool LatinexSessionClient::handle_application(const unsigned seqnum, const FIX8::Message *&msg)
{
    std::cout << "LatinexSessionClient::handle_application seqnum: " << seqnum << " with a message\n";
    return enforce(seqnum, msg) || msg->process(router_);
}

void LatinexSessionClient::state_change(const FIX8::States::SessionStates before, const FIX8::States::SessionStates after)
{
    // TODO: Do something inteligent
    std::cout << "LatinexSessionClient::state_change called. Old status: [" << to_string(before)
        << "] New status: [" << to_string(after) << "]\n";
    if (to_string(after) == "st_continuous")
        is_logged_in_ = true;
    else
        is_logged_in_ = false;
}

bool tex_router_client::operator() (const FIX8::TEX::ExecutionReport *msg) const
{
    std::cout << "tex_router_client: Received execution report\n";
    /*
    FIX8::TEX::LastCapacity lastCap;
    if (msg->get(lastCap))
    {
        // TODO: do something
    }
    */
    return true;
}

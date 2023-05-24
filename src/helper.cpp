#include "helper.h"

std::string to_string(FIX8::States::SessionStates in)
{
    switch(in)
    {
        case FIX8::States::SessionStates::st_none:
            return "st_none";
        case FIX8::States::SessionStates::st_continuous:
            return "st_continuous";
        case FIX8::States::SessionStates::st_session_terminated:
            return "st_session_terminated";
        case FIX8::States::SessionStates::st_wait_for_logon:
            return "st_wait_for_logon";
        case FIX8::States::SessionStates::st_not_logged_in:
            return "st_not_logged_in";
        case FIX8::States::SessionStates::st_logon_sent:
            return "st_logon_sent";
        case FIX8::States::SessionStates::st_logon_received:
            return "st_logon_received";
        case FIX8::States::SessionStates::st_logoff_sent:
            return "st_logoff_sent";
        case FIX8::States::SessionStates::st_logoff_received:
            return "st_logoff_received";
        case FIX8::States::SessionStates::st_test_request_sent:
            return "st_test_request_sent";
        case FIX8::States::SessionStates::st_sequence_reset_sent:
            return "st_sequence_reset_sent";
        case FIX8::States::SessionStates::st_sequence_reset_received:
            return "st_sequence_reset_received";
        case FIX8::States::SessionStates::st_resend_request_sent:
            return "st_resend_request_sent";
        case FIX8::States::SessionStates::st_resend_request_received:
            return "st_resend_request_received";
        case FIX8::States::SessionStates::st_num_states:
            return "st_num_states";
        default:
            return "st_unknown";
    }
}


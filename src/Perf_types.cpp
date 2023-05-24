//-------------------------------------------------------------------------------------------------
// *** f8c generated file: DO NOT EDIT! Created: 2023-05-21 18:57:58 ***
//-------------------------------------------------------------------------------------------------
/*

Fix8 is released under the GNU LESSER GENERAL PUBLIC LICENSE Version 3.

Fix8 Open Source FIX Engine.
Copyright (C) 2010-23 David L. Dight <fix@fix8.org>

Fix8 is free software: you can  redistribute it and / or modify  it under the  terms of the
GNU Lesser General  Public License as  published  by the Free  Software Foundation,  either
version 3 of the License, or (at your option) any later version.

Fix8 is distributed in the hope  that it will be useful, but WITHOUT ANY WARRANTY;  without
even the  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

You should  have received a copy of the GNU Lesser General Public  License along with Fix8.
If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************************
*                Special note for Fix8 compiler generated source code                     *
*                                                                                         *
* Binary works  that are the results of compilation of code that is generated by the Fix8 *
* compiler  can be released  without releasing your  source code as  long as your  binary *
* links dynamically  against an  unmodified version of the Fix8 library.  You are however *
* required to leave the copyright text in the generated code.                             *
*                                                                                         *
*******************************************************************************************

BECAUSE THE PROGRAM IS  LICENSED FREE OF  CHARGE, THERE IS NO  WARRANTY FOR THE PROGRAM, TO
THE EXTENT  PERMITTED  BY  APPLICABLE  LAW.  EXCEPT WHEN  OTHERWISE  STATED IN  WRITING THE
COPYRIGHT HOLDERS AND/OR OTHER PARTIES  PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED   OR   IMPLIED,  INCLUDING,  BUT   NOT  LIMITED   TO,  THE  IMPLIED
WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO
THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE,
YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

IN NO EVENT UNLESS REQUIRED  BY APPLICABLE LAW  OR AGREED TO IN  WRITING WILL ANY COPYRIGHT
HOLDER, OR  ANY OTHER PARTY  WHO MAY MODIFY  AND/OR REDISTRIBUTE  THE PROGRAM AS  PERMITTED
ABOVE,  BE  LIABLE  TO  YOU  FOR  DAMAGES,  INCLUDING  ANY  GENERAL, SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT
NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR
THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH
HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

*/

//-------------------------------------------------------------------------------------------------
#include <fix8/f8config.h>
#if defined FIX8_MAGIC_NUM && FIX8_MAGIC_NUM > 16793603L
#error Perf_types.cpp version 1.4.3 is out of date. Please regenerate with f8c.
#endif
//-------------------------------------------------------------------------------------------------
// Perf_types.cpp
//-------------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <iterator>
#include <algorithm>
#include <cerrno>
#include <string.h>
// f8 includes
#include <fix8/f8exception.hpp>
#include <fix8/hypersleep.hpp>
#include <fix8/mpmc.hpp>
#include <fix8/thread.hpp>
#include <fix8/f8types.hpp>
#include <fix8/f8utils.hpp>
#include <fix8/tickval.hpp>
#include <fix8/logger.hpp>
#include <fix8/traits.hpp>
#include <fix8/field.hpp>
#include <fix8/message.hpp>
#include "Perf_types.hpp"
//-------------------------------------------------------------------------------------------------
namespace FIX8 {
namespace TEX {

namespace {

//-------------------------------------------------------------------------------------------------
const f8String ExecInst_realm[]  
   { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "I", "L", "M", "N", "O", "P", "R", "S", "T", "U", "V", "W" };
const char *ExecInst_descriptions[]  
   { "STAY_ON_OFFERSIDE", "NOT_HELD", "WORK", "GO_ALONG", "OVER_THE_DAY", "HELD", "PARTICIPATE_DONT_INITIATE", "STRICT_SCALE", "TRY_TO_SCALE", "STAY_ON_BIDSIDE", "NO_CROSS", "OK_TO_CROSS", "CALL_FIRST", "PERCENT_OF_VOLUME", "DO_NOT_INCREASE", "DO_NOT_REDUCE", "ALL_OR_NONE", "INSTITUTIONS_ONLY", "LAST_PEG", "MID_PRICE_PEG", "NON_NEGOTIABLE", "OPENING_PEG", "MARKET_PEG", "PRIMARY_PEG", "SUSPEND", "FIXED_PEG_TO_LOCAL_BEST_BID_OR_OFFER_AT_TIME_OF_ORDER", "CUSTOMER_DISPLAY_INSTRUCTION", "NETTING", "PEG_TO_VWAP" };
const char ExecTransType_realm[]  
   { '0', '1', '2', '3' };
const char *ExecTransType_descriptions[]  
   { "NEW", "CANCEL", "CORRECT", "STATUS" };
const char HandlInst_realm[]  
   { '1', '2', '3' };
const char *HandlInst_descriptions[]  
   { "AUTOMATED_EXECUTION_ORDER_PRIVATE_NO_BROKER_INTERVENTION", "AUTOMATED_EXECUTION_ORDER_PUBLIC_BROKER_INTERVENTION_OK", "MANUAL_ORDER_BEST_EXECUTION" };
const f8String IDSource_realm[]  
   { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
const char *IDSource_descriptions[]  
   { "CUSIP", "SEDOL", "QUIK", "ISIN_NUMBER", "RIC_CODE", "ISO_CURRENCY_CODE", "ISO_COUNTRY_CODE", "EXCHANGE_SYMBOL", "CONSOLIDATED_TAPE_ASSOCIATION" };
const char LastCapacity_realm[]  
   { '1', '2', '3', '4' };
const char *LastCapacity_descriptions[]  
   { "AGENT", "CROSS_AS_AGENT", "CROSS_AS_PRINCIPAL", "PRINCIPAL" };
const f8String MsgType_realm[]  
   { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m" };
const char *MsgType_descriptions[]  
   { "HEARTBEAT", "TEST_REQUEST", "RESEND_REQUEST", "REJECT", "SEQUENCE_RESET", "LOGOUT", "INDICATION_OF_INTEREST", "ADVERTISEMENT", "EXECUTION_REPORT", "ORDER_CANCEL_REJECT", "LOGON", "NEWS", "EMAIL", "ORDER_SINGLE", "ORDER_LIST", "ORDER_CANCEL_REQUEST", "ORDER_CANCEL_REPLACE_REQUEST", "ORDER_STATUS_REQUEST", "ALLOCATION", "LIST_CANCEL_REQUEST", "LIST_EXECUTE", "LIST_STATUS_REQUEST", "LIST_STATUS", "ALLOCATION_ACK", "DONT_KNOW_TRADE", "QUOTE_REQUEST", "QUOTE", "SETTLEMENT_INSTRUCTIONS", "MARKET_DATA_REQUEST", "MARKET_DATA_SNAPSHOT_FULL_REFRESH", "MARKET_DATA_INCREMENTAL_REFRESH", "MARKET_DATA_REQUEST_REJECT", "QUOTE_CANCEL", "QUOTE_STATUS_REQUEST", "QUOTE_ACKNOWLEDGEMENT", "SECURITY_DEFINITION_REQUEST", "SECURITY_DEFINITION", "SECURITY_STATUS_REQUEST", "SECURITY_STATUS", "TRADING_SESSION_STATUS_REQUEST", "TRADING_SESSION_STATUS", "MASS_QUOTE", "BUSINESS_MESSAGE_REJECT", "BID_REQUEST", "BID_RESPONSE", "LIST_STRIKE_PRICE" };
const char OrdStatus_realm[]  
   { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E' };
const char *OrdStatus_descriptions[]  
   { "NEW", "PARTIALLY_FILLED", "FILLED", "DONE_FOR_DAY", "CANCELED", "REPLACED", "PENDING_CANCEL", "STOPPED", "REJECTED", "SUSPENDED", "PENDING_NEW", "CALCULATED", "EXPIRED", "ACCEPTED_FOR_BIDDING", "PENDING_REPLACE" };
const char OrdType_realm[]  
   { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'P' };
const char *OrdType_descriptions[]  
   { "MARKET", "LIMIT", "STOP", "STOP_LIMIT", "MARKET_ON_CLOSE", "WITH_OR_WITHOUT", "LIMIT_OR_BETTER", "LIMIT_WITH_OR_WITHOUT", "ON_BASIS", "ON_CLOSE", "LIMIT_ON_CLOSE", "FOREX_C", "PREVIOUSLY_QUOTED", "PREVIOUSLY_INDICATED", "FOREX_F", "FOREX_G", "FOREX_H", "FUNARI", "PEGGED" };
const char PossDupFlag_realm[]  
   { 'N', 'Y' };
const char *PossDupFlag_descriptions[]  
   { "NO", "YES" };
const char Side_realm[]  
   { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
const char *Side_descriptions[]  
   { "BUY", "SELL", "BUY_MINUS", "SELL_PLUS", "SELL_SHORT", "SELL_SHORT_EXEMPT", "UNDISCLOSED", "CROSS", "CROSS_SHORT" };
const char TimeInForce_realm[]  
   { '0', '1', '2', '3', '4', '5', '6' };
const char *TimeInForce_descriptions[]  
   { "DAY", "GOOD_TILL_CANCEL", "AT_THE_OPENING", "IMMEDIATE_OR_CANCEL", "FILL_OR_KILL", "GOOD_TILL_CROSSING", "GOOD_TILL_DATE" };
const char SettlmntTyp_realm[]  
   { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
const char *SettlmntTyp_descriptions[]  
   { "REGULAR", "CASH", "NEXT_DAY", "T_PLUS_2", "T_PLUS_3", "T_PLUS_4", "FUTURE", "WHEN_ISSUED", "SELLERS_OPTION", "T_PLUS_5" };
const char OpenClose_realm[]  
   { 'C', 'O' };
const char *OpenClose_descriptions[]  
   { "CLOSE", "OPEN" };
const char ProcessCode_realm[]  
   { '0', '1', '2', '3', '4', '5', '6' };
const char *ProcessCode_descriptions[]  
   { "REGULAR", "SOFT_DOLLAR", "STEP_IN", "STEP_OUT", "SOFT_DOLLAR_STEP_IN", "SOFT_DOLLAR_STEP_OUT", "PLAN_SPONSOR" };
const char PossResend_realm[]  
   { 'N', 'Y' };
const char *PossResend_descriptions[]  
   { "NO", "YES" };
const int EncryptMethod_realm[]  
   { 0, 1, 2, 3, 4, 5, 6 };
const char *EncryptMethod_descriptions[]  
   { "NONE", "PKCS", "DES", "PKCS_DES", "PGP_DES", "PGP_DES_MD5", "PEM_DES_MD5" };
const int OrdRejReason_realm[]  
   { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
const char *OrdRejReason_descriptions[]  
   { "BROKER_OPTION", "UNKNOWN_SYMBOL", "EXCHANGE_CLOSED", "ORDER_EXCEEDS_LIMIT", "TOO_LATE_TO_ENTER", "UNKNOWN_ORDER", "DUPLICATE_ORDER", "DUPLICATE_OF_A_VERBALLY_COMMUNICATED_ORDER", "STALE_ORDER" };
const char ReportToExch_realm[]  
   { 'N', 'Y' };
const char *ReportToExch_descriptions[]  
   { "NO", "YES" };
const char LocateReqd_realm[]  
   { 'N', 'Y' };
const char *LocateReqd_descriptions[]  
   { "NO", "YES" };
const char GapFillFlag_realm[]  
   { 'N', 'Y' };
const char *GapFillFlag_descriptions[]  
   { "NO", "YES" };
const char ResetSeqNumFlag_realm[]  
   { 'N', 'Y' };
const char *ResetSeqNumFlag_descriptions[]  
   { "NO", "YES" };
const char ExecType_realm[]  
   { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E' };
const char *ExecType_descriptions[]  
   { "NEW", "PARTIAL_FILL", "FILL", "DONE_FOR_DAY", "CANCELED", "REPLACE", "PENDING_CANCEL", "STOPPED", "REJECTED", "SUSPENDED", "PENDING_NEW", "CALCULATED", "EXPIRED", "RESTATED", "PENDING_REPLACE" };
const f8String SecurityType_realm[]  
   { "?", "BA", "CB", "CD", "CMO", "CORP", "CP", "CPP", "CS", "FHA", "FHL", "FN", "FOR", "FUT", "GN", "GOVT", "IET", "MF", "MIO", "MPO", "MPP", "MPT", "MUNI", "NONE", "OPT", "PS", "RP", "RVRP", "SL", "TD", "USTB", "WAR", "ZOO" };
const char *SecurityType_descriptions[]  
   { "WILDCARD_ENTRY", "BANKERS_ACCEPTANCE", "CONVERTIBLE_BOND", "CERTIFICATE_OF_DEPOSIT", "COLLATERALIZE_MORTGAGE_OBLIGATION", "CORPORATE_BOND", "COMMERCIAL_PAPER", "CORPORATE_PRIVATE_PLACEMENT", "COMMON_STOCK", "FEDERAL_HOUSING_AUTHORITY", "FEDERAL_HOME_LOAN", "FEDERAL_NATIONAL_MORTGAGE_ASSOCIATION", "FOREIGN_EXCHANGE_CONTRACT", "FUTURE", "GOVERNMENT_NATIONAL_MORTGAGE_ASSOCIATION", "TREASURIES_PLUS_AGENCY_DEBENTURE", "MORTGAGE_IOETTE", "MUTUAL_FUND", "MORTGAGE_INTEREST_ONLY", "MORTGAGE_PRINCIPAL_ONLY", "MORTGAGE_PRIVATE_PLACEMENT", "MISCELLANEOUS_PASS_THRU", "MUNICIPAL_BOND", "NO_ISITC_SECURITY_TYPE", "OPTION", "PREFERRED_STOCK", "REPURCHASE_AGREEMENT", "REVERSE_REPURCHASE_AGREEMENT", "STUDENT_LOAN_MARKETING_ASSOCIATION", "TIME_DEPOSIT", "US_TREASURY_BILL", "WARRANT", "CATS_TIGERS_LIONS" };
const int PutOrCall_realm[]  
   { 0, 1 };
const char *PutOrCall_descriptions[]  
   { "PUT", "CALL" };
const int CustomerOrFirm_realm[]  
   { 0, 1 };
const char *CustomerOrFirm_descriptions[]  
   { "CUSTOMER", "FIRM" };
const int SessionRejectReason_realm[]  
   { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const char *SessionRejectReason_descriptions[]  
   { "INVALID_TAG_NUMBER", "REQUIRED_TAG_MISSING", "TAG_NOT_DEFINED_FOR_THIS_MESSAGE_TYPE", "UNDEFINED_TAG", "TAG_SPECIFIED_WITHOUT_A_VALUE", "VALUE_IS_INCORRECT", "INCORRECT_DATA_FORMAT_FOR_VALUE", "DECRYPTION_PROBLEM", "SIGNATURE_PROBLEM", "COMPID_PROBLEM", "SENDINGTIME_ACCURACY_PROBLEM", "INVALID_MSGTYPE" };
const int ExecRestatementReason_realm[]  
   { 0, 1, 2, 3, 4, 5 };
const char *ExecRestatementReason_descriptions[]  
   { "GT_CORPORATE_ACTION", "GT_RENEWAL", "VERBAL_CHANGE", "REPRICING_OF_ORDER", "BROKER_OPTION", "PARTIAL_DECLINE_OF_ORDERQTY" };
const char MsgDirection_realm[]  
   { 'R', 'S' };
const char *MsgDirection_descriptions[]  
   { "RECEIVE", "SEND" };

//-------------------------------------------------------------------------------------------------
const RealmBase realmbases[] 
{
   { reinterpret_cast<const void *>(ExecInst_realm), RealmBase::dt_set, FieldTrait::ft_MultipleStringValue, 29, ExecInst_descriptions },
   { reinterpret_cast<const void *>(ExecTransType_realm), RealmBase::dt_set, FieldTrait::ft_char, 4, ExecTransType_descriptions },
   { reinterpret_cast<const void *>(HandlInst_realm), RealmBase::dt_set, FieldTrait::ft_char, 3, HandlInst_descriptions },
   { reinterpret_cast<const void *>(IDSource_realm), RealmBase::dt_set, FieldTrait::ft_string, 9, IDSource_descriptions },
   { reinterpret_cast<const void *>(LastCapacity_realm), RealmBase::dt_set, FieldTrait::ft_char, 4, LastCapacity_descriptions },
   { reinterpret_cast<const void *>(MsgType_realm), RealmBase::dt_set, FieldTrait::ft_string, 46, MsgType_descriptions },
   { reinterpret_cast<const void *>(OrdStatus_realm), RealmBase::dt_set, FieldTrait::ft_char, 15, OrdStatus_descriptions },
   { reinterpret_cast<const void *>(OrdType_realm), RealmBase::dt_set, FieldTrait::ft_char, 19, OrdType_descriptions },
   { reinterpret_cast<const void *>(PossDupFlag_realm), RealmBase::dt_set, FieldTrait::ft_Boolean, 2, PossDupFlag_descriptions },
   { reinterpret_cast<const void *>(Side_realm), RealmBase::dt_set, FieldTrait::ft_char, 9, Side_descriptions },
   { reinterpret_cast<const void *>(TimeInForce_realm), RealmBase::dt_set, FieldTrait::ft_char, 7, TimeInForce_descriptions },
   { reinterpret_cast<const void *>(SettlmntTyp_realm), RealmBase::dt_set, FieldTrait::ft_char, 10, SettlmntTyp_descriptions },
   { reinterpret_cast<const void *>(OpenClose_realm), RealmBase::dt_set, FieldTrait::ft_char, 2, OpenClose_descriptions },
   { reinterpret_cast<const void *>(ProcessCode_realm), RealmBase::dt_set, FieldTrait::ft_char, 7, ProcessCode_descriptions },
   { reinterpret_cast<const void *>(PossResend_realm), RealmBase::dt_set, FieldTrait::ft_Boolean, 2, PossResend_descriptions },
   { reinterpret_cast<const void *>(EncryptMethod_realm), RealmBase::dt_set, FieldTrait::ft_int, 7, EncryptMethod_descriptions },
   { reinterpret_cast<const void *>(OrdRejReason_realm), RealmBase::dt_set, FieldTrait::ft_int, 9, OrdRejReason_descriptions },
   { reinterpret_cast<const void *>(ReportToExch_realm), RealmBase::dt_set, FieldTrait::ft_Boolean, 2, ReportToExch_descriptions },
   { reinterpret_cast<const void *>(LocateReqd_realm), RealmBase::dt_set, FieldTrait::ft_Boolean, 2, LocateReqd_descriptions },
   { reinterpret_cast<const void *>(GapFillFlag_realm), RealmBase::dt_set, FieldTrait::ft_Boolean, 2, GapFillFlag_descriptions },
   { reinterpret_cast<const void *>(ResetSeqNumFlag_realm), RealmBase::dt_set, FieldTrait::ft_Boolean, 2, ResetSeqNumFlag_descriptions },
   { reinterpret_cast<const void *>(ExecType_realm), RealmBase::dt_set, FieldTrait::ft_char, 15, ExecType_descriptions },
   { reinterpret_cast<const void *>(SecurityType_realm), RealmBase::dt_set, FieldTrait::ft_string, 33, SecurityType_descriptions },
   { reinterpret_cast<const void *>(PutOrCall_realm), RealmBase::dt_set, FieldTrait::ft_int, 2, PutOrCall_descriptions },
   { reinterpret_cast<const void *>(CustomerOrFirm_realm), RealmBase::dt_set, FieldTrait::ft_int, 2, CustomerOrFirm_descriptions },
   { reinterpret_cast<const void *>(SessionRejectReason_realm), RealmBase::dt_set, FieldTrait::ft_int, 12, SessionRejectReason_descriptions },
   { reinterpret_cast<const void *>(ExecRestatementReason_realm), RealmBase::dt_set, FieldTrait::ft_int, 6, ExecRestatementReason_descriptions },
   { reinterpret_cast<const void *>(MsgDirection_realm), RealmBase::dt_set, FieldTrait::ft_char, 2, MsgDirection_descriptions },
};

//-------------------------------------------------------------------------------------------------

} // namespace

//-------------------------------------------------------------------------------------------------
extern const Perf_BaseEntry::Pair fldpairs[];
const Perf_BaseEntry::Pair fldpairs[] 
{
   { 1, { Type2Type<TEX::Account>(), "Account", 1 } },
   { 6, { Type2Type<TEX::AvgPx>(), "AvgPx", 6 } },
   { 7, { Type2Type<TEX::BeginSeqNo>(), "BeginSeqNo", 7 } },
   { 8, { Type2Type<TEX::BeginString>(), "BeginString", 8 } },
   { 9, { Type2Type<TEX::BodyLength>(), "BodyLength", 9 } },
   { 10, { Type2Type<TEX::CheckSum>(), "CheckSum", 10 } },
   { 11, { Type2Type<TEX::ClOrdID>(), "ClOrdID", 11 } },
   { 12, { Type2Type<TEX::Commission>(), "Commission", 12 } },
   { 14, { Type2Type<TEX::CumQty>(), "CumQty", 14 } },
   { 15, { Type2Type<TEX::Currency>(), "Currency", 15 } },
   { 16, { Type2Type<TEX::EndSeqNo>(), "EndSeqNo", 16 } },
   { 17, { Type2Type<TEX::ExecID>(), "ExecID", 17 } },
   { 18, { Type2Type<TEX::ExecInst>(), "ExecInst", 18, &TEX::realmbases[0] } },
   { 19, { Type2Type<TEX::ExecRefID>(), "ExecRefID", 19 } },
   { 20, { Type2Type<TEX::ExecTransType>(), "ExecTransType", 20, &TEX::realmbases[1] } },
   { 21, { Type2Type<TEX::HandlInst>(), "HandlInst", 21, &TEX::realmbases[2] } },
   { 22, { Type2Type<TEX::IDSource>(), "IDSource", 22, &TEX::realmbases[3] } },
   { 29, { Type2Type<TEX::LastCapacity>(), "LastCapacity", 29, &TEX::realmbases[4], 
      "Last capacity" } },
   { 30, { Type2Type<TEX::LastMkt>(), "LastMkt", 30 } },
   { 31, { Type2Type<TEX::LastPx>(), "LastPx", 31, nullptr, 
      "Last price" } },
   { 32, { Type2Type<TEX::LastShares>(), "LastShares", 32 } },
   { 34, { Type2Type<TEX::MsgSeqNum>(), "MsgSeqNum", 34 } },
   { 35, { Type2Type<TEX::MsgType>(), "MsgType", 35, &TEX::realmbases[5] } },
   { 36, { Type2Type<TEX::NewSeqNo>(), "NewSeqNo", 36 } },
   { 37, { Type2Type<TEX::OrderID>(), "OrderID", 37 } },
   { 38, { Type2Type<TEX::OrderQty>(), "OrderQty", 38 } },
   { 39, { Type2Type<TEX::OrdStatus>(), "OrdStatus", 39, &TEX::realmbases[6] } },
   { 40, { Type2Type<TEX::OrdType>(), "OrdType", 40, &TEX::realmbases[7] } },
   { 41, { Type2Type<TEX::OrigClOrdID>(), "OrigClOrdID", 41 } },
   { 43, { Type2Type<TEX::PossDupFlag>(), "PossDupFlag", 43, &TEX::realmbases[8] } },
   { 44, { Type2Type<TEX::Price>(), "Price", 44 } },
   { 45, { Type2Type<TEX::RefSeqNum>(), "RefSeqNum", 45 } },
   { 48, { Type2Type<TEX::SecurityID>(), "SecurityID", 48 } },
   { 49, { Type2Type<TEX::SenderCompID>(), "SenderCompID", 49 } },
   { 52, { Type2Type<TEX::SendingTime>(), "SendingTime", 52 } },
   { 54, { Type2Type<TEX::Side>(), "Side", 54, &TEX::realmbases[9] } },
   { 55, { Type2Type<TEX::Symbol>(), "Symbol", 55 } },
   { 56, { Type2Type<TEX::TargetCompID>(), "TargetCompID", 56 } },
   { 58, { Type2Type<TEX::Text>(), "Text", 58 } },
   { 59, { Type2Type<TEX::TimeInForce>(), "TimeInForce", 59, &TEX::realmbases[10] } },
   { 60, { Type2Type<TEX::TransactTime>(), "TransactTime", 60 } },
   { 63, { Type2Type<TEX::SettlmntTyp>(), "SettlmntTyp", 63, &TEX::realmbases[11] } },
   { 64, { Type2Type<TEX::FutSettDate>(), "FutSettDate", 64 } },
   { 65, { Type2Type<TEX::SymbolSfx>(), "SymbolSfx", 65 } },
   { 66, { Type2Type<TEX::ListID>(), "ListID", 66 } },
   { 75, { Type2Type<TEX::TradeDate>(), "TradeDate", 75 } },
   { 76, { Type2Type<TEX::ExecBroker>(), "ExecBroker", 76 } },
   { 77, { Type2Type<TEX::OpenClose>(), "OpenClose", 77, &TEX::realmbases[12] } },
   { 81, { Type2Type<TEX::ProcessCode>(), "ProcessCode", 81, &TEX::realmbases[13] } },
   { 89, { Type2Type<TEX::Signature>(), "Signature", 89 } },
   { 90, { Type2Type<TEX::SecureDataLen>(), "SecureDataLen", 90 } },
   { 91, { Type2Type<TEX::SecureData>(), "SecureData", 91 } },
   { 93, { Type2Type<TEX::SignatureLength>(), "SignatureLength", 93 } },
   { 95, { Type2Type<TEX::RawDataLength>(), "RawDataLength", 95 } },
   { 96, { Type2Type<TEX::RawData>(), "RawData", 96 } },
   { 97, { Type2Type<TEX::PossResend>(), "PossResend", 97, &TEX::realmbases[14] } },
   { 98, { Type2Type<TEX::EncryptMethod>(), "EncryptMethod", 98, &TEX::realmbases[15] } },
   { 99, { Type2Type<TEX::StopPx>(), "StopPx", 99 } },
   { 100, { Type2Type<TEX::ExDestination>(), "ExDestination", 100 } },
   { 103, { Type2Type<TEX::OrdRejReason>(), "OrdRejReason", 103, &TEX::realmbases[16] } },
   { 106, { Type2Type<TEX::Issuer>(), "Issuer", 106 } },
   { 107, { Type2Type<TEX::SecurityDesc>(), "SecurityDesc", 107 } },
   { 108, { Type2Type<TEX::HeartBtInt>(), "HeartBtInt", 108 } },
   { 109, { Type2Type<TEX::ClientID>(), "ClientID", 109 } },
   { 110, { Type2Type<TEX::MinQty>(), "MinQty", 110 } },
   { 111, { Type2Type<TEX::MaxFloor>(), "MaxFloor", 111 } },
   { 112, { Type2Type<TEX::TestReqID>(), "TestReqID", 112 } },
   { 113, { Type2Type<TEX::ReportToExch>(), "ReportToExch", 113, &TEX::realmbases[17] } },
   { 114, { Type2Type<TEX::LocateReqd>(), "LocateReqd", 114, &TEX::realmbases[18] } },
   { 115, { Type2Type<TEX::OnBehalfOfCompID>(), "OnBehalfOfCompID", 115 } },
   { 117, { Type2Type<TEX::QuoteID>(), "QuoteID", 117 } },
   { 119, { Type2Type<TEX::SettlCurrAmt>(), "SettlCurrAmt", 119 } },
   { 120, { Type2Type<TEX::SettlCurrency>(), "SettlCurrency", 120 } },
   { 122, { Type2Type<TEX::OrigSendingTime>(), "OrigSendingTime", 122 } },
   { 123, { Type2Type<TEX::GapFillFlag>(), "GapFillFlag", 123, &TEX::realmbases[19] } },
   { 126, { Type2Type<TEX::ExpireTime>(), "ExpireTime", 126 } },
   { 128, { Type2Type<TEX::DeliverToCompID>(), "DeliverToCompID", 128 } },
   { 140, { Type2Type<TEX::PrevClosePx>(), "PrevClosePx", 140 } },
   { 141, { Type2Type<TEX::ResetSeqNumFlag>(), "ResetSeqNumFlag", 141, &TEX::realmbases[20] } },
   { 150, { Type2Type<TEX::ExecType>(), "ExecType", 150, &TEX::realmbases[21] } },
   { 151, { Type2Type<TEX::LeavesQty>(), "LeavesQty", 151 } },
   { 152, { Type2Type<TEX::CashOrderQty>(), "CashOrderQty", 152 } },
   { 167, { Type2Type<TEX::SecurityType>(), "SecurityType", 167, &TEX::realmbases[22] } },
   { 168, { Type2Type<TEX::EffectiveTime>(), "EffectiveTime", 168 } },
   { 198, { Type2Type<TEX::SecondaryOrderID>(), "SecondaryOrderID", 198 } },
   { 200, { Type2Type<TEX::MaturityMonthYear>(), "MaturityMonthYear", 200 } },
   { 201, { Type2Type<TEX::PutOrCall>(), "PutOrCall", 201, &TEX::realmbases[23] } },
   { 202, { Type2Type<TEX::StrikePrice>(), "StrikePrice", 202 } },
   { 204, { Type2Type<TEX::CustomerOrFirm>(), "CustomerOrFirm", 204, &TEX::realmbases[24] } },
   { 205, { Type2Type<TEX::MaturityDay>(), "MaturityDay", 205 } },
   { 206, { Type2Type<TEX::OptAttribute>(), "OptAttribute", 206 } },
   { 207, { Type2Type<TEX::SecurityExchange>(), "SecurityExchange", 207 } },
   { 210, { Type2Type<TEX::MaxShow>(), "MaxShow", 210 } },
   { 231, { Type2Type<TEX::ContractMultiplier>(), "ContractMultiplier", 231 } },
   { 336, { Type2Type<TEX::TradingSessionID>(), "TradingSessionID", 336 } },
   { 348, { Type2Type<TEX::EncodedIssuerLen>(), "EncodedIssuerLen", 348 } },
   { 349, { Type2Type<TEX::EncodedIssuer>(), "EncodedIssuer", 349 } },
   { 354, { Type2Type<TEX::EncodedTextLen>(), "EncodedTextLen", 354 } },
   { 355, { Type2Type<TEX::EncodedText>(), "EncodedText", 355 } },
   { 371, { Type2Type<TEX::RefTagID>(), "RefTagID", 371 } },
   { 372, { Type2Type<TEX::RefMsgType>(), "RefMsgType", 372 } },
   { 373, { Type2Type<TEX::SessionRejectReason>(), "SessionRejectReason", 373, &TEX::realmbases[25] } },
   { 376, { Type2Type<TEX::ComplianceID>(), "ComplianceID", 376 } },
   { 378, { Type2Type<TEX::ExecRestatementReason>(), "ExecRestatementReason", 378, &TEX::realmbases[26] } },
   { 383, { Type2Type<TEX::MaxMessageSize>(), "MaxMessageSize", 383 } },
   { 384, { Type2Type<TEX::NoMsgTypes>(), "NoMsgTypes", 384 } },
   { 385, { Type2Type<TEX::MsgDirection>(), "MsgDirection", 385, &TEX::realmbases[27] } },
   { 424, { Type2Type<TEX::DayOrderQty>(), "DayOrderQty", 424 } },
   { 425, { Type2Type<TEX::DayCumQty>(), "DayCumQty", 425 } },
   { 426, { Type2Type<TEX::DayAvgPx>(), "DayAvgPx", 426 } },
   { 432, { Type2Type<TEX::ExpireDate>(), "ExpireDate", 432 } }
}; // 111
} // namespace TEX

} // namespace FIX8

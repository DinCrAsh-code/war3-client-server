//============================================================================
//  0x6F5375B0 - CNetObserver::CallbackObserver's vtable slot 3.  See
//  netobserver.h for the field layout this reads.
//============================================================================
#include "netobserver.h"

int CNetObserver::CallbackObserver::Method_0x0C(const SAgentMessage* msg)
{
    return m_callback(msg, m_userdata);
}

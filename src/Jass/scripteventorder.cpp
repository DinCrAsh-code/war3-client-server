//============================================================================
//  0x6F26FD10 - CScriptEventData::GetOrder, the +0x38 handle ref resolved.
//
//  Own translation unit: GetOrderPointX/GetOrderPointY
//  (scripteventpoint.cpp) each issue a genuine `call` to it, and at eight
//  instructions with a tail jump it is exactly the size /Ob2 would inline
//  into both of them if they shared a file.
//============================================================================
#include "jassevents.h"

SEventOrder* CScriptEventData::GetOrder()
{
    if ((m_orderTypeTag & m_orderHandle) == -1)
        return 0;

    return (SEventOrder*)QueryHandleField0x54(
        (SOptionalHandleRef*)&m_orderHandle);
}

//============================================================================
//  0x6F2B5D00 - CItem::StoreAndNotifyHandle.  See item.h.
//
//  Own translation unit, separate from item.cpp (CItem::ClearPendingAndNotify, its
//  one caller): /O2 would otherwise inline this single-call-site function
//  straight into Method_0x80, which the shipped code does not do (a real,
//  out-of-line `call` at 0x6F2B6142).
//============================================================================
#include "item.h"
#include "stormnotifysingleton.h"

void CItem::StoreAndNotifyHandle(int value)
{
    m_position.m_notifyValue = (unsigned int)value;
    if (m_position.m_notifyHandle != (unsigned int)-1)
        NotifyStormSingleton(m_position.m_notifyHandle, value);
}

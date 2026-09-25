//============================================================================
//  0x6F474860 / 0x6F4748C0 / 0x6F4748F0 - three flag setters on the path
//  data behind a widget's handle sub-object, all reached from the two
//  footprint attach/detach slots (0x6F2AD0C0 and 0x6F2AD300).
//
//  All three resolve the handle the same way and then set or clear one bit
//  in a word at +0x34 of the path data.  Two details are worth recording:
//
//  * the first two reach it through the object's +0x94 and the third
//    through its +0x98 - two different sub-objects, not one;
//  * the third one's sense is **inverted** relative to the other two - a
//    true flag *clears* its bit - and it follows up with a call the other
//    two do not make.
//
//  All three are thiscall members: the flag is pushed and cleaned
//  (`retn 4`), never passed in edx.
//============================================================================
#include "widgetpathrefadjust.h"
#include "game.h"

//  0x6F4A7520 - below the depth cut; thunked.
void __fastcall NotifyPathDataChanged(void* obj);

void SPathRefHandle::SetPathFlagBlocking(int on)
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    unsigned int* flags =
        (unsigned int*)(*(char**)((char*)obj + 0x94) + 0x34);
    if (on)
        *flags |= 0x4000000;
    else
        *flags &= ~0x4000000;
}

void SPathRefHandle::SetPathFlagWalkable(int on)
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    unsigned int* flags =
        (unsigned int*)(*(char**)((char*)obj + 0x94) + 0x34);
    if (on)
        *flags |= 0x8000000;
    else
        *flags &= ~0x8000000;
}

//  Inverted, and with the extra notify - see the file header.
void SPathRefHandle::SetPathFlagVisible(int on)
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    unsigned int* flags =
        (unsigned int*)(*(char**)((char*)obj + 0x98) + 0x34);
    if (on)
        *flags &= ~0x1000000;
    else
        *flags |= 0x1000000;
    NotifyPathDataChanged(obj);
}

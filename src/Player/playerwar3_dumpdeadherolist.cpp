//============================================================================
//  0x6F40FF20 - CPlayerWar3 vtable slot 32 (+0x80).  Debug-dump the "dead
//  hero" list.
//
//  First flushes two dependent objects, each reached through a raw
//  indirect vtable dispatch (per CLAUDE.md's own house style for a
//  receiver whose real class this session could not pin down - no dump in
//  this batch names either object's constructor or vtable): the object at
//  +0x2D0 via its own vtable slot 1 (+0x04), then the object at +0x34 via
//  its own vtable slot 4 (+0x10), both one-argument `__thiscall void
//  Method(int)`-shaped calls passed the same `sink` this function itself
//  received.
//
//  Then prints "Dead hero list size: %d" and one line per element of a
//  pointer array at +0x2B4, looped over 0..m_intMiniCount (the identical
//  +0x2C0 count DumpState already established, playerwar3_dumpstate.cpp -
//  the same field, just read as a raw loop bound here instead of through
//  DumpTo()).  Per element: a null pointer prints "No dead hero %d";
//  otherwise "Dead hero %d: age[%d] tags[%u %u]" pulling the "age" out of
//  the identical +0x2C4 int array at the same index (again the same field
//  DumpState walks through SIntMiniValue::DumpTo - SIntMiniValue is a bare
//  `unsigned int` with no vtable, see textsink.h, so reading
//  m_intMiniArray[i].m_value directly here is the same four bytes) and the
//  two "tags" out of the pointee's own +0x0C/+0x10 fields.
//============================================================================
#include "playerwar3.h"
#include "storm.h"

int __cdecl Storm_548(char* buffer, const char* format, ...);

typedef void (__thiscall *OneArgMethodFn)(void*, void*);

void CPlayerWar3::DumpDeadHeroList(void* sink)
{
    void* obj1 = *(void**)((char*)this + 0x2D0);
    ((OneArgMethodFn)(*(void***)obj1)[1])(obj1, sink);

    void* obj2 = *(void**)((char*)this + 0x34);
    ((OneArgMethodFn)(*(void***)obj2)[4])(obj2, sink);

    unsigned int count = m_intMiniCount;
    Storm_548((char*)sink, "Dead hero list size: %d", count);

    for (unsigned int i = 0; i < count; ++i)
    {
        //  Reloaded through +0x2B4 every iteration, not hoisted into a
        //  local before the loop - matching the dump's own
        //  `mov eax, [edi+2B4h]` inside the loop body exactly (hoisting it
        //  outside costs a spill/reload the dump does not have).
        void* hero = (*(void***)((char*)this + 0x2B4))[i];
        if (!hero)
        {
            Storm_548((char*)sink, "No dead hero %d", i);
        }
        else
        {
            unsigned int tag1 = *(unsigned int*)((char*)hero + 0x10);
            unsigned int tag0 = *(unsigned int*)((char*)hero + 0xC);
            Storm_548((char*)sink, "Dead hero %d: age[%d] tags[%u %u]",
                      i, m_intMiniArray[i].m_value, tag0, tag1);
        }
    }
}

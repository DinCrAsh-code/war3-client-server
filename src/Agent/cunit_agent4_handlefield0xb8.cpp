//============================================================================
//  0x6F474B10 - resolve a handle/type-tag pair (`this`'s own +0x8/+0xC,
//  the same shape LookupHandle's own callers use elsewhere - agent.h's own
//  note on the 'lga+' pair) through the already-reconstructed
//  LookupHandle (Game/game.h) and copy whatever it finds at +0xB8 into the
//  caller-supplied out-pointer.
//
//  +0xB8 is far past CHandleObject's own fixed 0x1C bytes, so - matching
//  that class's own rule (game.h: "anything a specific handle-carrying
//  type adds beyond m_typeTag must go on a further-derived struct rather
//  than here") - it is named on a local derived struct instead of on
//  CHandleObject itself; nothing else in this closure says which concrete
//  handle type this is.
//============================================================================
#include "game.h"

struct SAgent4HandleObject_0xB8 : public CHandleObject
{
    char m_reserved1C[0xB8 - 0x1C];
    void* m_field0xB8;   // +0xB8
};

//  `this` (ecx) carries the handle/type-tag pair at +0x8/+0xC; edx is
//  never read at the one call site in this tree, and `out` (the one stack
//  argument, `retn 4`) receives whatever LookupHandle finds at its own
//  +0xB8.  __fastcall's own edx slot has to stay declared even though
//  nothing here reads it, or `out` would land in edx instead of on the
//  stack (docs/msvc-vc8-idioms.md, "A receiver in ecx with a second
//  argument in edx is a free __fastcall").
void __fastcall CopyHandleField0xB8(void* self, void* /*unused edx*/, void** out)
{
    //  +0xC read before +0x8: the shipped stream reads edx off the
    //  original `this` before overwriting ecx with the +0x8 word, and only
    //  this order avoids an extra `mov` to save `this` first.
    int typeTag = *(int*)((char*)self + 0xC);
    unsigned int handle = *(unsigned int*)((char*)self + 8);

    SAgent4HandleObject_0xB8* object =
        (SAgent4HandleObject_0xB8*)LookupHandle(handle, typeTag);

    *out = object->m_field0xB8;
}

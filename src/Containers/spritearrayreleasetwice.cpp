//============================================================================
//  0x6F35D440 - `retn 0`, thiscall.  Zeroes +0x14, tears down the
//  TSGrowableArray<SSpriteRecord> at +0x04, re-zeroes its three fields to a
//  valid empty state, then tears it down a second time - a no-op the second
//  time given the just-zeroed state (m_count 0 skips the element loop,
//  m_data 0 skips the free), but a real second call in the shipped stream,
//  transcribed rather than dropped as apparently dead.
//
//  Not chased past the disassembly to a source-level explanation for the
//  repeat: it is not a call this dump's own callers make with different
//  arguments, and nothing about the outer receiver's own class is
//  established by anything reaching this address.
//
//  TSGrowableArray<SSpriteRecord>::~TSGrowableArray (0x6F3407D0, confirmed
//  by this function's own two call sites - `worktree_store.py show` on this
//  address) is NOT included from tsarray.inl like every other instantiation
//  in this repo: that generic body destroys each live element with
//  `m_data[i].~T()`, which needs SSpriteRecord to be a complete type, and
//  SSpriteRecord's own destructor (sub_6F60F430) is outside this batch's
//  dump. A naked thunk to the real, confirmed address is the honest
//  substitute - link_check.py caught the alternative (declaring it and
//  never defining it, which an earlier pass here did) as a live link
//  failure, not just an unverified guess.
//============================================================================
#include "storm.h"

struct SSpriteRecord;   // 24 bytes, sub_6F60F430's own destructor - out of
                        // this batch's own dump

struct SSpriteArrayHost
{
    char                            m_reserved00[0x04];
    TSGrowableArray<SSpriteRecord>  m_sprites;   // +0x04
    int                             m_field14;   // +0x14
};

template <>
__declspec(naked) TSGrowableArray<SSpriteRecord>::~TSGrowableArray()
{
    __asm { mov eax, 0x6F3407D0 }
    __asm { jmp eax }
}

void __fastcall ReleaseSpriteArrayTwice(SSpriteArrayHost* self)
{
    self->m_field14 = 0;

    self->m_sprites.~TSGrowableArray<SSpriteRecord>();

    self->m_sprites.m_alloc = 0;
    self->m_sprites.m_count = 0;
    self->m_sprites.m_data  = 0;

    self->m_sprites.~TSGrowableArray<SSpriteRecord>();
}

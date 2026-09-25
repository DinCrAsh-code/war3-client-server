//============================================================================
//  0x6F47AA10/0x6F47AA70 - one shipped module: allocate-or-reuse a
//  CBhPoProjectile out of a small fast-path pool, then (from the slot-3
//  override's own caller, projectilepositiontrackattach.cpp) look it up and
//  arm its tick-relay slot 0.
//============================================================================
#include "bhpoprojectile.h"
#include "dataallocator.h"
#include "game.h"

//----------------------------------------------------------------------------
//  The pool CTimeSync::GetField858() (Sync/ctimesyncfield858.cpp) points at.
//  A CDataAllocator (Net/dataallocator.h) at +0x00, sized to hand out
//  CBhPoProjectile-plus-one-header-dword elements (the allocator's own
//  first dword threads its free list the same way `sub_6F4C1AB0`'s own
//  header comment already documents), plus a *second*, bespoke free-list
//  cache this function reads directly instead of going through Alloc()/
//  Free() at all - +0x14/+0x18/+0x1C sit right after the embedded
//  CDataAllocator's own 0x14-byte body ends, so they are this specific
//  pool's own fields, not more of CDataAllocator's.  Only named generically:
//  nothing in this dump establishes what +0x18/+0x1C actually count beyond
//  "AllocateFromPool bumps both by one on every call, on every path".
//----------------------------------------------------------------------------
struct CBhPoProjectilePool
{
    //  0x6F47AA10.
    CBhPoProjectile* AllocateFromPool(int a, int b);

    CDataAllocator m_allocator;      // +0x00
    void*          m_fastFreeHead;   // +0x14 - a node's own first dword is the next link
    unsigned int   m_reserved18;     // +0x18
    unsigned int   m_reserved1C;     // +0x1C
};

//----------------------------------------------------------------------------
//  0x6F47AA10 - pop the pool's own fast free list if it has anything,
//  otherwise grow the underlying CDataAllocator by one element and
//  construct a fresh CBhPoProjectile into it; either way, stamp the two
//  caller-supplied words and two fixed tag constants onto the result and
//  bump both counters.  `retn 8` - two stack arguments, `this` (the pool)
//  in ecx.
//
//  The null check on the freshly constructed object is spelled the way the
//  shipped stream reads it, offset-computed-then-compared rather than
//  compared-then-offset: `obj = (char*)raw + 4; if (obj) ...` - which the
//  shipped code cannot actually observe failing (a real null `raw` would
//  make `obj` equal to 4, not 0), so this is what the source looked like,
//  not a robustness check that works.
//----------------------------------------------------------------------------
CBhPoProjectile* CBhPoProjectilePool::AllocateFromPool(int a, int b)
{
    //  `raw` starts as the free list head itself (0 on the alloc path) -
    //  the shipped stream reaches into that same dead register for Alloc's
    //  own `zeroMemory` argument on that path (`push edi`, not a fresh
    //  `push 0`) rather than reloading a literal, the same
    //  argument/register-reuse idiom this repo's own SetMaxSpeed writeups
    //  already document.
    void* raw = m_fastFreeHead;

    if (raw == 0)
    {
        raw = m_allocator.Alloc((int)raw, "AVCBhPoProjectile@NIpse@@", -2);

        CBhPoProjectile* obj = (CBhPoProjectile*)((char*)raw + 4);
        if (obj != 0)
            obj->CBhPoProjectile::CBhPoProjectile();
    }
    else
    {
        m_fastFreeHead = *(void**)raw;
    }

    CBhPoProjectile* obj = (CBhPoProjectile*)((char*)raw + 4);
    ++m_reserved18;
    *(int*)((char*)obj + 4) = a;
    *(unsigned int*)((char*)obj + 0xC) = 0x3E202062;
    *(unsigned int*)((char*)obj + 0x10) = 0x3E70726A;
    *(int*)((char*)obj + 8) = b;
    ++m_reserved1C;

    return obj;
}

//----------------------------------------------------------------------------
//  0x6F47AA70 - `this` here is the caller's own small on-stack scratch
//  block (the 11-dword local built just before this call in
//  projectilepositiontrackattach.cpp), not a CBhPoProjectile: it stashes
//  the pool's freshly allocated object at its own +0x00, then - if the
//  stack argument is non-zero - dispatches the new object's own vtable slot
//  0x0C with the second (register) argument.  `retn 4` - one stack
//  argument, `this` in ecx, `edx` a second, register-passed argument no
//  MSVC calling convention keyword reaches (a raw two-register receiver,
//  the same shape `game.h`'s CLAUDE.md-documented indirect-dispatch idiom
//  covers, so this stays a free function rather than a mis-declared
//  `__thiscall`/`__fastcall`).
//----------------------------------------------------------------------------
typedef void (__thiscall *BhPoProjectileSlot0xCFn)(void*, void*);

void __fastcall LookupAndArmSlot(void* outObj /*ecx*/, void* record /*edx*/,
                                 int dispatch)
{
    CBhPoProjectilePool* pool = (CBhPoProjectilePool*)GetTimeSyncField858();
    CBhPoProjectile* obj = pool->AllocateFromPool(0, 0);

    *(CBhPoProjectile**)outObj = obj;

    if (dispatch != 0)
    {
        void** vtbl = *(void***)obj;
        ((BhPoProjectileSlot0xCFn)vtbl[3])(obj, record);
    }
}

//============================================================================
//  CMiscCustom - the pair of configuration sources a map can override the
//  game's constants with, and (checksum-batch-E) the checksum-provider
//  registry's own slot 19 (CGameDataCache::m_pMiscCustom,
//  docs/notes/checksum-provider-registry.md) - RTTI-confirmed to derive
//  from CSynchronousData (agent_worktrees/classes/0x6F8750A4.json,
//  `CMiscCustom: CSynchronousData;`).
//
//  Only the constructor was in the sub_6F0020A0 dump, which is why the two
//  pointer-sized members after the vftable were reserved bytes.  The
//  sub_6F020D30 target reaches both of them: each is a CConfigSource, they are
//  tried in order, and a value is taken from the first one that has the key.
//  Nothing says what distinguishes the two, so they are named for the only
//  thing the code proves - the order they are consulted in.
//
//  m_vtable is a real member, not compiler-generated polymorphism: the call
//  tree only ever proves this class has a vtable pointer (the constructor
//  installs one), never what the class's actual virtual-method set is, so a
//  `virtual ~CMiscCustom()` here would build our own, incomplete vtable and
//  install *that* - textually identical to the shipped store, but wrong at
//  runtime the moment anything calls through it. Declaring the slot as a
//  plain pointer and writing the real, live vtable address (0x6F8750A4,
//  found via debugger - see CMiscCustom.cpp) keeps the exact same one-store
//  instruction shape while pointing it at the real thing.
//
//  checksum-batch-E confirmed the vtable's own remaining three slots
//  against a fresh RTTI dump (0x6F8750A4.json's own vtable_members) and
//  gave each a real body: slot 0 (ComputeChecksum, 0x6F0020D0 - the slot
//  the registry's own generic per-tick walker,
//  Config/miscdata.cpp's GameDataContextChecksumProviderHash, actually
//  calls), slot 2 (DeleteSelf, 0x6F002760, the scalar deleting destructor)
//  and slot 3 (Method3, 0x6F0020C0, purpose not established - see its own
//  note below). Slot 1 stays the shared nullsub_53 DebugDumpTo default,
//  same reasoning Game/synchronousdata.h's own header already gives -
//  not reconstructed as if unique to this class.  None of the four is
//  declared `virtual`, for the same reason the class itself is not
//  polymorphic in this source: the real vtable at 0x6F8750A4 is never one
//  this repo's own constructor stamps, so there is no C++-level dispatch
//  to model, only four ordinary member functions whose *addresses* happen
//  to be what the real vtable's data holds.
//============================================================================
#ifndef CMISCCUSTOM_H
#define CMISCCUSTOM_H

#include "config.h"

class CMiscCustom
{
public:
    CMiscCustom();
    //  0x6F0024E0 - the real, non-virtual ~CMiscCustom(): re-stamps the
    //  real vtable address (matching this class's own ctor, and the same
    //  "guard the vtable across teardown" shape a genuinely polymorphic
    //  class's compiler-generated destructor would produce on its own -
    //  see the class header above for why this class cannot use `virtual`
    //  to get that for free) then tail-calls TeardownSources() below.
    ~CMiscCustom();

    //  0x6F0020D0 - vtable slot 0.  Folds each non-null source's own
    //  CConfigSource::ComputeChecksum() (config.h) into a running hash via
    //  rol(hash ^ contribution, 3) - the same combine every other
    //  checksum-provider class in this family (CGameWar3, CRandData,
    //  CUnitDatabase, ...) already uses.  m_pPrimary is folded first,
    //  matching the dump's own `+0x04` before `+0x08` read order (its
    //  first instruction is `mov ecx,[edi+4]`).  Inherited unchanged by
    //  CSkinCustom (CSkinCustom.h) - the RTTI dump names the identical
    //  target address for both classes' own slot 0, so CSkinCustom does
    //  not override it.
    unsigned int ComputeChecksum() const;

    //  0x6F002760 - vtable slot 2, the scalar deleting destructor: calls
    //  the real destructor above, then frees `this` through Storm's
    //  allocator when bit 0 of `flags` is set.  Same shape this repo's
    //  other DeleteSelf overrides already use (e.g.
    //  Item/abilitycustomdata_deleteself.cpp).
    CMiscCustom* DeleteSelf(unsigned int flags);

    //  0x6F0020C0 - vtable slot 3, `xor eax,eax ; retn`: always answers 0
    //  for this class.  CSkinCustom overrides it to answer 1
    //  (CSkinCustom.h) - the only difference this batch found between the
    //  two classes' own vtables besides DeleteSelf's own dtor target.
    //  Purpose not established from the call tree available to this
    //  batch; kept as a generic Method3(), matching
    //  Item/abilitycustomdata.h's own naming for an equally unidentified
    //  CCustomData-family slot rather than guessing at a name the
    //  evidence does not support.
    unsigned int Method3() const;

    //  0x6F002170
    int HasValue(const char* section, const char* key) const;
    //  0x6F002300
    CFloat GetFloat(const char* section, const char* key, int index) const;
    //  0x6F002220 - the int sibling of GetFloat below, and the same
    //  two-source shape.  Reconstructed in CMiscCustom.cpp.
    int GetInt(const char* section, const char* key, int index) const;
    //  0x6F002290 - the plain-hardware-float sibling of GetFloat, and the
    //  same two-source shape.  Like CConfigSource::GetFloatRaw, which it
    //  forwards straight into, it hands the value back through ST(0) rather
    //  than through a CFloat return buffer.  The argument order it passes
    //  on is (section, key) even though that member's parameters are named
    //  (key, section) - see the note beside 0x6F6F34C0 in config.h.
    float GetFloatRaw(const char* section, const char* key, int index) const;
    //  0x6F002400 - the string sibling of the two above, `retn 0Ch`,
    //  reached on the same object from CGameUI's skin fallback
    //  (gameuiskinvalue.cpp) and from the camera slice.  Batch A met it as
    //  a redirect it had to give a receiver type of its own
    //  (SMiscCustomText) because this class's own body was not yet
    //  written; it is this class's, and CMiscCustom.cpp is the body.
    const char* GetString(const char* section, const char* key,
                          int index) const;

    void*          m_vtable;        // 0x00 - see the class comment above
    CConfigSource* m_pPrimary;      // 0x04
    CConfigSource* m_pSecondary;    // 0x08

private:
    //  0x6F002130 - ~CMiscCustom()'s own tail call: release each non-null
    //  source through its own vtable slot 0 with flags=1 (config.h's own
    //  note on CConfigSource's reserved first field has the dispatch
    //  shape) and null the pointer.  Secondary first, primary second -
    //  the dump's own field-descending order, matching ComputeChecksum()
    //  above.  Declared here, defined in its own TU
    //  (miscustom_teardownsources.cpp) for the same reason
    //  Item/abilitycustomdata_release.cpp's own ReleaseOwnedRefAndFinalize()
    //  is split from its own destructor: ~CMiscCustom() (miscustom_dtor.cpp)
    //  must reach it through a real, un-inlined `call`, matching the dump's
    //  own `jmp sub_6F002130` tail chain - with the two in one TU this
    //  compiler folds the whole teardown into the destructor and, from
    //  there, into every one of the destructor's own callers.
    void TeardownSources();
};

#endif

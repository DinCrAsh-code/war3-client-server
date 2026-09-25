//============================================================================
//  CSynchronousData - the real, RTTI-confirmed base class every entry of
//  CGameDataContext's checksum-provider array (gamecontext.h) derives from.
//
//  Confirmed by reading the compiler-emitted RTTI, not guessed from the
//  vtable shape alone: CCheatData, CRandData, CNetData and CGameWar3 each
//  carry a `??_R2<Class>@@8` base-class array whose second entry is
//  `??_R1A@?0A@EA@CSynchronousData@@8` at `mdisp 0` (checksum-provider-
//  registry.md has the byte-level read of all four).  `mdisp 0` means this
//  is the *first* base and shares its subobject's vtable-pointer slot with
//  the derived class's own vtable, which is exactly why every one of those
//  four classes' own reconstructed/observed vtables are the identical two
//  slots this base declares - none of them adds a virtual of its own.
//
//  Two virtuals, both confirmed against a live dump:
//    slot 0 - ComputeChecksum(): this provider's own contribution to the
//      per-tick sync hash GameDataContextChecksumProviderHash
//      (sub_6F005E20) folds together.  The base's own body (0x6F001FF0,
//      `xor eax,eax ; retn`) always answers 0 - a provider that never
//      overrides it is inert for sync purposes, which is exactly the shape
//      the debug print's own three "repurposed" slots (CAbilityDatabase,
//      CAbilityCustomData, CMiscCustom - none of them derive from this
//      class at all, so their own unrelated vtable slot 0 gets called
//      instead when the debug print's unbounded 20-slot walk reaches them,
//      per checksum-provider-registry.md's own "shared lazy-singleton slot
//      array" finding).
//    slot 1 - DebugDumpTo(buffer): format a one-line human-readable dump of
//      this provider's own state into the caller's buffer.  The base's own
//      body is the shipped binary's `nullsub_53` (0x6F002000, `retn 4`) -
//      a no-op, but *shared*: nullsub_53 is COMDAT-folded across a couple
//      dozen unrelated classes' own trivial virtuals (CAbilityDatabase's
//      own vtable uses the identical address for an unrelated slot), so it
//      is not reconstructed here as if it belonged to this class alone -
//      this header's own DebugDumpTo is a distinct, real definition that
//      compiles to its own address, not a claim on 0x6F002000.
//
//  Both CCheatData's and CRandData's own installers (Misc/cheatdata.cpp,
//  Sync/randdata.cpp) show only ONE vtable store at construction (their own
//  derived vtable, not a visible prior store of this base's), unlike the
//  CLayer/CObserver pair docs/msvc-vc8-idioms.md documents where the base
//  ctor's own store survives as a real, visible instruction. The
//  difference: CObserver's own ctor does other real work and stays a call
//  MSVC cannot inline away, so its own vtable store is never in the same
//  optimisation window as the derived one. This base's ctor is nothing but
//  the store itself (0x6F001FE0, `mov [ecx], offset vftable ; retn`) - small
//  enough that /Ob2 inlines it flat into every one of this base's derived
//  constructors, at which point the immediately-following derived-vtable
//  store to the exact same four bytes is a classic dead store and gets
//  eliminated before the derived ctor's own body is ever emitted. Modelling
//  this as ordinary public inheritance is still the matching spelling per
//  that same doc section ("a base class that already has one costs nothing
//  to inherit from") - the *shipped* RTTI says the inheritance is real, and
//  this toolchain reproduces the single-store shape from it without any
//  extra source-level trick.
//============================================================================
#ifndef SYNCHRONOUSDATA_H
#define SYNCHRONOUSDATA_H

//  0x6F87507C - ??_7CSynchronousData@@6B@.
class CSynchronousData
{
public:
    //  0x6F001FE0 - the vtable stamp only; this base carries no data of its
    //  own, so there is nothing else for it to initialise.
    CSynchronousData();

    //  0x6F001FF0 - vtable slot 0.  Base default: no contribution.
    virtual unsigned int ComputeChecksum();

    //  Vtable slot 1.  Base default: nothing to print.  See the file header
    //  above for why this is a real, distinct definition rather than a
    //  reconstruction of the shared nullsub_53 stub the shipped binary
    //  actually points this slot at.
    virtual void DebugDumpTo(void* buffer);
};

#endif

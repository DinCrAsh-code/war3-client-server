//============================================================================
//  0x6F42C210 - CAgentRegistrySingleton::FinishSingleton, the "you are the
//  singleton now" step CItemSlotSingletonHost::GetSlotTable
//  (itemslotsingleton.cpp) runs on the '+gam' agent it has just made.
//
//  The receiver is that agent: GetSlotTable calls this on the object it
//  just parked in its own SCheckedGameSlot, and the same object is what
//  every JASS handle resolution then indexes through.  It carries the
//  day/night state, which is what this function initialises.
//============================================================================
#ifndef AGENTREGISTRYSINGLETON_H
#define AGENTREGISTRYSINGLETON_H

class CAgent;
class CFloat;

//----------------------------------------------------------------------------
//  0x6F427F40 - already reconstructed, EXACT, in
//  Misc/misc_field_getters.cpp, where it was named for what it does with
//  no caller to explain it: element `index` of a 16-byte-stride inline
//  array at this+8.  This target supplies the caller, and the answer is
//  used as an SUnitTrackedRef* (unittrackedref.h) - which is exactly
//  sixteen bytes, so the array is an inline `SUnitTrackedRef[]`.
//
//  Re-declared here rather than moved to a header: MSVC puts the `struct`
//  keyword, the class name, the return type and the parameter type all in
//  the mangled name, so this declaration and misc_field_getters.cpp's
//  definition are the same symbol only while all four agree.  Changing
//  either without the other is the LNK2019 that
//  docs/notes/verifier-gate-link-failures.md is about.
//----------------------------------------------------------------------------
struct IndexedElementOwner_6F427F40
{
    char pad[8];
    char m_data[1][16];
    void* At(unsigned int index);
};

class CAgentRegistrySingleton
{
public:
    //  0x6F42C210 - `retn 0`.  A __thiscall member with no arguments.
    void FinishSingleton();

    //  0x6F429460 - rebuild the two day/night boundary listeners from the
    //  configured Dawn/Dusk hours.  Own TU, agentdaynight.cpp; `retn 0`.
    void RebuildDayNightListeners();

    //------------------------------------------------------------------
    //  Only what this call tree reads.
    //------------------------------------------------------------------
    char        m_reserved00[0x78];
    //  0x78 / 0x7C - the two boundary listeners (dawn and dusk), each a
    //  refcounted FloatListener the day/night rebuild replaces wholesale.
    //  Released through vtable slot 2 and then refcount-released, which
    //  is the two-step teardown a listener registered with a publisher
    //  needs.
    CAgent*     m_pDawnListener;
    CAgent*     m_pDuskListener;
    //  0x80 - the "is it daytime" flag, held as a handle-bearing sub-object
    //  rather than a bare word: it is published, so anything watching it
    //  has to be told when it changes (misc_handle_lookups.cpp's
    //  QueryField78 reads it, sub_6F473140 writes it).
    char        m_isDaytime[0x23C];         // 0x80
    //  0x2BC - the '+qum' agent this function registers.
    char        m_qumSlot[4];               // 0x2BC
};

#endif

//============================================================================
//  CPathingMapIndicator - RTTI-confirmed (agent_worktrees/classes/0x6F9409F0.json,
//  vtable @ 0x6F9409F0, "CPathingMapIndicator: TRefCnt;") five-slot class:
//  slot 0 is TRefCnt::ReleaseSelf, inherited unchanged (0x6F00C060, the same
//  address as TRefCnt::ReleaseSelf itself - refcnt.h); slots 1-4
//  (0x6F381510/0x6F378AA0/0x6F378C70/0x6F377BD0) are all out of this
//  session's own scope (no dump) and declared here as naked thunks so this
//  class's own vtable stays the real five-slot shape rather than an
//  incomplete one.
//
//  Field layout read straight off the constructor/destructor dumps
//  (pathingmapindicator_ctor.cpp / pathingmapindicator_dtor.cpp) - see
//  those files for the reasoning behind each name.  A large block of
//  fields (+0x28..+0x78) is zeroed by the constructor and individually
//  freed (each its own Storm_403 tag) by the destructor without this
//  session ever seeing a reader of any of them, so they are named by
//  offset alone.
//============================================================================
#ifndef PATHINGMAPINDICATOR_H
#define PATHINGMAPINDICATOR_H

#include "refcnt.h"
#include "CFloat.h"
#include "renderdevice.h"   // TEXTUREIMAGE - Render/textureimagector.cpp

class CPathingMapIndicator : public TRefCnt
{
public:
    CPathingMapIndicator(void* arg0, void* arg4, void* arg8);   // 0x6F381430, `retn 0Ch`
    ~CPathingMapIndicator();                                     // 0x6F37CCC0

    //  vtable slots 1-4 - all out of this session's own scope (no dump).
    //  Naked thunks, kept only so this class's own vtable is the real
    //  five-slot shape.
    virtual TRefCnt* DeleteSelf(int flags);          // slot 1, 0x6F381510
    virtual void Slot2();                            // slot 2, 0x6F378AA0
    virtual void Slot3();                            // slot 3, 0x6F378C70
    virtual void SetArg0(void* arg0);                // slot 4, 0x6F377BD0 - called
                                                       // directly (not virtually) from
                                                       // this class's own constructor.

    CFloat       m_f08;             // +0x08 = 0.0
    CFloat       m_f0C;             // +0x0C = 0.0
    CFloat       m_f10;             // +0x10 = 0.0
    CFloat       m_f14;             // +0x14 = 0.0
    unsigned int m_reserved18;      // +0x18 = 0
    unsigned int m_reserved1C;      // +0x1C = 0
    void*        m_arg4;            // +0x20 - the constructor's second argument
    void*        m_arg8;            // +0x24 - the constructor's third argument
    char         m_reserved28[8];    // +0x28
    void*        m_vec30;            // +0x30 - freed tagged ".?AVCIM..." (aAvcimvectorNte)
    char         m_reserved34[0x10]; // +0x34
    void*        m_vec44;            // +0x44 - freed tagged ".?AV?$vector..." (aAvc3vectorNtem)
    char         m_reserved48[0xC];  // +0x48
    void*        m_vec54;            // +0x54 - freed tagged ".?AV?$vector..." (aAvc3vectorNtem)
    char         m_reserved58[8];    // +0x58
    void*        m_g60;              // +0x60 - freed tagged "G" (aG_4)
    char         m_reserved64[8];    // +0x64
    void*        m_vec6C;            // +0x6C - freed tagged ".?AV?$vector..." (aAvc3vectorNtem)
    char         m_reserved70[8];    // +0x70
    void*        m_vec78;            // +0x78 - freed tagged ".?AV?$vector...2" (aAvc2vectorNtem)
    TEXTUREIMAGE m_subObject7C;        // +0x7C - 0x6F526F70, Render/textureimagector.cpp
};

#endif

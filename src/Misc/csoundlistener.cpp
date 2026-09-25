//============================================================================
//  CSoundListener - RTTI name only (??_7CSoundListener@@6B@, stamped
//  directly into `[edi]` by its own constructor below); nothing else in
//  this batch's dump reaches or is reached by this class, so only the one
//  function this dump actually contains is reconstructed.
//
//  0x6F4E9DA0 - CSoundListener::CSoundListener().  The same "CDataMgr base,
//  then own vtable, then embedded CBaseManaged-vtabled members" shape
//  Misc/ccamera.cpp's own CCamera::CCamera documents for the rest of this
//  family - see that file's header for the shared reasoning (the
//  `__except_handler4`-shaped SEH frame this toolchain cannot reproduce,
//  the CDataMgr base's own dead vtable store, why CDataMgr is not modelled
//  as a real C++ base here) - just with a two-slot property table instead
//  of nine, and two TManaged<NTempest::C3Vector> members (likely position
//  and orientation/velocity for a 3D audio listener, though nothing in
//  this dump reads either back to confirm) instead of CCamera's full nine.
//  `DIFFERS` on the frame alone, same as CCamera::CCamera and
//  CAngle::CAngle.
//
//  CSoundListener's own real virtual functions are not established by this
//  dump (nothing here calls one) - the placeholder below exists only so
//  the class is polymorphic enough for its constructor to stamp
//  `??_7CSoundListener@@6B@` at all, the same "invents no field this class
//  has no evidence for" reasoning Misc/ccamera.cpp's own file header used
//  for CCamera before its own constructor dump arrived.
//============================================================================
#include "storm.h"
#include "cameraangle.h"

namespace NTempest
{
//  `class`, not `struct` - see Misc/ccamera.cpp's own copy of this type for
//  why (the shipped vtable symbol mangles it with the class tag `V`).
class C3Vector
{
public:
    float x, y, z;
};
}

//  Same generic shape Misc/ccamera.cpp declares for its own two
//  TManaged<NTempest::C3Vector> members - see that file for the field
//  derivation.  Declared again here rather than shared: each of this
//  family's own files declares its own minimal, tailored stand-in.
template <class T>
class TManaged : public CBaseManaged
{
public:
    TManaged(float field18, const T& value)
    {
        m_reserved0C = 0;
        m_flags = 0;
        m_reserved0E[0] = 0;
        m_reserved0E[1] = 0;
        m_field10 = 0;
        m_field14 = 0;
        m_field18 = field18;
        m_value = value;
    }

    unsigned char m_reserved0C;
    unsigned char m_flags;
    unsigned char m_reserved0E[2];
    int           m_field10;
    int           m_field14;
    float         m_field18;
    T             m_value;
};

struct SManagedPropertyRegistrar3 { void __thiscall Register(void*, int, unsigned int); };

class CSoundListener
{
public:
    virtual void Method0() {}   // unestablished - see file header

    CSoundListener();
};

//  0x6F4E9DA0.
CSoundListener::CSoundListener()
{
    char* p = (char*)this;

    //  CDataMgr base: field4, its own TSFixedArray<CBaseManaged*> property
    //  table sized to *two* slots this time (Widget/cdatamgrpropertyarray.cpp
    //  is the same SetAlloc, 0x6F4D4F20), and the TSExplicitList<CBaseManaged,4>
    //  sentinel head reset to empty.
    *(int*)(p + 0x04) = 0;

    TSFixedArray<CBaseManaged*>* properties =
        (TSFixedArray<CBaseManaged*>*)(p + 0x08);
    properties->m_alloc = 0;
    properties->m_count = 0;
    properties->m_data = 0;
    if (properties->m_count != 2)
    {
        properties->SetAlloc(2);
        properties->m_count = 2;
    }

    TSExplicitList<CBaseManaged, 4>* list =
        (TSExplicitList<CBaseManaged, 4>*)(p + 0x14);
    list->m_linkoffset = 4;
    list->m_terminator.m_next = (TSLink<CBaseManaged>*)(p + 0x18);
    list->m_terminator.m_prevlink = ~(int)(p + 0x18);

    NTempest::C3Vector vecZero = { 0.0f, 0.0f, 0.0f };
    new (p + 0x20) TManaged<NTempest::C3Vector>(0.0f, vecZero);
    new (p + 0x48) TManaged<NTempest::C3Vector>(0.0f, vecZero);

    ((SManagedPropertyRegistrar3*)this)->Register(p + 0x20, 0, 0);
    ((SManagedPropertyRegistrar3*)this)->Register(p + 0x48, 1, 0);
}

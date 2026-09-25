//============================================================================
//  FloatModifier - ".\FloatProp.cpp"'s own sixteen-byte record, made when
//  something asks a tracked float to travel to a new value over a span.
//
//  It is the same design as MovementModifier (positionmodifier.h) and
//  FloatListener (floatboundarylistener.cpp): the same four words, the same
//  Storm allocation with the module's __FILE__/__LINE__, the same "make,
//  then run two of the made object's own slots" construction, and the same
//  one-word counted reference handed back through MSVC's hidden return
//  buffer.  Everything said there applies here and is not repeated.
//
//  The request block is *not* MovementModifier's: it carries a different
//  type id and leaves +0x14 a plain zero where the position copy stores a
//  third CFloat there.  Two modules, two blocks.
//============================================================================
#ifndef FLOATPROPMODIFIER_H
#define FLOATPROPMODIFIER_H

#include "unittrackedref.h"
#include "game.h"
#include "positionmodifier.h"   // SModifierSubject / SMadeModifier

//  ??_7FloatModifier@@6B@ - the shipped vtable, by address.  See
//  positionmodifier.h for why it is a data symbol and not a class with
//  virtuals.
extern void* const g_vftFloatModifier;

struct SFloatModifierRequest
{
    unsigned int m_tag;          // +0x00 - '^mod'
    unsigned int m_typeId;       // +0x04 - 'mcr`'
    unsigned int m_reserved08;   // +0x08 - 0
    CFloat       m_from;         // +0x0C - read out of g_CFloatZero
    CFloat       m_to;           // +0x10 - likewise
    unsigned int m_reserved14;   // +0x14 - 0, and a plain zero here
    unsigned int m_reserved18;   // +0x18 - 0
    unsigned int m_reserved1C;   // +0x1C - 0
    unsigned int m_reserved20;   // +0x20 - 0
    int          m_mode;         // +0x24 - -1, or -2 for a synced subject
    int          m_reserved28;   // +0x28 - -1
};

//----------------------------------------------------------------------------
//  The FloatProp side of what a modifier's handle pair resolves to.  Members
//  for the reason SPositionSink (positionmodifier.h) gives.
//----------------------------------------------------------------------------
struct SFloatSink
{
    //  0x6F4A9ED0 - `retn 0Ch`.
    void SetSpan(const CFloat* span, int a, int b);
    //  0x6F4A9B00 - `retn 4`.
    void SetValue(const CFloat* value);
};

struct FloatModifier
{
    FloatModifier()
    {
        m_refcount = 0;
        m_handle = (unsigned int)-1;
        m_typeTag = -1;
        m_vtable = &g_vftFloatModifier;
    }

    //  0x6F478B50 - `retn 8`.
    void* Bind(SModifierSubject* subject, int mode);
    //  0x6F478C50 - `retn 0Ch`.
    void SetSpan(const CFloat* span, int a, int b);
    //  0x6F478C80 - `retn 4`.
    void SetTarget(const CFloat* value);
    //  0x6F478CA0 - `retn 10h`.
    void Init(SModifierSubject* subject, SUnitTrackedRef* source,
              const CFloat* value, const CFloat* span);

    const void*  m_vtable;    // +0x00
    int          m_refcount;  // +0x04
    unsigned int m_handle;    // +0x08
    int          m_typeTag;   // +0x0C
};

//  The counted reference MakeFloatModifier returns by value.  See
//  positionmodifier.h's own twin for why it has a destructor.
struct SFloatModifierRef
{
    ~SFloatModifierRef()
    {
        FloatModifier* held = m_ptr;
        if (held != 0)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                ((void (__thiscall *)(void*))(*(void***)held)[0])(held);
        }
    }

    FloatModifier* m_ptr;
};

#endif

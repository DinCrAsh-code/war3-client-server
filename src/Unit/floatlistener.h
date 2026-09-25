//============================================================================
//  FloatListener - ".\FloatProp.cpp"'s four-word record, made when something
//  asks a tracked float to tell it when the value crosses one boundary.
//
//  It is the third member of the same design MovementModifier
//  (positionmodifier.h) and FloatModifier (floatpropmodifier.h) already
//  carry: the same four words, the same sixteen-byte Storm allocation with
//  the module's own __FILE__/__LINE__, the same "submit a request block to
//  the global maker and adopt the handle pair of what came back"
//  construction, and the same one-word counted reference handed back
//  through MSVC's hidden return buffer.  Everything argued there applies
//  here and is not repeated.
//
//  The struct used to live inside floatboundarylistener.cpp.  It is a
//  header now because four translation units need it: the maker
//  (floatboundarylistener.cpp) and the three members below, which call each
//  other for real and so may not share a translation unit.
//============================================================================
#ifndef FLOATLISTENER_H
#define FLOATLISTENER_H

#include "unittrackedref.h"
#include "game.h"
#include "positionmodifier.h"   // SModifierSubject / SMadeModifier

//----------------------------------------------------------------------------
//  The vtable the shipped code stamps, as a data symbol rather than as a
//  C++ class with eight virtuals.
//
//  `??_7FloatListener@@6B@` is at 0x6F878E40 and its eight slots
//  (0x6F00C060, 0x6F053610, 0x6F47AD30, 0x6F4788C0, 0x6F478900,
//  0x6F478820, 0x6F47AD60, 0x6F47AC30 - `ida_query vtable 0x6F878E40`)
//  are all real bodies outside this call tree.  Declaring the class with
//  eight `virtual`s would make this build emit its own vtable and then
//  fail to link every one of those eight slots, so the pointer is what is
//  modelled: an `extern` at the vtable's own address, bound through
//  funcmap.DATA's `off_6F878E40` row the same way every other game global
//  in this repo is.  `mov [reg], SYM` either way, which is what the
//  instruction score sees.
//----------------------------------------------------------------------------
extern void* const g_vftFloatListener;      // ??_7FloatListener@@6B@

//----------------------------------------------------------------------------
//  The 0x2C-byte request block FloatListener::Bind builds on its stack.
//  Same shape as SModifierRequest and submitted through the same
//  0x6F48AB80, but its own tag pair and only one CFloat: where the
//  modifier blocks park two or three encoded floats at +0x0C onwards, this
//  one parks a single g_CFloatZero and leaves +0x10 a plain zero.
//
//  The tags are 0x5E6C6973 ('^lis') and 0x6072746C ('`rtl'), both read most
//  significant byte first.  Neither is invented here: the same pair is the
//  first two words of the event context CDrivenRelation::Update announces
//  arrival with (agentrelationdrive.cpp), and '`rtl' is that announcement's
//  event code as well - which is what says the object this block asks for
//  and the object that announces to a listener are the same kind of thing.
//----------------------------------------------------------------------------
struct SFloatListenerRequest
{
    unsigned int m_tag;          // +0x00 - '^lis'
    unsigned int m_typeId;       // +0x04 - '`rtl'
    unsigned int m_reserved08;   // +0x08 - 0
    CFloat       m_from;         // +0x0C - read out of g_CFloatZero
    unsigned int m_reserved10;   // +0x10 - 0, a plain zero here
    unsigned int m_reserved14;   // +0x14 - 0
    unsigned int m_reserved18;   // +0x18 - 0
    unsigned int m_reserved1C;   // +0x1C - 0
    unsigned int m_reserved20;   // +0x20 - 0
    int          m_mode;         // +0x24 - -1, or -2 for a synced subject
    int          m_reserved28;   // +0x28 - -1
};

//----------------------------------------------------------------------------
//  The listener itself.  Four words, and the constructor writes three of
//  them plus the vtable - which is why sixteen bytes is the allocation
//  size and not a guess.
//----------------------------------------------------------------------------
struct FloatListener
{
    //  Placement-new'd over Storm's block, which is what produces the
    //  shipped `test eax,eax` / fill / `mov esi,eax` / `jmp` over an
    //  `xor esi,esi`: placement new's own null test skips the constructor
    //  and the expression's value is the null pointer.  Writing the fill
    //  as an `if (p) { ... }` on an already-named pointer instead keeps
    //  the pointer in one register throughout and loses the join
    //  entirely - the same idiom OsNet::TCPMGR::CreateConnection
    //  (ostcp_create.cpp) already records.
    //
    //  Field order is the shipped store order, vtable last.  It is a
    //  plain member and not a `virtual` table: see g_vftFloatListener
    //  above.
    FloatListener()
    {
        m_refcount = 0;
        m_handle = (unsigned int)-1;
        m_typeTag = -1;
        m_vtable = &g_vftFloatListener;
    }

    //  0x6F480E90 (`retn 10h`) - build the request block, submit it, adopt
    //  the handle pair of whatever came back, tell this listener's own
    //  slot 7 about both ends, and finally take a channel registration on
    //  the made object under `msgId`.  See floatlistenerbind.cpp.
    void* Bind(SModifierSubject* subject, unsigned int msgId,
               void* target, int mode);

    //  0x6F480F80 (`retn 8`) - publish the boundary onto the made object:
    //  which side of it fires, then the boundary value itself.  See
    //  floatlistenerboundary.cpp.
    void SetBoundary(int above, const CFloat* boundary);

    //  0x6F480FB0 (`retn 1Ch`) - the whole of the above in shipped order,
    //  the FloatProp twin of MovementModifier::Init / FloatModifier::Init.
    //  See floatlistenerinit.cpp.
    void Init(SModifierSubject* subject, SUnitTrackedRef* source, int above,
              const CFloat* boundary, unsigned int msgId, void* target,
              int mode);

    const void*  m_vtable;   // +0x00
    int          m_refcount; // +0x04
    unsigned int m_handle;   // +0x08
    int          m_typeTag;  // +0x0C
};

//  The counted reference MakeBoundaryListener returns by value.  One word,
//  a real copy semantic (the addref at the tail), and nothing else this
//  call tree can see.
struct SFloatListenerRef
{
    FloatListener* m_ptr;
};

#endif

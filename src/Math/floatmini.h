//============================================================================
//  FloatMini / FloatMiniB: a two-level base pair CItem__Constructor
//  (0x6F2B7520) constructs in place right after its own vtable store -
//  base-then-derived, both fully inlined (single call site, same shape as
//  CWidget's own tail in widget.h): FloatMini's constructor sets its own
//  vtable and one CFloat field, then FloatMiniB's constructor overwrites
//  the same vtable slot with its own and adds two more fields.
//
//  Unlike CAgentWar3/TRefCnt (agentwar3.h/refcnt.h), the redundant
//  FloatMini-then-FloatMiniB vtable store is *not* eliminated here even
//  though both are inlined into the same straight-line block - the
//  distinguishing fact is that FloatMini's own constructor does real work
//  besides the vtable store (m_value), so the whole thing runs for real
//  rather than being skipped outright the way a base constructor with
//  *nothing but* a vtable store is (see refcnt.h's own note on this for
//  TRefCnt/Position).
//
//  FloatMiniB's own two fields come from two more unmapped globals
//  (g_unk6FAAE624/g_unk6FAAE628, funcmap.py's DATA table) that sit in the
//  same 0x6FAAExxx run as every other named CFloat constant in this
//  codebase (CFloat.h, boxmath.h, gamebounds.h, quadrant.h) but that
//  nothing in this call tree reads back or does arithmetic on - so they
//  are plain dwords here, not CFloat, per CLAUDE.md's "do not invent
//  fields the call tree does not read" (an interpretation, not a fact this
//  call tree can confirm).
//============================================================================
#ifndef FLOATMINI_H
#define FLOATMINI_H

#include "CFloat.h"

class FloatMini
{
public:
    //  Inlined at its one call site (CItem::CItem, item.cpp) - sets the
    //  vtable and one field, nothing else.
    FloatMini() : m_value(g_CFloatZero) {}

    //  A trivial, non-pure virtual destructor: the only thing this call
    //  tree needs from FloatMini is that it *have* a vtable for
    //  FloatMiniB to override (see refcnt.h's own note on why classes
    //  embedded by value can't leave a base virtual unresolved even when
    //  nothing calls through it).
    virtual ~FloatMini() {}

    //  0x6F4770E0 - see floatmini_dump.cpp.  Not virtual: the shipped
    //  callers reach it with a direct `call`, and vtable slot 0 is the
    //  two-argument notify FloatMiniB::SetPair uses.
    void DumpTo(void* sink);

    CFloat m_value;   // +0x04 - g_CFloatZero
};

extern const unsigned int g_unk6FAAE624;   // funcmap.py's DATA table
extern const unsigned int g_unk6FAAE628;   // funcmap.py's DATA table

class FloatMiniB : public FloatMini
{
public:
    //  Inlined at its one call site the same way FloatMini's own
    //  constructor is - declared in this header rather than a .cpp so the
    //  compiler actually can.
    //  The two globals stay `unsigned int` (see the file header): retyping
    //  *them* would change their mangled names, and the DATA table in
    //  tools/funcmap.py keys off those.  The fields themselves are CFloat -
    //  see below.
    FloatMiniB()
        : m_min(*(const CFloat*)&g_unk6FAAE628),
          m_max(*(const CFloat*)&g_unk6FAAE624) {}

    //  Both fields at once, then a notify through FloatMini's own vtable
    //  slot 0 with this object's m_value and a literal 1.  Inline in the
    //  header because the shipped code has it inlined at both its call
    //  sites (CItem's vtable slots 80 and 81, item_vtable.cpp) and gives it
    //  no address of its own.
    //
    //  A *pair* setter and not two single-field ones: each of those two
    //  call sites writes the field it is not changing straight back with
    //  the value it just read out, which is what passing a read-back
    //  argument through a two-argument setter looks like and what two
    //  independent setters never produce.
    void SetPair(CFloat lo, CFloat hi)
    {
        m_min = lo;
        m_max = hi;
        ((NotifyFn)(*(void***)this)[0])(this, &m_value, 1);
    }

    //  FloatMini's vtable slot 0 taken as a two-argument notify rather than
    //  as the destructor this reconstruction declares there: the shipped
    //  call pushes two arguments, which a destructor slot never does.
    //  Reached by raw offset for the same reason widget.h's own slots are.
    typedef void (__thiscall *NotifyFn)(void* self, CFloat* value, int flag);

    //  0x6F4772E0 - see floatmini_dump.cpp.
    void DumpTo(void* sink);

    //  CFloat, not the plain dwords this header used to call them, and a
    //  {min, max} pair rather than two anonymous words: FloatMiniB::DumpTo
    //  (0x6F4772E0, floatmini_dump.cpp) prints them as literally
    //  `val[%s] min[%s] max[%s]` from +0x04, +0x08 and +0x0C.  Three of
    //  CItem's vtable slots also hand them back through a CFloat
    //  out-buffer (75/76/77, item_vtable.cpp), which settles the type - and it is load-bearing for the two setters above: passing a
    //  *class* by value copies it through CFloat's own copy constructor,
    //  and that copy is what stops MSVC eliding the "write the field I am
    //  not changing back with the value it already had" store that both
    //  shipped setters really do emit.  With plain `unsigned int`
    //  parameters the self-store folds away and slot 81 comes out three
    //  instructions short.
    CFloat m_min;   // +0x08 (+0x5C in CItem) - g_unk6FAAE628
    CFloat m_max;   // +0x0C (+0x60 in CItem) - g_unk6FAAE624
};

#endif

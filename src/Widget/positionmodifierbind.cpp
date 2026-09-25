//============================================================================
//  0x6F47C6A0 - MovementModifier::Bind: build the module's own request
//  block, submit it through the same global maker CWidget's slot 68 uses,
//  adopt the handle pair of whatever came back, and tell this modifier's
//  own slot 7 about both ends.
//
//  Two things in the block are worth saying out loud.
//
//   * The three CFloat fields at +0x0C/+0x10/+0x14 are filled from
//     g_CFloatZero rather than from a literal zero.  The shipped code loads
//     dword_6FAAE470 into a register once and stores it three times, which
//     an `= CFloat()` would not do (that is `mov [x], 0`).  The value is
//     the same; the read is what the source has to spell.
//
//   * `m_mode` is -1 unless the subject is on the synchronised channel (bit
//     31 of its own flags word) or the caller asked for mode 2, in which
//     case it is -2.  The two tests share a single store, which is what the
//     `||` gives; two separate `if`s give two.
//
//  The subject is resolved and then checked-cast against the '+agl' tag as
//  a conditional expression, not as a second `if` - widgetpathref.cpp
//  carries the note on why that spelling is the one that produces
//  `setnz`/`sub 1`/`and`.  The `test [edi+14h]` that follows runs on the
//  possibly-null result in the shipped code too, so it is not guarded here
//  either.
//
//  Own translation unit: MovementModifier::Init reaches it with a real call.
//============================================================================
#include "positionmodifier.h"
#include "widgetagentquery.h"

//  '+agl' MSB first - see widgetpathref.cpp for the full note.
static const unsigned int kAgentTagPathable = 0x2B61676C;

void* MovementModifier::Bind(SModifierSubject* subject, int mode)
{
    CHandleObject* resolved = LookupHandle(subject->m_handle,
                                           subject->m_typeTag);
    //  An if/else with its own zero in each arm, not one pre-initialised
    //  variable: the shipped code here zeroes the checked pointer *inside*
    //  the null arm and jumps over the other one, which is what a plain
    //  if/else gives.  Its FloatProp twin (floatpropmodifierbind.cpp) has
    //  the other shape - a single zero hoisted above the test - and the two
    //  really do differ in the shipped image.
    CHandleObject* checked;
    if (resolved == 0)
        checked = 0;
    else
        checked = (resolved->m_kindTag == kAgentTagPathable) ? resolved : 0;

    //  Field order below is the shipped *store* order, not a tidy one:
    //  the three CFloats first (one load of g_CFloatZero, three stores),
    //  then the two tags, then the zeroes, then +0x28 ahead of +0x24 -
    //  the same "one register serves as the zero and is then turned into
    //  the -1" shape Position::RecordSlot8 (positionrecordbuild.cpp)
    //  already records for this family of blocks.
    SModifierRequest request;
    request.m_from       = g_CFloatZero;
    request.m_to         = g_CFloatZero;
    request.m_rate       = g_CFloatZero;
    request.m_tag        = 0x5E6D6F64;      // '^mod'
    request.m_typeId     = 0x6070766D;      // 'mvp`'
    request.m_reserved08 = 0;
    request.m_reserved18 = 0;
    request.m_reserved1C = 0;
    request.m_reserved20 = 0;
    request.m_reserved28 = -1;
    request.m_mode       = -1;

    if ((checked->m_flags & 0x80000000) != 0 || mode == 2)
        request.m_mode = -2;

    void* made = SubmitWidgetAgentQuery((SWidgetAgentQuery*)&request, 1, 1);
    if (made != 0)
    {
        m_handle  = ((SMadeModifier*)made)->m_handle;
        m_typeTag = ((SMadeModifier*)made)->m_typeTag;
    }

    //  Slot 7 (+0x1C): "you are bound now, here is the subject and here is
    //  what the maker built".  Both arguments pushed, `this` in ecx.
    typedef void (__thiscall *OnBoundFn)(void*, void*, void*);
    ((OnBoundFn)(*(void***)this)[0x1C / 4])(this, checked, made);

    return made;
}

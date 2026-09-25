//============================================================================
//  0x6F478B50 - FloatModifier::Bind.  The FloatProp copy of
//  MovementModifier::Bind (0x6F47C6A0, positionmodifierbind.cpp), which
//  carries the notes; only the differences are worth repeating.
//
//   * a different type id ('mcr`' rather than 'mvp`');
//   * +0x14 of the request block is a plain zero here, where the position
//     copy stores a third read of g_CFloatZero into it;
//   * the '+agl' checked cast is written the other way round - the shipped
//     code zeroes both registers first and only fills the checked one when
//     the resolve succeeded, which is one `cmp` against a register instead
//     of a second constant.
//
//  Own translation unit: FloatModifier::Init reaches it with a real call.
//============================================================================
#include "floatpropmodifier.h"
#include "widgetagentquery.h"

//  '+agl' MSB first - see widgetpathref.cpp.
static const unsigned int kAgentTagPathable = 0x2B61676C;

void* FloatModifier::Bind(SModifierSubject* subject, int mode)
{
    CHandleObject* resolved = LookupHandle(subject->m_handle,
                                           subject->m_typeTag);
    CHandleObject* checked = 0;
    if (resolved != 0)
        checked = (resolved->m_kindTag == kAgentTagPathable) ? resolved : 0;


    //  Shipped store order - see positionmodifierbind.cpp.
    SFloatModifierRequest request;
    request.m_from       = g_CFloatZero;
    request.m_to         = g_CFloatZero;
    request.m_tag        = 0x5E6D6F64;      // '^mod'
    request.m_typeId     = 0x6072636D;      // 'mcr`'
    request.m_reserved08 = 0;
    request.m_reserved14 = 0;
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

    typedef void (__thiscall *OnBoundFn)(void*, void*, void*);
    ((OnBoundFn)(*(void***)this)[0x1C / 4])(this, checked, made);

    return made;
}

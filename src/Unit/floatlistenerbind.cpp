//============================================================================
//  0x6F480E90 - FloatListener::Bind.  The FloatProp listener's copy of
//  FloatModifier::Bind (0x6F478B50, floatpropmodifierbind.cpp), which
//  carries the notes; only the differences are worth repeating.
//
//   * a different tag pair - '^lis'/'`rtl' rather than '^mod'/'mcr`' - and
//     one CFloat in the block rather than two, with +0x10 left a plain
//     zero (floatlistener.h);
//   * two extra parameters.  `msgId` is stamped onto the made object's own
//     +0x48 and `target` is not read here at all - it travels this far only
//     because FloatListener::Init hands the same four words to both this
//     and the subject's own registration call (floatlistenerinit.cpp);
//   * a tail the two modifier copies do not have: the made object is a
//     CAgentRelation and this takes a channel registration on it, at
//     g_unk6FAAE4A0 and time zero.  That is what makes it a *listener* -
//     the relation is what will notice the value moving.
//
//  Own translation unit: FloatListener::Init reaches it with a real call.
//============================================================================
#include "floatlistener.h"
#include "widgetagentquery.h"
#include "agentrelation.h"

//  '+agl' MSB first - see widgetpathref.cpp.
static const unsigned int kAgentTagPathable = 0x2B61676C;

//  dword_6FAAE4A0 - the encoded CFloat the registration is taken at.
//  Nothing in this call tree gives it a value, only that it is the
//  module's own constant; g_CFloatZero's neighbour in the same block.
extern const CFloat g_unk6FAAE4A0;

void* FloatListener::Bind(SModifierSubject* subject, unsigned int msgId,
                          void* /*target*/, int mode)
{
    CHandleObject* resolved = LookupHandle(subject->m_handle,
                                           subject->m_typeTag);
    CHandleObject* checked = 0;
    if (resolved != 0)
        checked = (resolved->m_kindTag == kAgentTagPathable) ? resolved : 0;

    //  Shipped store order - see positionmodifierbind.cpp.
    SFloatListenerRequest request;
    request.m_from       = g_CFloatZero;
    request.m_tag        = 0x5E6C6973;      // '^lis'
    request.m_typeId     = 0x6072746C;      // '`rtl'
    request.m_reserved08 = 0;
    request.m_reserved10 = 0;
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

    //  Slot 7 (+0x1C): "you are bound now, here is the subject and here is
    //  what the maker built".  Both arguments pushed, `this` in ecx.
    typedef void (__thiscall *OnBoundFn)(void*, void*, void*);
    ((OnBoundFn)(*(void***)this)[0x1C / 4])(this, checked, made);

    ((CAgentRelation*)made)->m_eventId = msgId;
    ((CAgentRelation*)made)->TakeRegistration(&g_unk6FAAE4A0, 0);

    return made;
}

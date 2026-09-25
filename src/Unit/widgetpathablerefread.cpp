//============================================================================
//  0x6F05F3A0 - the load-time counterpart of a plain '+agl'-tagged widget
//  reference: read the {handle, typeTag} pair back, resolve it, and only if
//  the resolved object is still tagged '+agl' (kAgentTagPathable - the same
//  finer-grained check floatlistenerbind.cpp/positionmodifierbind.cpp/
//  floatpropmodifierbind.cpp already use) hand its own +0x54 field to the
//  slot's type-checked assign (SCheckedWidgetSlot::Assign, 0x6F029770,
//  widgettypedassign.cpp) - the same object that field is on CUnit is
//  literally addressed as a SCheckedWidgetSlot here, one field (`m_value`)
//  wide, which is what makes reusing its own Assign body legal.
//
//  When the handle does not resolve, or resolves to something not tagged
//  '+agl', the slot is released and nulled *directly* - the shipped code
//  does not call Assign(0) for that path, it inlines the release exactly
//  the way Assign's own body would but without the extra call, so this is
//  spelled as two separate branches rather than one call with a computed
//  argument.
//
//  __fastcall(store in ecx, &slot in edx); returns store.
//
//  Own translation unit: SCheckedWidgetSlot::Assign is a real call out of
//  it, everything else is CDataStore readers already elsewhere.
//============================================================================
#include "cdatastore.h"
#include "agenttypedslots.h"
#include "game.h"

CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

//  '+agl' MSB first - see widgetpathref.cpp.
static const unsigned int kAgentTagPathable = 0x2B61676C;

CDataStore* __fastcall ReadWidgetPathableRef(CDataStore* store, CAgent** slot)
{
    unsigned int handle;
    unsigned int typeTag;
    store->ReadDwordAlt(&handle);
    store->ReadDwordAlt(&typeTag);

    CHandleObject* resolved = LookupHandle(handle, (int)typeTag);
    if (resolved != 0 && resolved->m_kindTag == (int)kAgentTagPathable)
    {
        CAgent* candidate = *(CAgent**)((char*)resolved + 0x54);
        ((SCheckedWidgetSlot*)slot)->Assign(candidate);
    }
    else
    {
        CAgent* old = *slot;
        if (old != 0)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        *slot = 0;
    }
    return store;
}

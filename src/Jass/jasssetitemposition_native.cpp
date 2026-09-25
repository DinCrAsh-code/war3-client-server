//============================================================================
//  0x6F3C4C80 - JASS_SetItemPosition, the native's own top-level entry
//  point (not CItem::JASS_SetItemPosition_callee1, which it always calls
//  last regardless of anything above it).  user_knowledge.json's own
//  ground truth gives this function's signature directly
//  (__cdecl(hItem, float* posX, float* posY)) and a hand-decompiled
//  pseudocode listing that this reconstruction follows closely, corrected
//  in two places against the raw disassembly: the final call passes
//  posY's own *pointer* through unchanged (not a dereferenced float - the
//  pseudocode's own `*(float*)&posY` is a decompiler artifact, not what
//  the bytes show), and the nested resolution chain's `this` for the
//  final CItem::JASS_SetItemPosition_callee1 call is always the object
//  ResolveItemHandle found (`v4`/edi), never the intermediate handle
//  QueryHandleField0x54 resolves (`v5`/esi) - the nested block only
//  decides *whether* to also call SItemHandleFieldObject::DispatchUnknown
//  along the way, not what gets passed to the final call.
//
//  Does this relate to CWidget::MoveTo/MoveForward? No - this chain never
//  reaches CWidget's own vtable at all. What it *does* establish: hItem
//  resolves through ResolveItemHandle to a CAgent* already type-checked
//  against 'item' (itemtypedassign.h), which is exactly the same
//  underlying object CItem::JASS_SetItemPosition_callee1 (jasssetitemposition.cpp)
//  operates on as `this` - the two functions are two ends of the same
//  JASS-native-to-CItem-method call, not independently-arrived-at objects.
//============================================================================
#include "itemhandlemain.h"
#include "itemhandleresolve.h"
#include "itemhandledispatch.h"
#include "item.h"

void __cdecl JASS_SetItemPosition(int hItem, const CFloat* posX, const CFloat* posY)
{
    CAgent* item = ResolveItemHandle(hItem);

    if (item)
    {
        SOptionalHandleRef* ref = (SOptionalHandleRef*)((char*)item + 0x8C);
        if ((ref->m_handle & ref->m_typeTag) != (unsigned int)-1)
        {
            CAgent* field = (CAgent*)QueryHandleField0x54(ref);
            if (field)
            {
                //  A second LookupHandle+magic-tag+pending-flag validation,
                //  the same three-step shape ResolveItemHandle's own
                //  second check uses (itemhandlemain.cpp) - `field` is
                //  read as a CAgent* here (its own m_handle/m_typeTag at
                //  +0xC/+0x10) purely to drive this lookup, then handed to
                //  DispatchUnknown as whatever SItemHandleFieldObject
                //  really is; both are the same object, read two ways for
                //  two different purposes, the same "same object, different
                //  declared type per use" shape this call tree already
                //  uses for `item` itself (CAgent* here, CItem* below).
                SItemHandleObject* handleObj = (SItemHandleObject*)LookupHandle(field->m_handle, field->m_typeTag);
                if (handleObj)
                {
                    SItemHandleObject* tagOk = (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
                    if (tagOk && tagOk->m_pendingFlag == 0)
                        ((SItemHandleFieldObject*)field)->DispatchUnknown(item);
                }
            }
        }
    }

    ((CItem*)item)->JASS_SetItemPosition_callee1(posX, posY, -1);
}

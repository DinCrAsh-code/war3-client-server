//============================================================================
//  0x6F2AF7E0 - CGhostImageMaskOwner::ClearGhostImageRenderMask.  `this`
//  in ecx, one stack word (`retn 4`).  Reached from
//  SAbilityHandleCarrier::NotifyOwnerOfGrant
//  (Unit/unit_abilitygrantflagnotify.cpp) with the owner as `this` and the
//  ability's own +0xCC flag word as the mask to clear.
//
//  Walks `this`'s own ghost-image array (populated by 0x6F2AF520, kept a
//  redirect - a second SEH-framed function with its own several-deep
//  unestablished closure of Storm registration calls, well outside this
//  batch's scope) the same way Widget/ghostimageslotarraydtoralt.cpp's own
//  TSGrowableArray<SGhostImagePtrSlotAlt> destructor walks: addref each
//  live element for the duration of the loop body, release it again at
//  the end (SGhostImagePtrSlotAlt::Release, same file).  A null slot
//  falls back to LookupHandle on two fixed low addresses (0xC/0x10) - not
//  a stack or member offset, the literal absolute addresses the shipped
//  `mov edx,[10h]` / `mov ecx,[0Ch]` read, confirmed off this address's
//  own raw bytes (`8B 15 10 00 00 00` / `8B 0D 0C 00 00 00`) - checked
//  against the same 'lga+' SItemHandleObject-shaped tag/gate every other
//  resolve in this closure uses.
//
//  Per live element: clear the caller's mask out of its own "mask low"
//  word (SItemRenderMaskFields::GetMaskLow/SetMaskLow,
//  Item/itemrendermaskflags.cpp) and OR the *inverted* mask into "mask
//  high" (OrMaskHigh) - not a typo of the low-word op, the shipped stream
//  really does use the same inverted value both ways - then re-derive the
//  element's own world position through the already-reconstructed
//  CPathRef::SmartPtrToWorldVec3 (Pathfinding/CPathRef.cpp) at its own
//  +0x30, discarding the result (nothing downstream reads the local it
//  writes into).
//============================================================================
#include "agent.h"
#include "storm.h"
#include "itemrendermaskflags.h"
#include "game.h"   // CPathRef::SmartPtrToWorldVec3

class CHandleObject;
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);   // Game/game.h

//  Re-declared to match Widget/ghostimageslotarraydtoralt.cpp's own
//  SGhostImagePtrSlotAlt exactly, so Release() and the array destructor
//  (0x6F2ADBA0, storm.h's own TSGrowableArray<T>::~TSGrowableArray, that
//  file's explicit instantiation) bind onto those real bodies.
struct SGhostImagePtrSlotAlt
{
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

//  The SItemHandleObject-shaped resolve target: only the tag/gate this
//  walk itself reads.
struct SGhostImageResolveTarget
{
    char         m_reserved00[0x0C];
    unsigned int m_kindTag;    // +0x0C - 'lga+' when it is one
    char         m_reserved10[0x20 - 0x10];
    int          m_gate;       // +0x20
};

struct CGhostImageMaskOwner
{
    void ClearGhostImageRenderMask(unsigned short mask);   // 0x6F2AF7E0

    //  0x6F2AF520 - populate the local array below with this owner's own
    //  live ghost images.  Its own SEH frame and several-deep
    //  unestablished closure put it outside this batch's own scope.
    void PopulateGhostImageArray(TSGrowableArray<SGhostImagePtrSlotAlt>* out);
};

void CGhostImageMaskOwner::ClearGhostImageRenderMask(unsigned short mask)
{
    TSGrowableArray<SGhostImagePtrSlotAlt> images = {0, 0, 0, 0};
    PopulateGhostImageArray(&images);

    for (unsigned int i = 0; i < images.m_count; i++)
    {
        CAgent* element = images[i].m_value;
        if (element != 0)
            element->m_refcount++;

        CAgent* resolved = element;
        if (resolved == 0)
        {
            //  Two fixed, absolute low addresses - not a stack/member
            //  offset.  See the file header.
            unsigned int handle = *(unsigned int*)(void*)0x0C;
            int          typeTag = *(int*)(void*)0x10;
            CHandleObject* obj = LookupHandle(handle, typeTag);
            if (obj != 0)
            {
                SGhostImageResolveTarget* candidate = (SGhostImageResolveTarget*)obj;
                if (candidate->m_kindTag == 0x2B61676Cu && candidate->m_gate == 0)
                    resolved = (CAgent*)candidate;
            }
        }

        if (resolved != 0)
        {
            SItemRenderMaskFields* fields = (SItemRenderMaskFields*)resolved;
            unsigned short inverted = (unsigned short)~mask;
            unsigned short low = fields->GetMaskLow();
            fields->SetMaskLow((unsigned short)(low & inverted));
            fields->OrMaskHigh(inverted);

            ((CPathRef*)((char*)resolved + 0x30))->SmartPtrToWorldVec3();
        }

        if (element != 0)
            images[i].Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F2AF520 - see the declaration above.  Kept a redirect: own
//  __except_handler4-shaped SEH frame, a Storm class-info registration
//  call and two more unnamed leaves, none of it in this batch's scope.
//----------------------------------------------------------------------------
__declspec(naked) void CGhostImageMaskOwner::PopulateGhostImageArray(TSGrowableArray<SGhostImagePtrSlotAlt>*)
{
    __asm { mov eax, 06F2AF520h }
    __asm { jmp eax }
}

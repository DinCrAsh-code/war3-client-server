//============================================================================
//  0x6F26B9E0 - CWidget's vtable slot 34 (+0x88).  See widget.h.
//
//  Slot 31's twin (widget_setmodel.cpp), taking a model *handle* instead of
//  a name, and with three differences that are all in the shipped code
//  rather than in the reading of it:
//
//   * the whole body is skipped when the handle is zero, including the
//     "create a sprite if there is none" step slot 31 does first;
//   * an optional visibility notify (slot 38, +0x98) runs before anything
//     else when the caller asks for it;
//   * the sprite comes from *slot 25* (+0x64), not from
//     CreateSpriteForDetailLevel - so it is the caller's business to have
//     one, and this slot only ever loads into it.
//
//  Own translation unit: eleven real calls.
//============================================================================
#include "widget.h"
#include "sprite.h"
#include "modeltokens.h"

void __fastcall ModelReleaseNotify();
int __fastcall GetModelDetailFlags(int wantsFullDetail);
void __fastcall LoadModelWithTokens(CSprite* self, unsigned int modelHandle,
                                    CTokenizer* animTokens,
                                    CTokenizer* attachmentTokens,
                                    CTokenizer* boneTokens, int detailFlags,
                                    unsigned int player, void* notify);

//  slot 38 (+0x98) - NotifyWidgetVisibilityChanged (widgetvisibility.cpp).
typedef void (__thiscall *Slot0x98Fn)(void* self);
//  slot 25 (+0x64) - CAgentWar3::Method_0x64 / CItem::Method_0x64.
typedef unsigned int (__thiscall *Slot0x64Fn)(void* self);
//  slots 35/36, called for the side effect only.
typedef float (__thiscall *QueryFloatFn)(void*, int, int, int);

//  0x6F4D3550 - "does this sprite's model carry detail flag 0x10"
//  (spritemodeldetail.cpp).  A thiscall member on the sprite with no stack
//  arguments, which is why the shipped stream can call it *between* two of
//  LoadModelWithTokens' own pushes without disturbing them.
struct SSpriteModelQuery
{
    int QueryModelDetailFlag();
};

void CWar3Image::SetModelByHandle(unsigned int modelHandle, int notify)
{
    if (modelHandle == 0)
        return;

    if (notify != 0)
        ((Slot0x98Fn)(*(void***)this)[0x98 / 4])(this);

    //  Two things here are not slot 31's (widget_setmodel.cpp) shape, and
    //  the shipped stream is explicit about both:
    //
    //   * the owning player is *this widget's own* slot 25 (+0x64), not the
    //     literal -1 slot 31 passes.  `push eax` right after that vtable
    //     call is the argument going in, and the value stays on the stack
    //     across the detail query below;
    //   * the detail flag is the sprite's own model flag - a thiscall on
    //     m_pSprite with no stack arguments (`mov ecx, [esi+28h]` /
    //     `call SSpriteModelQuery::QueryModelDetailFlag`), whose answer
    //     feeds GetModelDetailFlags.  Passing slot 25's return value there
    //     instead both loses that call and asks GetModelDetailFlags a
    //     different question.
    LoadModelWithTokens(m_pSprite, modelHandle,
                        GetAnimationTokenizer(),
                        GetAttachmentTokenizer(),
                        GetBoneTokenizer(),
                        GetModelDetailFlags(
                            ((SSpriteModelQuery*)m_pSprite)
                                ->QueryModelDetailFlag()),
                        ((Slot0x64Fn)(*(void***)this)[0x64 / 4])(this),
                        (void*)&ModelReleaseNotify);

    StoreOwningPlayer(((Slot0x64Fn)(*(void***)this)[0x64 / 4])(this),
                      (unsigned char)m_reserved24);

    ((QueryFloatFn)(*(void***)this)[0x8C / 4])(this, 4, 0, 0);
    StopSpriteAnimationsOfType(m_pSprite, 5);
}

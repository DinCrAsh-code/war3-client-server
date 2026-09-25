//============================================================================
//  0x6F2B9600 - SItemPowerupSource::ApplyPowerupEffect: `this` is the item
//  being consumed, not the ability inventory that called it.  Finds a
//  matching ability already on the recipient (or grants one for each
//  whitespace/dash/comma-separated token in the item type's own "ability
//  list" string field), refreshes visibility-mask bookkeeping, warns when
//  an item names more than four abilities or none at all, and - only for
//  the caller that passes a nonzero `arg4` - either notifies the recipient
//  and marks the item consumed (no ability was granted) or clears the
//  recipient's now-stale attachment slots (one was).  244 instructions on
//  its own, the largest piece of this batch.
//
//  Own translation unit: every callee not already established elsewhere in
//  this batch is a real out-of-line call with no dump of its own
//  (itemabilitybatch2_thunks.cpp) - some inside a loop, which rules out
//  inlining on its own.
//
//  Read close to the raw disassembly rather than restructured: three
//  different early-exit paths (ResolveChained's own optional immediate
//  vtable call, "no matching ability found", "the matched ability's own
//  powerup-acceptance field is zero") all land on the same tail
//  (loc_6F2B9907 in the dump) *without* the `ebp` scratch the main walk
//  uses ever having been pushed on those paths - a real asymmetry in the
//  shipped frame, not a transcription slip, so the early-outs are written
//  as `goto`s straight to that tail rather than folded into one guard.
//============================================================================
#include "unitinventory.h"
#include "itemhandlemain.h"
#include "item.h"
#include "CFloat.h"
#include "handleobject.h"        // SHandleHolder
#include "gameui.h"              // CGameWorld

//  0x6F0419C0 - SOptionalHandleRefResolver::ResolveChained, already
//  reconstructed (Agent/handlereref_resolve.cpp); redeclared here the way
//  every other caller of it does, with the identical signature - a member
//  taking `const void*` and returning `SOptionalHandleRef&`, not `void`.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  0x6F2834E0 - UnitVisibilityMaskOwner_6F2834E0: resolves the recipient's
//  own visibility-mask owner record.  Declared here the same way every
//  other caller of an already-named function redeclares it locally.
void* __fastcall UnitVisibilityMaskOwner_6F2834E0(void* recipient);

//  Already reconstructed elsewhere in this repo; redeclared locally with
//  each one's real signature (Net/packfourcc.cpp, Misc/misc_trivial_getters.cpp,
//  Unit/unit_findability.cpp).
int __fastcall UnpackFourCCString(unsigned int fourCC, char* out, int size);
unsigned int GetFourCC_6F0DB030();
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
    //  0x6F079CC0 - already a real naked thunk (Unit/unit_flagbit8thunks.cpp),
    //  redeclared here on the same class rather than inventing a second
    //  name for the address - verify.py's own address-to-name
    //  canonicalisation only tolerates one funcmap row per address, and a
    //  second one measurably cost score elsewhere in the tree.
    void RemoveAbility(void* ability);
};
void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int selector);
SHandleHolder* __fastcall ItemTypeGetHandleField98(unsigned int fourCC);
unsigned int __fastcall GetHandleOrZeroAlias(const SHandleHolder* self);
int __stdcall Storm_504(char** cursor, char* outBuffer, unsigned int bufSize,
                         const char* delims, int flag);
unsigned int __fastcall PackFourCCString(const char* text);
void __cdecl SLogWarningV(int level, const char* fmt, ...);
void* __fastcall QueryHandleField0x54(SOptionalHandleRef* ref);

struct SRecipientRefresh        { void __thiscall RefreshA(); void __thiscall RefreshB(); };
struct SAttachmentTarget        { void Show(int alsoTotal, int delta); };
void* __fastcall GrantAbilityToRecipient(void* recipient, unsigned int packedId,
                                          int arg4, int arg8, void* item);
//  0x6F3A35C0 - already CGameWorld::TestDetectionRelationBit
//  (GameUI/gameui.h, Game/gameworldvisiblerelation.cpp) from the concurrent
//  CUnit closure batch; redeclared here rather than under a second name -
//  the same address-canonicalisation issue this batch already hit once
//  (see the file header of itemabilitybatch2_thunks.cpp).
void __fastcall LogAbilityListWarning(int a, int b, int c);
struct SRecipientItemNotify     { void __thiscall NotifyItem(void* item); };
struct SItemFinalize            { void __thiscall Finalize(); };

//  0x6F2B9600's own receiver - a local, single-method stand-in the same way
//  every other helper struct above is, matching abilityapplypowerup.cpp's
//  and abilitystoreslot.cpp's own local declaration of it.
struct SItemPowerupSource
{
    int __thiscall ApplyPowerupEffect(void* recipient, int arg4, int arg8,
                                       int argC, int arg10);
};

int __thiscall SItemPowerupSource::ApplyPowerupEffect(void* recipient, int arg4,
                                                        int arg8, int argC, int arg10)
{
    void* item = this;

    ((SOptionalHandleRefResolver*)((char*)item + 0x8C))->ResolveChained(recipient);

    if (arg4 == 0)
    {
        typedef void (__thiscall *Slot33Fn)(void*);
        Slot33Fn slot33 = *(Slot33Fn*)(*(void***)item + 33);
        slot33(item);
    }

    if (!(*(unsigned int*)((char*)item + 0x20) & 0x100) &&
        !(*(unsigned int*)((char*)recipient + 0x5C) & 0x40000000))
    {
        void* mask = UnitVisibilityMaskOwner_6F2834E0(recipient);
        ++*(int*)((char*)mask + 0x254);
        *(unsigned int*)((char*)item + 0x20) |= 0x100;
    }

    char fourCCBuf[8];
    UnpackFourCCString(*(unsigned int*)((char*)item + 0x30), fourCCBuf, 8);
    unsigned int abilityFourCC = GetFourCC_6F0DB030();

    void* ability = ((SUnitAbilityHost*)recipient)->FindAbility(abilityFourCC, 0, 0, 1, 1);
    if (!ability)
        goto tail;

    {
        CFloat accept(kCFloatNoInit);
        int level = *(int*)((char*)ability + 0x50);
        CFloat* field = ((CAbilityInventory*)ability)->GetPowerupLevelField(&accept, level);
        if (*(const float*)field == *(const float*)&g_CFloatZero)
            goto tail;
    }

    {
        SHandleHolder* handle98 = ItemTypeGetHandleField98(*(unsigned int*)((char*)item + 0x30));
        unsigned int cursorStr = GetHandleOrZeroAlias(handle98);

        int minCharge = 0xDD;
        int grantedCount = 0;

        if (cursorStr != 0)
        {
            char* cursorPos = (char*)item + 0x9C;
            char* cursor = (char*)cursorStr;

            for (;;)
            {
                if (*cursor == 0)
                    break;

                if (grantedCount >= 4)
                {
                    SLogWarningV(4, "Item '%s' has more than %d abilities!", fourCCBuf, 4);
                    break;
                }

                char token[0x100];
                char* c = cursor;
                Storm_504(&c, token, 0x100, " ,-_\t", 0);
                cursor = c;

                if (token[0] != 0)
                {
                    unsigned int packedId = PackFourCCString(token);
                    ((SRecipientRefresh*)recipient)->RefreshA();

                    void* newAbility = GrantAbilityToRecipient(recipient, packedId, arg4, arg8, item);
                    ((SOptionalHandleRefResolver*)cursorPos)->ResolveChained(newAbility);
                    ((SRecipientRefresh*)recipient)->RefreshB();

                    if (newAbility)
                    {
                        if (arg10)
                        {
                            ((SAttachmentTarget*)newAbility)->Show(argC, arg10);

                            typedef int (__thiscall *SlotFn)(void*, int, int, int);
                            SlotFn slotAC = *(SlotFn*)(*(void***)newAbility + 43);
                            slotAC(newAbility, 1, argC, arg10);
                            SlotFn slotB0 = *(SlotFn*)(*(void***)newAbility + 44);
                            slotB0(newAbility, 1, argC, arg10);
                        }

                        if (arg4 != 0)
                        {
                            typedef int (__thiscall *SlotFn1)(void*, void*);
                            SlotFn1 slot98 = *(SlotFn1*)(*(void***)newAbility + 38);
                            int charge = slot98(newAbility, item);
                            if (!charge)
                            {
                                SlotFn1 slot9C = *(SlotFn1*)(*(void***)newAbility + 39);
                                slot9C(newAbility, item);
                            }
                            if (charge < minCharge)
                                minCharge = charge;
                        }

                        ++grantedCount;
                        cursorPos += 0xC;
                    }
                }

                if (!cursor)
                    break;
            }
        }

        if (grantedCount == 0 && (*(unsigned int*)((char*)item + 0x20) & 0x200))
        {
            SLogWarningV(4, "Usable item '%s' has no abilities!", fourCCBuf);
        }

        if (arg4 != 0)
        {
            if (grantedCount != 0 && minCharge != 0)
            {
                void* world = g_unk6FAB65F4;
                typedef int (__thiscall *QuerySlot25Fn)(void*);
                QuerySlot25Fn slot25 = *(QuerySlot25Fn*)(*(void***)recipient + 25);
                int slotResult = slot25(recipient);
                //  TestDetectionRelationBit(unsigned int player, int bit) is
                //  the confirmed real callee (retn 8, asm/sub_6F28DCF0's own
                //  dump of it), but this call site's own second argument was
                //  not independently pinned down - `slotResult` is the one
                //  value this call tree actually computes fresh here; `bit`
                //  is a placeholder.  ABI-safe regardless (a real two-`int`
                //  C++ call reproduces the shipped `retn 8` no matter what
                //  either value is), flagged in this function's own
                //  BEHAVIOUR entry as one of the specific pieces not
                //  confirmed against a second source.
                if (((CGameWorld*)world)->TestDetectionRelationBit(slotResult, 0))
                    LogAbilityListWarning(minCharge, 0, 0);

                int slotCount = *(int*)((char*)item + 0x98);
                if (slotCount != 0)
                {
                    unsigned int* slot = (unsigned int*)((char*)item + slotCount * 12 + 0x9C);
                    do
                    {
                        --slot;
                        --slot;
                        --slot;
                        --slotCount;
                        if ((slot[0] & slot[1]) != 0xFFFFFFFFu)
                        {
                            void* handleObj = QueryHandleField0x54((SOptionalHandleRef*)slot);
                            if (handleObj)
                            {
                                ((SUnitAbilityHost*)recipient)->RemoveAbility(handleObj);
                                slot[1] = 0xFFFFFFFF;
                                slot[0] = 0xFFFFFFFF;
                            }
                        }
                    } while (slotCount != 0);
                }
                *(unsigned int*)((char*)item + 0x8C) = 0xFFFFFFFF;
                *(unsigned int*)((char*)item + 0x90) = 0xFFFFFFFF;
            }
            else
            {
                ((SRecipientItemNotify*)recipient)->NotifyItem(item);

                typedef void (__thiscall *Slot54Fn)(void*, const CFloat*, int);
                Slot54Fn slot54 = *(Slot54Fn*)(*(void**)((char*)item + 0x54));
                slot54((char*)item + 0x54, &g_CFloatZero, 1);

                ((CItem*)item)->OnValueFloorReached();
            }
        }
    }

tail:
    if (!(*(unsigned int*)((char*)recipient + 0x5C) & 0x40000000) && arg4 == 0)
        ((SItemFinalize*)item)->Finalize();

    return 1;
}

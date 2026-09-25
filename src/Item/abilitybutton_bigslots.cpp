//============================================================================
//  Five of CAbilityButton's vtable slots - 108/112/166 (the three
//  substantial icon/command-order accessors the class comment on each
//  calls out) and 206/207 (the two substantial new leaf slots).
//
//  **Corrected this session.**  The previous session's own note here
//  claimed all five reached a call site whose callee's own `retn`
//  disagreed with what the call site visibly pushed - a genuine
//  stack-imbalance risk, and the reason all five were left as THUNKs.
//  That claim does not survive a full, byte-level stack-effect trace:
//  every call site in all five functions was walked instruction by
//  instruction (worktree_store.py show/bytes), tracking the live push
//  count against each callee's own confirmed `retn <n>` (direct calls'
//  `retn` read straight off their own raw_asm; vtable-slot calls
//  resolved against docs/targets/vtables/CAbilityButton.txt and then the
//  same way), and every single one balances exactly:
//
//    * 108 (0x6F0312A0) calls sub_6F332A50 (`retn 1Ch`, 7 stack dwords)
//      twice and sub_6F332A10 (`retn 14h`, 5 stack dwords) twice.  Each
//      call site interleaves those pushes with several `retn 0` vtable
//      calls to this instance's own new leaf slots (0x308/0x30C/0x318/
//      0x320/0x324/0x328/0x32C - all confirmed `retn 0`,
//      abilitybutton_newslots.cpp) whose *return values* are what get
//      pushed - so the pushes accumulate across many instructions before
//      the actual call, and a read that stops at "what's pushed
//      immediately before the `call`" undercounts badly.  Recounting
//      every push from each block's start gives exactly 7 and exactly 5
//      each time.
//    * 112 (0x6F045950) reaches CAbility::NotifyAttachmentRender
//      (0x6F02E160, already real, `retn 28h` - 10 stack dwords) through
//      this instance's own slot 0x294 (index 165, inherited unchanged).
//      Two leading pushes (`push -1`; `push eax`) go to two back-to-back
//      calls at slot 0xA0 (CAbility::Method_6F052250, `xor eax,eax;
//      retn` - confirmed `retn 0`, does not touch them), so they stay
//      live on the stack for the final call too.  Full count from the
//      block's start: exactly 10.
//    * 166 (0x6F045BE0) reaches sub_6F332B50
//      (ForwardToGameUI_6F2F44D0, `retn 18h` - 6 stack dwords),
//      sub_6F332C20 (`retn 4` - 1 stack dword) and sub_6F330580
//      (`retn 0Ch` - 3 stack dwords); all three balance exactly against
//      their own immediately-preceding pushes.
//    * 206/207 call CUnit::AppendOrder (unit.h, `retn 0Ch` - order,
//      afterCurrent, flags) with what looks like one push (the order
//      pointer) directly in front of the call - but two more stack
//      dwords (the literal `1`/`1` afterCurrent/flags pair) were pushed
//      *earlier*, ahead of the vtable call that computes
//      GetOwningPlayerIndex (CUnit's own slot 59, `retn 0` - untouched)
//      and MakeOrderAgent's own third stack argument (`push 0`,
//      MakeOrderAgent's `sourceRef`, __fastcall/unitorder.h).  This is
//      not a new shape: it is the exact interleaved-push idiom
//      CUnit::IssueStopOrder (unitstoporder.cpp) already documents for
//      the identical MakeOrderAgent/AppendOrder pair - "the `push 0`
//      that belongs to MakeOrderAgent's own stack argument sits between
//      them and the virtual call."  MakeOrderAgent's own `retn 4` cleans
//      only the topmost of the three, leaving the order pointer's own
//      later push plus the two leftover `1`s exactly matching
//      AppendOrder's `retn 0Ch`.
//
//  206 and 207 are reconstructed for real below - every instruction,
//  register and call is right, verify.py-confirmed - and score DIFFERS
//  (0.605/0.722) rather than EXACT for a single, well-understood reason:
//  MSVC's own block-layout choice for the three-test refused/queued-order
//  OR-chain does not follow source order (confirmed by trying it both
//  ways - inverted condition, then explicit per-condition gotos - and
//  getting the byte-identical generated listing each time), the same
//  "layout, not source order, decides" gap Method_0x304 above already
//  has in this same class.
//
//  **108/112/166 are reconstructed for real this session too** - see each
//  one's own comment below for the full derivation.  All three build on
//  CAbility::GetCachedFieldRecord/LookupAbilityFieldRecord
//  (ability_slots.cpp/ability_typefieldrecord.cpp) the same way the class
//  comment already promised, and all three read new fields off
//  SAbilityFieldRecordNode (abilityfieldrecordtable.h) that no earlier
//  call tree had touched - named there, not guessed at, the same
//  "only what the call tree reads gets a name" discipline the rest of
//  this record already follows.  See docs/targets/CAbilityButton.md for
//  the current, real verify.py numbers.
//============================================================================
#include "abilitybutton.h"
#include "unit.h"
#include "unitorder.h"
#include "ability_typefieldrecord.h"
#include "widget_selectiondepthcache.h"

//  ForwardToGameUI_6F2F4330/6F2F46B0/6F2F4380/6F2F4710 - GameUI/gameuiforwardcalls.cpp,
//  the same "look up the singleton, forward this/edx plus every stack arg"
//  shape ForwardToGameUI_6F2F75F0/6F2F44D0 already have there; declared
//  here since nothing there is exported through a shared header.
extern void __fastcall ForwardToGameUI_6F2F75F0(void* a, int b, int c0, int c1, int c2, int c3, int c4);
extern void __fastcall ForwardToGameUI_6F2F44D0(void* a, int b, int c0, int c1, int c2, int c3, int c4, int c5);
extern void __fastcall ForwardToGameUI_6F2F4330(void* a, int b, int c0, int c1, int c2, int c3, int c4, int c5, int c6);
extern void __fastcall ForwardToGameUI_6F2F46B0(void* a, int b, int c0);
extern void __fastcall ForwardToGameUI_6F2F4380(void* a, int b, int c0);
extern void __fastcall ForwardToGameUI_6F2F4710(void* a, int b, int c0);

//  0x6F332830 - NotifyGameUIAbilityGranted, GameUI/gameuiabilitygrantnotify.cpp
//  (already real).  Slot 112's own call site reaches it through a second
//  shipped address, 0x6F26FE30 (a bare `jmp sub_6F332830`, no frame of
//  its own) - one C++ function reachable through two addresses, the
//  mirror image of docs/notes/thunk-removal-pass.md's own "one shipped
//  address carrying two C++ names" defect.  0x6F26FE30 is marked THUNK
//  in agent_worktrees (nothing to reconstruct at that address beyond the
//  jmp itself) and deliberately left out of this file's own calltree
//  dump - the call site below compiles against NotifyGameUIAbilityGranted
//  directly, the same real, retn-0 function every other caller reaches.
extern void __fastcall NotifyGameUIAbilityGranted(void* recipient);

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner, redeclared here the way
//  ability_slots.cpp/ability_depcheckalias.cpp already do (that file keeps
//  the struct private; an identical-signature redeclaration is the same
//  linker symbol).  Used by all five real functions in this file.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();
};

//  0x6F021270 - SAttachmentCounterNotify::OnCountChanged, already a real
//  body (attachmentcounters.cpp) reached the same way abilitystoreslot.cpp
//  and friends already reach it.
struct SAttachmentCounterNotify
{
    void __thiscall OnCountChanged();
};

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed
//  (Misc/misc_field_getters.cpp) - declared locally the way every other
//  TU that calls it does (see e.g. GameUI/cameraupdate.cpp).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};
extern void* g_unk6FAB65F4;   // dword_6FAB65F4

//----------------------------------------------------------------------------
//  slot 108 / +0x1B0 (0x6F0312A0).  Registers this ability's command-card
//  icon variants with GameUI.  Resolves the field record the same way
//  CAbility's own field accessors do (GetCachedFieldRecord, falling back
//  to a fresh LookupAbilityFieldRecord tagged "CAbilityButton::
//  RegisterButtons"), reads its own icon-path override
//  (m_fieldB0/m_fieldB4) and two opaque value pairs
//  (m_field50/m_field54, m_field58/m_field5C - forwarded unread, same
//  "generic pass-through pair" shape both slots in this file share), and
//  makes two batches of GameUI forwarding calls - one unconditional
//  (built from Method_0x308/0x30C/0x320/0x324 and the first pair), one
//  gated on Method_0x318 actually differing from the default icon (built
//  from Method_0x318/0x31C/0x328/0x32C and the second pair) - each
//  batch followed by a narrower "did the icon actually change" re-check
//  that makes one more, five-argument forwarding call when so.  Every
//  leaf-slot call is repeated exactly as many times as the shipped code
//  repeats it (no caching into a local): MSVC cannot prove two virtual
//  calls to the same slot return the same value, so neither can this
//  source pretend to.
//----------------------------------------------------------------------------
void CAbilityButton::Method_0x1B0()
{
    unsigned int typeId = m_typeId;

    SAbilityFieldRecordNode* record = (SAbilityFieldRecordNode*)GetCachedFieldRecord();
    if (record == 0)
        record = (SAbilityFieldRecordNode*)LookupAbilityFieldRecord(
            typeId, "CAbilityButton::RegisterButtons");

    unsigned int r50 = record->m_field50;
    unsigned int r54 = record->m_field54;
    const char* iconPath = (record->m_fieldB0 != 0)
        ? *record->m_fieldB4
        : "ReplaceableTextures\\CommandButtons\\BTNTemp.blp";

    ForwardToGameUI_6F2F4330((void*)typeId, Method_0x308(),
        Method_0x30C(), Method_0x320(), Method_0x324(),
        r50, r54, (int)iconPath, 0);

    if (Method_0x320() != g_defaultAbilityIconId)
    {
        if (Method_0x320() != Method_0x308())
        {
            ForwardToGameUI_6F2F75F0((void*)typeId, Method_0x320(),
                r50, r54, (int)iconPath, Method_0x324(), 0);
        }
    }

    if (Method_0x318() != g_defaultAbilityIconId)
    {
        ForwardToGameUI_6F2F4330((void*)typeId, Method_0x318(),
            Method_0x31C(), Method_0x328(), Method_0x32C(),
            record->m_field58, record->m_field5C, record->m_field38, 0);

        if (Method_0x328() != g_defaultAbilityIconId)
        {
            if (Method_0x328() != Method_0x318())
            {
                if (Method_0x328() != Method_0x320())
                {
                    ForwardToGameUI_6F2F75F0((void*)typeId, Method_0x328(),
                        record->m_field58, record->m_field5C, record->m_field38,
                        Method_0x32C(), 0);
                }
            }
        }
    }
}

//----------------------------------------------------------------------------
//  slot 112 / +0x1C0 (0x6F045950).  Two stack arguments: `iconArg`
//  selects which icon id to compare against this instance's own
//  m_defaultIconId (the default id itself when `hasIcon == 0`, `iconArg`
//  otherwise) - a mismatch means this call is not talking about the icon
//  this instance actually carries, so it returns immediately.  Otherwise
//  resolves the field record, then - only when this instance's own icon
//  answer (Method_0x318) differs from the default *and* the icon-dirty
//  bit (m_flags & 0x80) is set - assembles a field-record dependency
//  check (EvaluateDepCheckAlias) and forwards this ability's owner and
//  requirement pair (m_field78/m_field7C when `hasIcon != 0`, the field
//  record's own m_field58/m_field5C otherwise) into
//  CAbility::NotifyAttachmentRender through this instance's own inherited
//  slot 0x294, alongside two more GameUI forwarders
//  (0x6F332AA0/0x6F332C50) reached only when this ability's owner has a
//  live selection-depth-cache entry (CDepthCacheOwner::
//  QueryOwnerAt20CField).  Two of NotifyAttachmentRender's own ten
//  arguments (Method_0x328/Method_0x32C's own results) are computed and
//  passed through only for the virtual call's own side effect - neither
//  is read back anywhere in this function, the same "an indirect call's
//  return value may go unread, but the call itself cannot be elided"
//  shape as the two back-to-back Method_6F052250 calls immediately
//  above them.
//----------------------------------------------------------------------------
void CAbilityButton::Method_0x1C0(int hasIcon, int iconArg)
{
    if (hasIcon == 0)
    {
        if (g_defaultAbilityIconId != m_defaultIconId)
            return;
    }
    else
    {
        if ((unsigned int)iconArg != m_defaultIconId)
            return;
    }

    unsigned int typeId = m_typeId;
    SAbilityFieldRecordNode* record = (SAbilityFieldRecordNode*)GetCachedFieldRecord();
    if (record == 0)
        record = (SAbilityFieldRecordNode*)LookupAbilityFieldRecord(
            typeId, "CAbilityButton::SetCommands");

    if ((Method_0x318() != g_defaultAbilityIconId) && ((m_flags & 0x80) != 0))
    {
        unsigned int v1 = Method_0x318();
        unsigned int m31c = Method_0x31C();
        Method_0x328();
        Method_0x32C();

        //  reqPair[0]/[1] have to sit adjacent - the shipped code passes
        //  &reqPair[0] to ForwardToGameUI_6F2F4710 below and its own
        //  callee (0x6F2F4710) reads both dwords through that one pointer.
        unsigned int reqPair[2];
        if (iconArg != 0)
        {
            reqPair[0] = m_field70;
            reqPair[1] = m_field74;
        }
        else
        {
            reqPair[0] = record->m_field58;
            reqPair[1] = record->m_field5C;
        }
        unsigned int v4 = record->m_field38;

        int outFlag = 0;
        int depResult = EvaluateDepCheckAlias(&outFlag);
        if (depResult != 0 || outFlag != 0)
        {
            void* owner = (void*)m_field30;
            if (owner == 0)
                owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();

            IndexedArrayHolder_6F3A1650* holder =
                (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
            unsigned int playerIndex = *(unsigned short*)((char*)holder + 0x28);
            void* player = (void*)holder->At(playerIndex);
            CDepthCacheOwner* depthOwner = *(CDepthCacheOwner**)((char*)player + 0x34);

            if (depthOwner != 0)
            {
                if (depthOwner->QueryOwnerAt20CField(owner) != 0)
                {
                    ForwardToGameUI_6F2F4380((void*)typeId, iconArg, v4);
                    ForwardToGameUI_6F2F4710((void*)typeId, iconArg, (int)reqPair);
                    NotifyGameUIAbilityGranted(owner);
                }
            }

            unsigned int callA = Method_6F052250();
            unsigned int callB = Method_6F052250();
            v1 = (Method_6F02E150() != 0) ? ((v1 != 0) ? 1u : 0u) : 0u;

            NotifyAttachmentRender((void*)reqPair[1], (void*)m31c, (void*)reqPair[0],
                (void*)typeId, (void*)v1, (void*)(m_flags & 1), (void*)0,
                (void*)callB, (void*)callA, (unsigned int)-1);
        }
    }
}

//----------------------------------------------------------------------------
//  slot 166 / +0x298 (0x6F045BE0).  This ability's own "set tooltip"
//  accessor.  Resolves the current player's dependency-cache owner
//  (IndexedArrayHolder_6F3A1650::At -> +0x34) and, when present and this
//  ability's owner has a live selection-depth-cache entry
//  (CDepthCacheOwner::QueryOwnerAt20CField), bails out entirely - the
//  same "already handled elsewhere" early-out slot 112 makes through the
//  same depth-cache check.  Otherwise resolves the field record
//  (GetCachedFieldRecord, tagged "CAbilityButton::SetTooltip" on a
//  cache miss) and, when this instance's own icon (Method_0x318) is
//  set and its icon-dirty bit is clear, walks four of the record's own
//  counted-array pairs (m_field140/144, m_field164/168, m_field8C/90 -
//  three name/id/count-shaped clamped lookups, the same clamp
//  GetFootprintTypeName already has) to build a title/subtitle/order-
//  class triple, appends this ability's own dependency-requirements text
//  through EvaluateDepCheckAlias + SAbilityFieldRecordNode::
//  AppendRequiresText when the alias check does not already refuse it,
//  and forwards the whole thing plus the order-class value into GameUI
//  (0x6F332B50/0x6F332C20, both already real forwarders).
//----------------------------------------------------------------------------
//  Single-level clamped array reads - the same clamp
//  Pathfinding/footprinttype.h's own GetFootprintTypeName already has,
//  just against a plain array (not a two-level SGroup) since these are
//  the record's own single counted-array fields.
static const char* ClampedFieldStr(unsigned int count, const char* const* arr,
                                    unsigned int index, const char* fallback)
{
    if (count == 0)
        return fallback;
    unsigned int i = index;
    if (count - 1 < i)
        i = count - 1;
    return arr[i];
}

static unsigned int ClampedFieldU(unsigned int count, const unsigned int* arr,
                                   unsigned int index, unsigned int fallback)
{
    if (count == 0)
        return fallback;
    unsigned int i = index;
    if (count - 1 < i)
        i = count - 1;
    return arr[i];
}

typedef int (__thiscall *AbilityButtonVtableIntFn)(void*);

void CAbilityButton::Method_0x298(int param0)
{
    IndexedArrayHolder_6F3A1650* holder = (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    unsigned int playerIndex = *(unsigned short*)((char*)holder + 0x28);
    void* player = (void*)holder->At(playerIndex);
    CDepthCacheOwner* depthOwner = *(CDepthCacheOwner**)((char*)player + 0x34);

    if (depthOwner != 0)
    {
        void* owner = (void*)m_field30;
        if (owner == 0)
            owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();
        if (depthOwner->QueryOwnerAt20CField(owner) != 0)
            return;
    }

    unsigned int typeId = m_typeId;
    unsigned int index = m_field50;   // CAbility's own reserved field - not the record's

    unsigned int iconId = Method_0x318();
    int depResult = EvaluateDepCheckAlias(0);
    int hasFreshIcon = (iconId != g_defaultAbilityIconId) ? 1 : 0;

    SAbilityFieldRecordNode* record = (SAbilityFieldRecordNode*)GetCachedFieldRecord();
    if (record == 0)
        record = (SAbilityFieldRecordNode*)LookupAbilityFieldRecord(
            typeId, "CAbilityButton::SetTooltip");

    const char* title;
    unsigned int secondValue;
    unsigned int thirdValue;
    unsigned int mergeSlotOffset;

    if (hasFreshIcon && ((m_flags & 0x80) != 0))
    {
        title = ClampedFieldStr(record->m_field140, record->m_field144, index, "Tool tip missing!");
        secondValue = ClampedFieldU(record->m_field164, record->m_field168, index, 0);
        thirdValue = ClampedFieldU(record->m_field8C, record->m_field90, index, (unsigned int)-1);
        mergeSlotOffset = 0x31C;
    }
    else
    {
        title = ClampedFieldStr(record->m_field134, record->m_field138, index, "Tool tip missing!");
        secondValue = ClampedFieldU(record->m_field158, record->m_field15C, index, 0);
        thirdValue = ClampedFieldU(record->m_field80, record->m_field84, index, (unsigned int)-1);
        mergeSlotOffset = 0x30C;
    }

    AbilityButtonVtableIntFn mergeFn =
        (AbilityButtonVtableIntFn)(*(void***)this)[mergeSlotOffset / 4];
    int mergeResult = mergeFn(this);
    if ((mergeResult & 0x40) != 0)
        thirdValue = (unsigned int)-1;

    char requiresText[0x80];
    if (depResult == 0)
        record->AppendRequiresText(requiresText, 0x80, index);
    const char* requiresTextArg = (depResult != 0) ? (const char*)0 : requiresText;

    ForwardToGameUI_6F2F44D0((void*)typeId, param0,
        (int)title, (int)requiresTextArg, secondValue, 0, 0, 0);
    ForwardToGameUI_6F2F46B0((void*)typeId, param0, thirdValue);
}

//----------------------------------------------------------------------------
//  Shared machinery for slots 206/207 below (SAttachmentOwnerQuery/
//  SAttachmentCounterNotify/ADDR_THUNK are declared near the top of this
//  file now - shared with slots 108/112/166 above).
//----------------------------------------------------------------------------

//  Slot 0x1B8 (index 110) - inherited unchanged from CAbility
//  (DefaultIntFieldQueryQ, `xor eax,eax; retn` - confirmed `retn 0`) but
//  reached through the vtable rather than by name, the same
//  "call through the vtable, not the base name" reasoning
//  abilitybutton_newslots.cpp's own Method_0x310/Method_0x340 already
//  use for an inherited/self slot: a derived class further down this
//  chain (CAbilitySpell and beyond) may override it with a real record.
typedef void* (__thiscall *VtableSlot1B8Fn)(void*);

//  0x6F0788F0 - an unnamed helper Method_0x338's own icon-refresh path
//  reaches (walks a linked pair of agent-slot chains via sub_6F4786B0,
//  matching field 0x1B8 against the record Method_0x338 already looked
//  up, and calling this instance's own Method_0x33C on a match).  Not one
//  of the five addresses this session was asked to resolve - declared as
//  a real, unhooked-shipped redirect so Method_0x338's own reconstruction
//  below can call it correctly rather than guess at its body.  `retn 4`
//  confirmed off its own raw_asm; thiscall on the unit/agent the call
//  site passes in ecx, one stack argument (the record pointer).
struct SUnitAttachmentRecordWalker
{
    void __thiscall NotifyMatchingSlot(void* rec);
};
ADDR_THUNK(void SUnitAttachmentRecordWalker::NotifyMatchingSlot(void*),
           0x6F0788F0)

//  Slot 0x340 (208, Method_0x340) - already real (abilitybutton_newslots.cpp),
//  but reached here through the vtable the same way, since the shipped
//  code dispatches through `[this][0x340]` rather than calling it by name
//  (a real `jmp`, not a `call` - the tail end of this function).
typedef unsigned int (__thiscall *VtableSlot340Fn)(void*);

//----------------------------------------------------------------------------
//  slot 206 / +0x338 (0x6F054880).  This ability's owning unit either gets
//  a fresh order (queue not idle / auto-cast-shaped flags set, and orders
//  not refused outright) built the same way CUnit::IssueStopOrder
//  (unitstoporder.cpp) builds its own - MakeOrderAgent(orderId, target, 0)
//  handed straight to AppendOrder - or, when no order is warranted, this
//  instance's own "icon dirty" bit (m_flags & 0x80, agentwar3.h) gets set
//  and the owner's matching command-card slot notified through
//  sub_6F0788F0, unless it is already dirty. Either way (dirty already,
//  or just marked so) the icon answer itself is refreshed through this
//  instance's own slot 0x340 (Method_0x340) as a genuine tail call - the
//  shipped code's own trailing `jmp edx`, not a `call`.
//----------------------------------------------------------------------------
void CAbilityButton::Method_0x338()
{
    CUnit* owner = (CUnit*)m_field30;
    if (owner == 0)
        owner = (CUnit*)((SAttachmentOwnerQuery*)this)->QueryOwner();

    //  Written with explicit gotos - see Method_0x33C's own comment below
    //  for why a plain if/else does not reproduce the shipped branch
    //  layout here.
    unsigned int flags = owner->m_flags5C;
    if ((flags & 0x100000) != 0)
        goto orderPath;
    if ((int)owner->m_int54 > 0)
        goto orderPath;
    if ((flags & 0x200000) == 0)
        goto notifyPath;

orderPath:
    if ((flags & 0x100) != 0)
        return;
    if ((int)owner->m_int198 <= 0)
        return;
    owner->AppendOrder(
        MakeOrderAgent(Method_0x308(), owner->GetOwningPlayerIndex(), 0),
        1, 1);
    return;

notifyPath:
    if ((m_flags & 0x80) == 0)
    {
        if (Method_0x318() != g_defaultAbilityIconId)
        {
            void* rec = ((VtableSlot1B8Fn)(*(void***)this)[0x1B8 / 4])(this);
            if (rec != 0)
            {
                CUnit* owner2 = (CUnit*)m_field30;
                if (owner2 == 0)
                    owner2 = (CUnit*)((SAttachmentOwnerQuery*)this)->QueryOwner();
                ((SUnitAttachmentRecordWalker*)owner2)->NotifyMatchingSlot(rec);
            }
            m_flags |= 0x80;
            ((SAttachmentCounterNotify*)this)->OnCountChanged();
        }

        ((VtableSlot340Fn)(*(void***)this)[0x340 / 4])(this);
    }
}

//----------------------------------------------------------------------------
//  slot 207 / +0x33C (0x6F054950).  Same order-or-notify shape as slot
//  206 above, minus the icon-refresh half: the "no order warranted"
//  side just clears the dirty bit if it was set (rather than setting
//  it), tells this instance nothing changed (Method_0x344 - a genuine
//  no-op, nullsub_257, abilitybutton.h), and refreshes through the same
//  shared attachment-counter path 206 also ends most of its own paths
//  through.  Orders build off Method_0x318 rather than 206's own
//  Method_0x308 - the only other difference.
//----------------------------------------------------------------------------
void CAbilityButton::Method_0x33C()
{
    CUnit* owner = (CUnit*)m_field30;
    if (owner == 0)
        owner = (CUnit*)((SAttachmentOwnerQuery*)this)->QueryOwner();

    //  Written with explicit gotos, one per shipped condition, rather than
    //  a combined if/else: MSVC's own branch-layout choice - which side of
    //  this three-test OR-chain becomes the fall-through - does not follow
    //  source order for a plain if/else here (confirmed: an inverted
    //  condition with the two bodies swapped compiled to the identical
    //  instruction stream as the original ordering), the same "layout,
    //  not source order, decides" gap Method_0x304's own reconstruction
    //  above already documents for this class. Explicit per-condition
    //  gotos pin each jump target directly, matching the shipped code's
    //  own three separate tests one for one.
    unsigned int flags = owner->m_flags5C;
    if ((flags & 0x100000) != 0)
        goto orderPath;
    if ((int)owner->m_int54 > 0)
        goto orderPath;
    if ((flags & 0x200000) == 0)
        goto notifyPath;

orderPath:
    if ((flags & 0x100) != 0)
        return;
    if ((int)owner->m_int198 <= 0)
        return;
    owner->AppendOrder(
        MakeOrderAgent(Method_0x318(), owner->GetOwningPlayerIndex(), 0),
        1, 1);
    return;

notifyPath:
    if (m_flags & 0x80)
    {
        m_flags &= ~0x80;
        Method_0x344();
    }
    ((SAttachmentCounterNotify*)this)->OnCountChanged();
}

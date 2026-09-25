#include "gamecontext.h"    // CGameDataContext, GetThreadLocalSlot, kThreadLocalGameData

//  Same Storm_578 (SStrPrintf-family) declaration every other caller in
//  src/ already uses (see e.g. Render/fontcreate.cpp, GameUI/chatfontvalue.cpp).
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  g_unk6FAB778C - see netcommand_dispatch.cpp's own comment (the
//  "tempest presence" host TempestProviderListHash walks).
extern void* g_unk6FAB778C;

unsigned int __fastcall TempestProviderListHash(void* presenceHost);

//  A single checksum-provider's own vtable-slot-0 hash method: `this` in
//  ecx, no other arguments, result in eax.  Matches
//  GameDataContextChecksumProviderHash's identical inline call shape
//  (gamecontext.h / netcommand_dispatch.cpp) - kept as its own small
//  static here (rather than shared) because /Ob2 inlines it back to the
//  identical `mov edx,[ecx] ; mov eax,[edx] ; call eax` at every one of
//  the twenty call sites below, which is what the dump itself repeats
//  twenty times rather than factoring into one out-of-line helper.
static unsigned int CallChecksumProviderSlot0(void* provider)
{
    typedef unsigned int (__thiscall *ProviderChecksumFn)(void*);
    ProviderChecksumFn fn = (ProviderChecksumFn)(*(void***)provider)[0];
    return fn(provider);
}

//============================================================================
//  0x6F5455A0 - ChecksumProviderDebugPrint: the checksum-provider debug
//  dump.  Confirmed genuinely dead code (zero static callers anywhere in
//  the binary) - see docs/notes/checksum-provider-registry.md for the
//  full investigation this function's own format string
//  (aSbuildUSnextPr, 0x6F958818) anchors: it names the seven checksum
//  categories Warcraft III's own real desync-report logs are publicly
//  known to print, and is what named the mechanism.
//
//  `Storm_578(dest, size, format, ...)` (same signature every other
//  caller in src/ already declares) is a tail call, and its first two
//  arguments are this function's own first two: `retn 0Ch` (3 stack
//  arguments) is `dest`, `size`, and `indent` - a prefix string
//  re-pushed for every one of the format string's nine `%s` conversions,
//  confirmed by the dump repeating the same `push eax` (the incoming
//  `indent`, loaded once into eax and never reloaded) nine times.
//
//  Reads dword_6FAB778C's own presence tag / birth tag pair off +0x40
//  (fallback +0x1C when that reads -1) and +0x50 - the same two fields
//  CNetData_FireTickNotification reads off the same global, and named
//  the OPPOSITE way round from that function's own `legs.birthTag`/
//  `legs.presenceTag` names: this dump's own variadic push order (the
//  cdecl right-to-left push means the *last*-printed of a line's values
//  pushes *first*) has the +0x50 field print as "next birth tag" and the
//  +0x40-or-+0x1C field print as "next presence tag", which is backwards
//  from how CNetData_FireTickNotification's own locals are named.  Left
//  as a note rather than a fix to that already-EXACT function: purely a
//  comment/local-name discrepancy, no instruction it emits depends on
//  which name is attached to which field.
//
//  Computes tempest (TempestProviderListHash over the same host) and then
//  walks the per-thread CGameDataContext's cache array
//  (context->m_pCache, gamecontext.h) as a flat pointer array, hard-coded
//  for exactly 20 slots (offsets 0x00-0x4C) with NO count check at all -
//  unlike GameDataContextChecksumProviderHash's own bounded walk through
//  m_providerCount.  Slots 0/1/2/3/4/6 are the six named categories
//  (cheat/combat/net/rand/unit/game); slot 5 (the lazily-created
//  CAbilityDatabase - Item/abilitygenericfieldrecord_thunks.cpp) is
//  called for its side effect and its result is thrown away, matching the
//  format string never printing an eighth checksum; slots 7-19 (the
//  remaining repurposed cache singletons, including m_field40 at +0x40
//  and CMiscCustom* at +0x4C) are walked too, also unstored - this is the
//  unbounded part of the walk that would, if this dead code ever ran
//  live, call vtable slot 0 on whatever those slots hold today as if
//  they were checksum providers.  context->m_pCache is re-read fresh
//  before every single slot access (never hoisted across a provider
//  call), the same defensive re-read GameDataContextChecksumProviderHash
//  and TempestProviderListHash both already do for the same reason.
//============================================================================
void __stdcall ChecksumProviderDebugPrint(char* dest, unsigned int size, const char* indent)
{
    void* host = g_unk6FAB778C;

    unsigned int fallbackOrHost = *(unsigned int*)((char*)host + 0x40);
    unsigned int presenceTag = (fallbackOrHost == 0xFFFFFFFFu)
                                    ? *(unsigned int*)((char*)host + 0x1C)
                                    : fallbackOrHost;
    unsigned int birthTag = *(unsigned int*)((char*)host + 0x50);

    unsigned int tempest = TempestProviderListHash(host);

    //  Every stored-result slot below is pre-zeroed and only conditionally
    //  overwritten (never a ternary) - matching the dump's own shape
    //  exactly: `cmp ecx,esi ; je <fallthrough>` with no jump-around,
    //  because the result local already reads 0 on the skip path.  A
    //  ternary here compiles to an explicit if/else with a jump around
    //  the call's result store, which is not what the dump does.
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    void* provider = *(void**)((char*)context->m_pCache + 0x00);
    unsigned int cheat = 0;
    if (provider)
        cheat = CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x04);
    unsigned int combat = 0;
    if (provider)
        combat = CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x08);
    unsigned int net = 0;
    if (provider)
        net = CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x0C);
    unsigned int rand_ = 0;
    if (provider)
        rand_ = CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x10);
    unsigned int unit = 0;
    if (provider)
        unit = CallChecksumProviderSlot0(provider);

    //  Slot 5 (+0x14) - the lazily-created CAbilityDatabase this cache
    //  array shares with the checksum-provider mechanism.  Called for its
    //  side effect only; the format string has no eighth checksum, so the
    //  result is never stored anywhere.
    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x14);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x18);
    unsigned int game = 0;
    if (provider)
        game = CallChecksumProviderSlot0(provider);

    //  Slots 7-19 (+0x1C..+0x4C) - the rest of the hard-coded 20-slot
    //  walk, unbounded by m_providerCount.  Every one of these is a
    //  repurposed cache singleton the live per-tick walk never reaches
    //  (m_field40 at +0x40, m_pMiscCustom at +0x4C among them); called
    //  for side effect only, nothing stored, exactly like slot 5 above.
    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x1C);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x20);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x24);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x28);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x2C);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x30);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x34);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x38);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x3C);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x40);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x44);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x48);
    if (provider)
        CallChecksumProviderSlot0(provider);

    context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    provider = *(void**)((char*)context->m_pCache + 0x4C);
    if (provider)
        CallChecksumProviderSlot0(provider);

    Storm_578(dest, size,
              "%sbuild %u\n%snext presence tag %05d next birth tag %05d\n"
              "%stempest checksum %08x\n%scheat checksum %08x\n"
              "%scombat checksum %08x\n%snet checksum %08x\n"
              "%srand checksum %08x\n%sunit checksum %08x\n%sgame checksum %08x",
              indent, 0x17AB,
              indent, presenceTag, birthTag,
              indent, tempest,
              indent, cheat,
              indent, combat,
              indent, net,
              indent, rand_,
              indent, unit,
              indent, game);
}

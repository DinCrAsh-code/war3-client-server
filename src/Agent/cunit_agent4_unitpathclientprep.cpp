//============================================================================
//  0x6F495960 - lazily build `this`'s (a CUnit's) own CPathClient at +0xA8
//  (Unit/unit.h's own `m_pRefA8` - already established there as
//  "refcounted, released by ~CUnit"; this is its lazy-build path) if it is
//  not already there, otherwise bail out early when the one stack argument
//  is zero. Once the client exists: clear its ignore-node (a setter this
//  agent's own worklist does not reach, out of scope), clear its target
//  node (CPathClient::SetTargetNode, already reconstructed), hand it a
//  fixed sentinel route (CPathClient::SetRoute, already reconstructed),
//  release its lane (CPathClient::ReleaseLane, already reconstructed) and
//  OR bit 0x100000 into its own +0x88 flags word (CPathClient::m_flags,
//  pathmove.h).
//============================================================================
#include "unit.h"
#include "pathmove.h"

//  0x6F48BFD0 - out of this agent's own worklist; a naked redirect to the
//  real, unhooked body (src/Item/abilityfieldflagclosure_thunks.cpp's own
//  established convention). `retn 4` read off its own raw_asm.
#define AGENT4_THUNK(sig, addr)         \
    __declspec(naked) sig               \
    {                                    \
        __asm { mov eax, addr }         \
        __asm { jmp eax }                \
    }

//  ecx = &(CUnit's own +0xA8 slot), edx = 0 (a real argument, not garbage -
//  the call site sets it explicitly), one stack argument = 1.
AGENT4_THUNK(void __fastcall LazyBuildPathClient_6F48BFD0(
        void** slot, int zero, int flag), 0x6F48BFD0)

//  0x6F49AD20 - ecx = the client, one stack argument stored straight into
//  its own +0xA0 (CPathClient::m_ignore, pathmove.h) - out of this agent's
//  own worklist.  edx is never touched at this call site (a __thiscall
//  shape, not __fastcall's own dummy-edx-parameter idiom), and this
//  toolchain rejects __thiscall on a free function
//  (src/Item/abilityfieldflagclosure_thunks.cpp's own established note),
//  so it is spelled as a member of a tiny opaque type sharing CPathClient's
//  layout instead - the same STargetFlagIndexArray idiom that file uses
//  for the identical reason.
class SAgent4PathClientIgnoreSetter
{
public:
    void __thiscall SetIgnoreNode(CGridRegistration* node);
};
AGENT4_THUNK(void __thiscall SAgent4PathClientIgnoreSetter::SetIgnoreNode(
        CGridRegistration*), 0x6F49AD20)

//  dword_6FAB743C - a fixed grid coordinate this function fills both words
//  of a stack CGridVec2 with before calling SetRoute; not otherwise
//  identified.
extern int g_pathClientRouteSentinelCoord;   // dword_6FAB743C

//  `*slot` is re-read before every call below rather than kept in one
//  local - that is what the shipped stream itself does (a fresh
//  `mov ecx,[esi]`/`mov esi,[esi]` ahead of each one), not a stylistic
//  choice.
void __fastcall PrepareUnitPathClient(void* unit, void* /*unused edx*/,
                                      int forceRebuild)
{
    void** slot = (void**)((char*)unit + 0xA8);

    if (*slot == 0)
    {
        LazyBuildPathClient_6F48BFD0(slot, 0, 1);
    }
    else if (forceRebuild == 0)
    {
        return;
    }

    ((SAgent4PathClientIgnoreSetter*)*slot)->SetIgnoreNode(
        (CGridRegistration*)*(void**)((char*)unit + 0x98));
    ((CPathClient*)*slot)->SetTargetNode(0);

    //  Copied as a raw bit pattern, not converted through the FPU - the
    //  dump's own two stores are plain integer `mov`s, and CFloat's own
    //  `explicit CFloat(int bits)` constructor is exactly that copy
    //  (Math/CFloat.h).
    CFloat coord((int)g_pathClientRouteSentinelCoord);
    CGridVec2 sentinel(coord, coord);
    ((CPathClient*)*slot)->SetRoute(&sentinel, 1);

    //  The one place this function keeps `*slot` in a local rather than
    //  re-reading it - the shipped stream's own `mov esi,[esi]` loads it
    //  once and reuses that register for both ReleaseLane's `this` and the
    //  final flags OR.
    CPathClient* client = (CPathClient*)*slot;
    client->ReleaseLane();
    client->m_flags |= 0x100000;
}

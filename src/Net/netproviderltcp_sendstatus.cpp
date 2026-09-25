//============================================================================
//  SendStatusTarget::PostSendStatus, 0x6F65AB30.  Kept in its own
//  translation unit, separate from its only caller
//  (Net::NetProviderLTCP::FlushPendingSend, netproviderltcp_send.cpp) -
//  CLAUDE.md's own house rule prefers a second TU over
//  `__declspec(noinline)` for exactly this: both live in the same address
//  neighbourhood/module, but the shipped code keeps this a real, un-inlined
//  `call sub_6F65AB30`, and compiling the two together let this build's own
//  /Ob2 fold the call away, costing FlushPendingSend its own 1.000 EXACT.
//============================================================================
#include <windows.h>

//  0x6F658210 - a fourth, distinct status-post primitive (`__stdcall`, a
//  fixed discriminator and a pointer to the caller's own on-stack `code`
//  value - not the same shape as the PostStatusTarget family's own
//  fixed-offset writes), reached only from PostSendStatus below and
//  genuinely below this session's scope; `retn 8` read off the fact that
//  PostSendStatus's own caller pushes the two dwords and then does nothing
//  but `retn 4` for its own frame - no cleanup left for a cdecl callee to
//  need.  Naked redirect - argument count only, not per-argument meaning.
extern "C" __declspec(naked) void __stdcall
PostStatusRecord(int kind, int* codePtr)
{
    __asm { mov eax, 0x6F658210 }
    __asm { jmp eax }
}

//  0x6F65AB30 - a fourth, distinct status-post helper: forwards `code`
//  (spilled to its own stack slot so PostStatusRecord can take its address)
//  and the fixed kind 3 into PostStatusRecord above.  `this` is passed in
//  `ecx` per the thiscall declaration but the shipped body never reads it.
class SendStatusTarget
{
public:
    void PostSendStatus(int code);
};
void SendStatusTarget::PostSendStatus(int code)
{
    PostStatusRecord(3, &code);
}

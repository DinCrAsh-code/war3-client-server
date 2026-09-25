//============================================================================
//  0x6F4A9C70 - the driven relation's per-update entry point.
//
//  Update asks the arrival test first and does one of two completely
//  different things with the answer: at the target it lets the channel
//  registration go and tells endpoint A it has arrived, short of it, it
//  recomputes and returns.  The announcement is the same 11-dword context
//  the rest of this module sends, except that its *first* two words are not
//  the module's own '^rel' identity - they are '^lis' and '`rtl', and the
//  second of those is also the event code.
//============================================================================
#include "agentrelation.h"
#include "agenttickrelay.h"

typedef void* (__thiscall *RelationFireFn)(void* self, SAgentTickEventCtx* ctx);

//----------------------------------------------------------------------------
//  0x6F4A9C70
//----------------------------------------------------------------------------
//  Written naked.  The C++ it replaces is
//
//      if (AtTarget(value) == 0) goto recompute;   // arrival first, so it
//      ReleaseRegistration();                      //   is the fall-through
//      SAgentTickEventCtx ctx;
//      ctx.field0 = (void*)0x5E6C6973;      //  '^lis'
//      ctx.field4 = (void*)0x6072746C;      //  '`rtl'
//      ctx.eventCode = 0x6072746C;
//      ctx.agent = (CAgent*)this;
//      ctx.field10 = 0;  ctx.field14 = 0;  ctx.field18 = 0;
//      ctx.field1C = 0;  ctx.field20 = 0;
//      ctx.field28 = -1; ctx.field24 = -1;
//      if (m_endpointA == 0) return;
//      void* target = m_endpointA;
//      void* vtable = *(void**)target;
//      RelationFireFn fn = *(RelationFireFn*)((char*)vtable + 0x20);
//      fn(target, &ctx);
//      return;
//  recompute:
//      Recompute();
//
//  - same two paths, same context, same slot 0x20 dispatch - and the eleven
//  it could not reach are block layout and one cached load: VC8 puts the
//  recompute call inline behind a `jne` and the whole announcement after
//  it (source order is supposed to decide that, and here it does not once
//  the arrival path is this much bigger), and it keeps m_endpointA in ecx
//  across the block where the shipped code re-reads it after the branch.
//  Neither is source-addressable, so the thirty-nine are transcribed.
__declspec(naked) void CDrivenRelation::Update(const CFloat* /*value*/)
{
    __asm
    {
        mov     eax, [esp+4]                // value
        sub     esp, 2Ch                    // the announcement block
        push    esi
        push    eax
        mov     esi, ecx
        call    CDrivenRelation::AtTarget
        test    eax, eax
        mov     ecx, esi
        jz      short recompute
        call    CAgentRelation::ReleaseRegistration
        mov     eax, 6072746Ch              // '`rtl', the identity and the
        mov     [esp+8], eax                //   event code alike
        mov     [esp+0Ch], eax
        xor     eax, eax
        or      ecx, 0FFFFFFFFh
        cmp     [esi+30h], eax              // m_endpointA
        mov     dword ptr [esp+4], 5E6C6973h    // '^lis'
        mov     [esp+10h], esi              // subject: this
        mov     [esp+14h], eax
        mov     [esp+18h], eax
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        mov     [esp+24h], eax
        mov     [esp+2Ch], ecx
        mov     [esp+28h], ecx
        jz      short done
        mov     ecx, [esi+30h]
        mov     edx, [ecx]
        mov     edx, [edx+20h]
        lea     eax, [esp+4]
        push    eax
        call    edx
        pop     esi
        add     esp, 2Ch
        retn    4
    recompute:
        call    CDrivenRelation::Recompute
    done:
        pop     esi
        add     esp, 2Ch
        retn    4
    }
}

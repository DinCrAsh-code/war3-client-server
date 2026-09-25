//============================================================================
//  0x6F4A5F40 / 0x6F4A5FA0 / 0x6F4A6010 - the relation's three event
//  senders.
//
//  All three build the same 11-dword SAgentTickEventCtx the rest of the
//  agent system uses (agenttickrelay.h) and hand it to a receiver's own
//  vtable slot 0x20.  They differ only in who receives it and what the
//  context names as the subject:
//
//      0x6F4A5F40   endpoint A (+0x30)   subject = this, marker 0
//      0x6F4A5FA0   endpoint B (+0x40)   subject = this, marker 1
//      0x6F4A6010   this itself          subject = null,  marker 0
//
//  A null receiver is a silent no-op in all three, and none of them returns
//  what the receiver returned.
//
//  The first two words are the sender's identity - 0x5E72656C, `'^rel'` -
//  and are the same in all three, so they are written as one constant
//  stored twice, which is what the shipped `mov eax, 5E72656Ch` / two stores
//  does.
//============================================================================
#include "agentrelation.h"
#include "agenttickrelay.h"

//  All three bodies are __declspec(naked) transcriptions now (see each
//  one's own note).  The C++ they replace shared one static helper,
//
//      typedef void* (__thiscall *RelationFireFn)(void*, SAgentTickEventCtx*);
//      static const unsigned int kRelationSource = 0x5E72656C;
//      static void FillContext(SAgentTickEventCtx* ctx, unsigned int code,
//                              CAgentRelation* subject, int marker,
//                              void* detail, void* extra)
//      {
//          ctx->field0 = ctx->field4 = (void*)kRelationSource;
//          ctx->eventCode = code;   ctx->agent   = (CAgent*)subject;
//          ctx->field10 = (void*)marker;
//          ctx->field14 = detail;   ctx->field18 = extra;
//          ctx->field1C = 0;        ctx->field20 = 0;
//          ctx->field28 = -1;       ctx->field24 = -1;
//      }
//
//  which the compiler inlined into each sender exactly as the shipped code
//  does - the whole difference in all three was where one store or one
//  field read landed in the block.  It is kept here as a comment rather
//  than as dead code with no caller.

//----------------------------------------------------------------------------
//  0x6F4A5F40
//----------------------------------------------------------------------------
//
//  Written naked.  The C++ it replaces is
//
//      SAgentTickEventCtx ctx;
//      FillContext(&ctx, code, this, 0, detail, extra);
//      if (m_endpointA == 0) return;
//      void* target = m_endpointA;
//      void* vtable = *(void**)target;
//      RelationFireFn fn = *(RelationFireFn*)((char*)vtable + 0x20);
//      fn(target, &ctx);
//
//  and it produces this stream with the endpoint read scheduled
//  differently: the shipped build tests the field in memory
//  (`cmp [ecx+30h], eax`) and re-reads it after the branch, where VC8 loads
//  it once into a register and tests that.  One extra load of a field
//  nothing between the two writes, so the two agree for every input; it is
//  the compiler's choice of where to keep the value, which no source
//  spelling reaches (a local copy is what this source already has).  So the
//  twenty-eight instructions are transcribed; the dispatch is the same slot
//  0x20 with the same single pushed argument RelationFireFn declares.
__declspec(naked) void CAgentRelation::PostToEndpointA(unsigned int /*code*/,
                                                       void* /*detail*/,
                                                       void* /*extra*/)
{
    __asm
    {
        sub     esp, 2Ch
        mov     edx, [esp+34h]              // detail
        mov     eax, 5E72656Ch              // '^rel', written twice
        mov     [esp], eax
        mov     [esp+4], eax
        mov     eax, [esp+30h]              // code
        mov     [esp+14h], edx
        mov     edx, [esp+38h]              // extra
        mov     [esp+8], eax
        xor     eax, eax
        mov     [esp+18h], edx
        or      edx, 0FFFFFFFFh
        cmp     [ecx+30h], eax              // m_endpointA
        mov     [esp+0Ch], ecx              // subject: this
        mov     [esp+10h], eax              // marker 0
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        mov     [esp+28h], edx
        mov     [esp+24h], edx
        jz      short done
        mov     ecx, [ecx+30h]
        mov     eax, [ecx]
        mov     eax, [eax+20h]
        lea     edx, [esp]
        push    edx
        call    eax
    done:
        add     esp, 2Ch
        retn    0Ch
    }
}

//----------------------------------------------------------------------------
//  0x6F4A5FA0
//----------------------------------------------------------------------------
//
//  Written naked for the same reason as endpoint A above - the same
//  re-read of the endpoint field the shipped build does - and it is the
//  same body against +0x40 with marker 1, which the shipped code stores as
//  an immediate rather than out of a register (`mov [esp+10h], 1`) because
//  the zero it would otherwise share is not this sender's marker.  The
//  register roles rotate with it: eax carries `extra` and then the -1, edx
//  the zero.
__declspec(naked) void CAgentRelation::PostToEndpointB(unsigned int /*code*/,
                                                       void* /*detail*/,
                                                       void* /*extra*/)
{
    __asm
    {
        sub     esp, 2Ch
        mov     edx, [esp+34h]              // detail
        mov     eax, 5E72656Ch
        mov     [esp], eax
        mov     [esp+4], eax
        mov     eax, [esp+30h]              // code
        mov     [esp+8], eax
        mov     eax, [esp+38h]              // extra
        mov     [esp+14h], edx
        xor     edx, edx
        mov     [esp+18h], eax
        or      eax, 0FFFFFFFFh
        cmp     [ecx+40h], edx              // m_endpointB
        mov     [esp+0Ch], ecx              // subject: this
        mov     dword ptr [esp+10h], 1      // marker 1
        mov     [esp+1Ch], edx
        mov     [esp+20h], edx
        mov     [esp+28h], eax
        mov     [esp+24h], eax
        jz      short done
        mov     ecx, [ecx+40h]
        mov     edx, [ecx]
        mov     edx, [edx+20h]
        lea     eax, [esp]
        push    eax
        call    edx
    done:
        add     esp, 2Ch
        retn    0Ch
    }
}

//----------------------------------------------------------------------------
//  0x6F4A6010
//----------------------------------------------------------------------------
//
//  Written naked.  The C++ it replaces is
//
//      SAgentTickEventCtx ctx;
//      FillContext(&ctx, code, 0, 0, detail, 0);
//      void* vtable = *(void**)this;
//      RelationFireFn fn = *(RelationFireFn*)((char*)vtable + 0x20);
//      fn(this, &ctx);
//
//  - the same FillContext its two siblings above use - and it produces this
//  stream with one store rescheduled: the shipped build sinks the
//  `ctx.field14 = detail` store below the two vtable loads, where VC8 keeps
//  it with the rest of the block.  Nothing between them reads either
//  location, and no statement order in the source moves a store across two
//  loads it does not depend on, so the twenty-four instructions are
//  transcribed.  The dispatch is the same slot 0x20 with the same one
//  pushed argument RelationFireFn declares.
__declspec(naked) void CAgentRelation::PostToSelf(unsigned int /*code*/,
                                                  void* /*detail*/)
{
    __asm
    {
        sub     esp, 2Ch                    // the 11-dword context
        mov     edx, [esp+34h]              // detail
        mov     eax, 5E72656Ch              // '^rel', written twice
        mov     [esp], eax
        mov     [esp+4], eax
        mov     eax, [esp+30h]              // code
        mov     [esp+8], eax
        xor     eax, eax
        mov     [esp+0Ch], eax              // subject: null, unlike the two
        mov     [esp+10h], eax              //   endpoint senders
        mov     [esp+18h], eax
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        or      eax, 0FFFFFFFFh
        mov     [esp+28h], eax
        mov     [esp+24h], eax
        mov     eax, [ecx]                  // this's own vtable ...
        mov     eax, [eax+20h]              //   ... slot 0x20
        mov     [esp+14h], edx
        lea     edx, [esp]
        push    edx
        call    eax
        add     esp, 2Ch
        retn    8
    }
}

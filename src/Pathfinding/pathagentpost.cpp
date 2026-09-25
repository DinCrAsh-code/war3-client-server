//============================================================================
//  0x6F495530 / 0x6F495600 / 0x6F495660 - three fixed-event senders on the
//  moving agent, called from the pathfinder with `g_pPathMover` in ecx.
//
//  Each is CAgentTickRelay::FireEvent (agenttickrelay_tick.cpp) with the
//  resolve step gone and the event code baked in: build the same 11-dword
//  SAgentTickEventCtx, name this object as its subject, and hand it to this
//  object's own +0x30 event-router sub-object through vtable slot 0x20 -
//  the same router CAgent's slot-6 handler dispatches out of, which is what
//  ties these three codes to CAgent::NotifyMessage65/67/68 (agent_notify.cpp)
//  at the far end.
//
//  The two leading words are the *sender's* identity rather than the event's
//  and are the same in all three; only the third word differs, which is why
//  these are three functions and not one with an argument.
//
//  Nothing is done with the router's return value and nothing is returned:
//  a null router is simply a no-op, so the whole body sits under one test.
//============================================================================
#include "agent.h"
#include "agenttickrelay.h"

//  All three bodies are __declspec(naked) transcriptions now.  The C++
//  they replace - one shared helper the compiler inlined into each of
//  them, exactly as the shipped code does - is kept here as a comment
//  rather than as dead code with no caller:
//
//  typedef void* (__thiscall *AgentSub30FireFn)(void* self, SAgentTickEventCtx* ctx);
//
//  //  The sender identity these three stamp into the context's first two words.
//  const unsigned int kPathMoverSourceA = 0x5E70726F;
//  const unsigned int kPathMoverSourceB = 0x60706375;
//
//  static void PostPathEvent(CAgent* self, unsigned int eventCode)
//  {
//      SAgentTickEventCtx ctx;
//      ctx.field0 = (void*)kPathMoverSourceA;
//      ctx.field4 = (void*)kPathMoverSourceB;
//      ctx.eventCode = eventCode;
//      ctx.agent = self;
//      ctx.field10 = 0;
//      ctx.field14 = 0;
//      ctx.field18 = 0;
//      ctx.field1C = 0;
//      ctx.field20 = 0;
//      //  field28 before field24: the shipped store order, not a typo.
//      ctx.field28 = -1;
//      ctx.field24 = -1;
//
//      //  Read twice on purpose - the shipped code compares the router slot
//      //  against null and then loads it again rather than keeping the first
//      //  read, which is what its `cmp [ecx+30h],eax` / `mov ecx,[ecx+30h]`
//      //  pair says.
//      if (*(void**)((char*)self + 0x30) == 0)
//          return;
//
//      void* sub = *(void**)((char*)self + 0x30);
//      void* vtable = *(void**)sub;
//      AgentSub30FireFn fn = *(AgentSub30FireFn*)((char*)vtable + 0x20);
//      fn(sub, &ctx);
//  }

//  Written naked, and the C++ it replaces is the `PostPathEvent(this,
//  0x63702661)` above.  Everything in that helper is in this stream in the
//  shipped order; the four instructions it could not reach are the ones the
//  "read twice on purpose" comment is about - VC8 common-subexpressions the
//  two reads of the +0x30 router back into one register load and tests
//  that, which also drags the subject store above the compare.  A second
//  read of a field nothing writes in between is the same value either way,
//  so this is scheduling, not behaviour; it is simply not spellable, since
//  the source already asks for the second read.
__declspec(naked) void CAgent::PostMessage65()
{
    __asm
    {
        sub     esp, 2Ch
        xor     eax, eax
        or      edx, 0FFFFFFFFh
        cmp     [ecx+30h], eax              // the +0x30 event router
        mov     dword ptr [esp], 5E70726Fh  // sender identity, two words
        mov     dword ptr [esp+4], 60706375h
        mov     dword ptr [esp+8], 63702661h    // the event code
        mov     [esp+0Ch], ecx              // subject: this
        mov     [esp+10h], eax
        mov     [esp+14h], eax
        mov     [esp+18h], eax
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        mov     [esp+28h], edx              // +0x28 before +0x24
        mov     [esp+24h], edx
        jz      short done
        mov     ecx, [ecx+30h]
        mov     eax, [ecx]
        mov     eax, [eax+20h]              // slot 0x20
        lea     edx, [esp]
        push    edx
        call    eax
    done:
        add     esp, 2Ch
        retn
    }
}

//  The same body as PostMessage65 above, and naked for the same reason;
//  only the event code differs.
__declspec(naked) void CAgent::PostMessage67()
{
    __asm
    {
        sub     esp, 2Ch
        xor     eax, eax
        or      edx, 0FFFFFFFFh
        cmp     [ecx+30h], eax              // the +0x30 event router
        mov     dword ptr [esp], 5E70726Fh  // sender identity, two words
        mov     dword ptr [esp+4], 60706375h
        mov     dword ptr [esp+8], 63702677h    // the event code
        mov     [esp+0Ch], ecx              // subject: this
        mov     [esp+10h], eax
        mov     [esp+14h], eax
        mov     [esp+18h], eax
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        mov     [esp+28h], edx              // +0x28 before +0x24
        mov     [esp+24h], edx
        jz      short done
        mov     ecx, [ecx+30h]
        mov     eax, [ecx]
        mov     eax, [eax+20h]              // slot 0x20
        lea     edx, [esp]
        push    edx
        call    eax
    done:
        add     esp, 2Ch
        retn
    }
}

//  The same body again; only the event code differs.
__declspec(naked) void CAgent::PostMessage68()
{
    __asm
    {
        sub     esp, 2Ch
        xor     eax, eax
        or      edx, 0FFFFFFFFh
        cmp     [ecx+30h], eax              // the +0x30 event router
        mov     dword ptr [esp], 5E70726Fh  // sender identity, two words
        mov     dword ptr [esp+4], 60706375h
        mov     dword ptr [esp+8], 63702678h    // the event code
        mov     [esp+0Ch], ecx              // subject: this
        mov     [esp+10h], eax
        mov     [esp+14h], eax
        mov     [esp+18h], eax
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        mov     [esp+28h], edx              // +0x28 before +0x24
        mov     [esp+24h], edx
        jz      short done
        mov     ecx, [ecx+30h]
        mov     eax, [ecx]
        mov     eax, [eax+20h]              // slot 0x20
        lea     edx, [esp]
        push    edx
        call    eax
    done:
        add     esp, 2Ch
        retn
    }
}

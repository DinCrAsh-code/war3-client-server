//============================================================================
//  0x6F48BD50 / 0x6F48BDB0 - NIpse::CPoFgLis::ComputeChecksum/DumpDebugState,
//  vtable slots 15/16. Unlike Agent/cpolklis.h's own CPoLkLis (the family's
//  one exception, which inherits both slots unchanged), this class DOES
//  override them for real, folding its own +0x50 mask field into each.
//============================================================================
#include "cpofglis.h"
#include "agentrelation.h"
#include "textsink.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F48BD50 - ComputeChecksum. Calls the base's own
//  CAgentRelation::ComputeChecksum(hash) first, then folds m_mask's own four
//  bytes into `*hash`, one byte at a time, through a `(b*0x7A2Dh) ^ b` mix
//  per byte (the same idiom Agent/agentbaseabscomputechecksum.cpp's own
//  `AgentBaseAbsByteFold` already names for a sibling checksum fold, though
//  NOT via that shared `AgentBaseAbsFoldDword` shape: this call tree stores
//  the running total back to `*hash` twice - once after the top byte alone,
//  once at the very end - rather than once at the end of a single local
//  accumulation) - byte3 first (folded straight into `*hash`), then byte2,
//  byte0 and byte1 (each mixed independently and summed together with the
//  updated `*hash` before the final store).
//
//  Written naked, and TWO rewrites as ordinary C++ were tried and abandoned
//  before settling here - both scored WORSE (0.794 and 0.324) than this
//  naked transcription's 0.882, because forcing the shipped code's real
//  store-then-reload of `*hash` through a `volatile` cast (needed at all,
//  since a plain round trip lets the optimiser prove the two folds
//  commute and merge them into one local sum, discarding the shipped
//  two-store shape entirely) also changes exactly which of two independent,
//  purely-local byte extractions the optimiser schedules before the
//  volatile read - the C++ mandates no order between them, and every
//  spelling tried moved a different, unrelated instruction instead of
//  fixing just the one this session was chasing. Byte-identical to the
//  shipped code either way; the remaining 4/34 gap under the naked
//  transcription is not semantic at all, and not a transcription defect
//  under CLAUDE.md's own naked-body rule either, once traced (see below) -
//  it comes from a mismatch between two DIFFERENT tools' *listing*
//  conventions for the identical encoded bytes, not from anything this
//  source spells wrong.
//
//  **The imul finding**: every one of the 4 mismatches is the same
//  `imul reg, imm32` instruction, rendered 3-operand by MASM's own
//  `__asm`-block listing (`imul edi, edi, 31277`) against the identical
//  bytes' 2-operand IDA-style rendering the target dump uses
//  (`imul edi, 31277`). Confirmed via an UNRELATED existing naked body with
//  the identical shape (`Render/optionalslot98dispatch.cpp`'s own
//  `Thunk_0x6F501CF0`, already `DIFFERS` at 18/19 for this exact reason) and
//  via the compiler's OWN code generator - for the exact same
//  `(b*0x7A2Du)^b` bytes, from real C++ arithmetic rather than `__asm` -
//  consistently choosing the 2-operand spelling instead
//  (`agentbaseabscomputechecksum.cpp`'s own build listing). So this is a
//  genuine, provable "different tool, different mnemonic spelling, same
//  bytes" gap - not a register/scheduling excuse, and not fixable by
//  re-spelling the naked `imul` any other way (tried).
//----------------------------------------------------------------------------
__declspec(naked) void CPoFgLis::ComputeChecksum(unsigned int* /*hash*/)
{
    __asm
    {
        push    esi
        mov     esi, [esp+8]
        push    edi
        push    esi
        mov     edi, ecx
        call    CAgentRelation::ComputeChecksum
        mov     ecx, [edi+50h]
        mov     eax, ecx
        shr     eax, 10h
        movzx   edx, ah
        mov     edi, edx
        imul    edi, 7A2Dh
        xor     edi, edx
        add     [esi], edi
        mov     edx, [esi]
        movzx   eax, al
        mov     edi, eax
        imul    edi, 7A2Dh
        xor     edi, eax
        lea     eax, [edi+edx]
        movzx   edx, ch
        movzx   ecx, cl
        mov     edi, ecx
        imul    edi, 7A2Dh
        xor     edi, ecx
        mov     ecx, edx
        imul    ecx, 7A2Dh
        add     edi, eax
        xor     ecx, edx
        add     ecx, edi
        pop     edi
        mov     [esi], ecx
        pop     esi
        retn    4
    }
}

//  IDA's own (unresolved) name for the shared empty-string global,
//  0x6F87529C - see Agent/rlproperty_dump.cpp's own header comment (same
//  precedent) and Window/messageboxprompt.cpp's own file header for why
//  every includer keeps its own file-local copy rather than a shared DATA
//  declaration.
static const char ValueName[] = "";

//----------------------------------------------------------------------------
//  0x6F48BDB0 - DumpDebugState. Base DumpDebugState, then an empty line, a
//  "[FgLis]" tag, then this object's own m_mask as "\tmsk[0x%X]" - same
//  three-call LogFormatted shape Agent/rlproperty_dump.cpp's own
//  CRlProperty::DumpDebugState already establishes.
//----------------------------------------------------------------------------
void CPoFgLis::DumpDebugState(void* sink)
{
    ((CAgentRelation*)this)->DumpDebugState(sink);

    LogFormatted(sink, ValueName);
    LogFormatted(sink, "[FgLis]");
    LogFormatted(sink, "\tmsk[0x%X]", m_mask);
}

}  // namespace NIpse

//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67D270. Same guarded-list lookup shape
//  as the rest of this batch, then the SAME array-indexed inner search
//  (record's own +0x114 base / +0x11C head, the node's own +4 giving the
//  next raw address rather than a further index - the ListWalker idiom
//  w3gsaction_tier10.cpp's own header documents and w3gsaction_tier12.cpp's
//  own 0x6F67AC60 already reuses) keyed by a byte the field validator
//  parses into a fresh local buffer (not the caller's own arg8 slot, since
//  this validator's own signature writes 5+ bytes: one key byte at offset
//  0, one dword clamp-candidate at offset 1). On a match, clamps the
//  candidate's own +0x88 dword to at least the validator's parsed value and
//  at most 0xAFC8 (stored twice in the shipped body - once unconditionally
//  with the raw max, once more only when the clamp actually fires,
//  overwriting with the literal ceiling - both stores kept here rather
//  than folded into one, to match), then, only when the candidate's own
//  +0x80 bit 1 is set, fires the not-in-scope trigger Trigger_6F67BC20
//  with a genuinely hidden second argument: the caller leaves the just-
//  matched candidate pointer sitting in edx from the search loop, and the
//  shipped body's own call site never reloads it - the same class of
//  hidden-register-argument thunk_inventory's own notes warn about,
//  caught here by reading the real callee's own register use rather than
//  trusting the call site's visible pushes alone.
//============================================================================

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

struct GameRecord9
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x114 - 0x5C];
    char* m_field114;           // +0x114 (inner array base)
    unsigned int m_field118;    // +0x118 (padding to reach 0x11C - not read)
    int m_field11C;              // +0x11C (inner list head)
    char m_reserved120[0x130 - 0x120];
    int m_state;                // +0x130
};

//  0x6F675870 - fastcall(ecx, edx, stack ptr), retn 4. Same plain-SEH-
//  plus-/GS-cookie `CDataStore` byte-read shape as every other
//  FieldValidator_ in this batch, this one writing at least 5 bytes into
//  its own out-buffer (a byte, then a dword one byte past it) - out of
//  scope, unreproducible under this build's fixed /GS-.
extern "C" __declspec(naked) int __fastcall FieldValidator_6F675870(unsigned int, unsigned int, void*)
{
    __asm { mov eax, 0x6F675870 }
    __asm { jmp eax }
}

//  0x6F67A960 - fastcall(this, edx), plain `retn`. netremainder2 session
//  (batch-H closure pass): this file's own naked thunk was NOT a second
//  reconstruction target - it is the exact same address
//  w3gsaction_tier13.cpp already reconstructs for real as `Sub_6F67A960`
//  (that file's own header explains the field layout), just spelled with
//  a different letter-case (`sub_6F67A960` here vs `Sub_6F67A960` there).
//  Both are distinct link symbols despite naming the same shipped address
//  (`thunk_inventory`'s own "one shipped address, two C++ names" defect
//  class - see docs/notes/thunk-removal-pass.md), so thunk_inventory.py
//  carried this one as a still-open READY item even though its real body
//  already exists elsewhere. w3gsaction_tier15.cpp already gets this
//  right (forward-declares `Sub_6F67A960` rather than re-defining it under
//  a second decorated name); this file now does the same, and Trigger_
//  6F67BC20's own naked `call` below is retargeted to the real symbol.
extern "C" void __fastcall Sub_6F67A960(void*, unsigned int);
//  0x6F673710 - ClearField0x16C_6F673710, already reconstructed for real
//  in src/Misc/trivialaccessors_06.cpp (see playertable_installhandler.cpp's
//  own note on its two siblings, 0x6F673750/0x6F673790).
void __fastcall ClearField0x16C_6F673710(void* self);
//  0x6F65B080 - Trigger_6F67BC20's own tail dispatch: builds an 8-byte
//  local out of `*(int*)arg` and `*(int*)(arg+4)` and forwards it into a
//  further undumped callee. Not one of this batch's own worklist
//  addresses - out of scope. `__fastcall(ecx=?, stack arg)`, confirmed
//  `retn 4` off its own dump (one stack argument).
extern "C" __declspec(naked) void __fastcall sub_6F65B080(void*, void*)
{
    __asm { mov eax, 0x6F65B080 }
    __asm { jmp eax }
}

//  0x6F67BC20 - fastcall(this=record, candidate) plus ONE stack argument
//  (record->m_field58), retn 4. The shipped call site never reloads
//  either register argument before `call` - ecx still holds the record
//  from the earlier `mov eax,[ecx+58h]`, edx still holds the just-matched
//  candidate from the search loop - so this declaration carries both
//  explicitly rather than leaving edx to arrive by coincidence.
//  batch-J-w3gsaction-tiers: reconstructed for real below - its own body
//  has no SEH frame, only the two not-in-scope callees above.
__declspec(naked) void __fastcall Trigger_6F67BC20(void* record, void* candidate, void* field58)
{
    __asm
    {
        sub     esp, 8
        push    esi
        push    edi
        mov     edi, edx
        and     dword ptr [edi+80h], 0FFFFFFFDh
        mov     esi, ecx
        add     dword ptr [esi+23Ch], 0FFFFFFFFh
        jnz     short skipReset
        mov     edx, 7
        call    Sub_6F67A960
        mov     ecx, esi
        call    ClearField0x16C_6F673710
    skipReset:
        mov     ecx, [edi+88h]
        mov     al, [edi+0B4h]
        lea     edx, [esp+8]
        mov     [esp+0Ch], ecx
        mov     ecx, [esp+14h]
        push    edx
        mov     [esp+0Ch], al
        call    sub_6F65B080
        pop     edi
        pop     esi
        add     esp, 8
        retn    4
    }
}

//  0x6F67D270 - case (see w3gsactionhandler2.cpp). On a record match
//  whose own state == 8: search the +0x11C inner list for a candidate
//  whose own +0xB4 byte matches the validator's parsed key byte, clamp
//  its own +0x88 dword into [validator's parsed dword, 0xAFC8], and, only
//  when its own +0x80 bit 1 is set, fire Trigger_6F67BC20 and report
//  status 2; every other outcome (no key, no record, wrong state, no
//  inner-list match, or a match whose own bit 1 is clear) reports 0.
int __stdcall W3GSAction2_6F67D270(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[8];

    if (!FieldValidator_6F675870(arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord9* rec = (GameRecord9*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord9*)rec->m_nextRecord;

        if (rec && rec->m_state == 8)
        {
            unsigned char keyByte = buf[0];
            int index = (int)MaskedPositive(rec->m_field11C);
            while (index > 0)
            {
                char* candidateBase = (char*)index;
                if (*(unsigned char*)(candidateBase + 0xB4) == keyByte)
                {
                    unsigned int validated = *(unsigned int*)(buf + 1);
                    unsigned int clamped = *(unsigned int*)(candidateBase + 0x88);
                    if (clamped <= validated)
                        clamped = validated;
                    *(unsigned int*)(candidateBase + 0x88) = clamped;
                    if (clamped >= 0xAFC8)
                    {
                        clamped = 0xAFC8;
                        *(unsigned int*)(candidateBase + 0x88) = clamped;
                    }

                    if (*(unsigned char*)(candidateBase + 0x80) & 2)
                    {
                        Trigger_6F67BC20(rec, candidateBase, rec->m_field58);
                        status = 2;
                    }
                    break;
                }

                char* next = rec->m_field114 + index;
                index = *(int*)(next + 4);
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}

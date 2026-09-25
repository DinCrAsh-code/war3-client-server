//============================================================================
//  batch-B-w3gsaction_thunks - two more opcode handlers (0x6F66E240,
//  0x6F66E430) sharing the SAME `g_actionQueueTable`/`g_actionQueueSignal`
//  infrastructure GameAction_0x26_PacketHandler's own reconstruction
//  (gameaction0x26_handler.cpp) already establishes: validate one field via
//  an already-reconstructed ParseXAction-family reader
//  (w3gsaction_parsers.cpp), then - only when self+0x40 is set -
//  FindOrCreateRecord this session's own queue record and, under a couple
//  of state/flag gates, run a one-shot per-record action and report status
//  2 instead of the default 0; always SignalOne the shared condition
//  afterwards (skipped only when wasCreated came back exactly -1, the same
//  sentinel gameaction0x26_handler.cpp's own `wasCreated != -1` guard
//  checks).
//
//  Both trigger actions (sub_6F6697D0/sub_6F66C470+sub_6F66DB40) are not
//  addresses this batch's own worklist entry names - out of this session's
//  scope, same whole-function naked-redirect reasoning already established
//  elsewhere in this batch (w3gsaction_6f66c4b0.cpp's own Method_6F66A140).
//============================================================================

struct ConditionVariable
{
    void SignalOne(unsigned int value);   // sub_6F6D8950
};
extern "C" unsigned int g_actionQueueSignal;   // unk_6FACFF20

struct ActionQueueTable
{
    void* FindOrCreateRecord(void* key, unsigned int arg2, int* wasCreated,
                              unsigned int arg4, unsigned int createIfMissing);
};
extern ActionQueueTable g_actionQueueTable;   // stru_6FACFF00

//  The queue record, extended past gameaction0x26_handler.cpp's own
//  ActionQueueRecord with the two extra fields these two handlers read/
//  write (+0xCC a sub-object passed as `this` to Sub_6F6697D0, +0xD4 a
//  session-key comparison field) - offsets exact, everything between left
//  as reserved padding since nothing here reads it.
struct ActionQueueRecord2
{
    char m_reservedCC[0xCC];
    char m_subObjectCC[0x4];   // +0xCC, passed as `this` to Sub_6F6697D0
    char m_reservedD0[0xD4 - 0xD0];
    unsigned int m_field0xD4;  // +0xD4
    char m_reservedD8[0xE4 - 0xD8];
    int  m_state;              // +0xE4
    char m_reservedE8[0x400];  // generous pad; nothing past +0xE4 is read here
};

int __fastcall ParseNoFieldsAction_6F666350(void* src, unsigned int declaredLength, void* out);
int __fastcall ParseNoFieldsAction_6F666610(void* src, unsigned int declaredLength, void* out);

//  0x6F6697D0 - fastcall(this, int* arg), plain `retn` (no stack args).
extern "C" __declspec(naked) void __fastcall Sub_6F6697D0(void*, unsigned int*)
{
    __asm { mov eax, 0x6F6697D0 }
    __asm { jmp eax }
}

//  0x6F669470 - Sub_6F66C470's own callee: real `__except_handler4`-shaped
//  SEH frame this build's fixed /GS- /EHs-c- cannot reproduce (confirmed
//  off its own 92-instruction dump). `__fastcall(this, edx)`, `retn` bare
//  (0) - thunked.
extern "C" __declspec(naked) void __fastcall sub_6F669470(void*, unsigned int)
{
    __asm { mov eax, 0x6F669470 }
    __asm { jmp eax }
}

//  0x6F66C470 - fastcall(this=record, edx=candidate), plain `retn`.
//  batch-J-w3gsaction-tiers: reconstructed for real below (its own body is
//  a plain flag-latch gate, no SEH frame) - only its own callee
//  sub_6F669470 stays a thunk.
__declspec(naked) void __fastcall Sub_6F66C470(void* record, void* candidate)
{
    __asm
    {
        push    ecx
        mov     eax, [edx+80h]
        test    eax, 10000h
        jnz     short skip
        test    al, 1
        jz      short skip
        or      eax, 10000h
        mov     [edx+80h], eax
        mov     al, [edx+0B4h]
        add     ecx, 0CCh
        lea     edx, [esp+3]
        mov     byte ptr [esp+3], al
        call    sub_6F669470
    skip:
        pop     ecx
        retn
    }
}

//  0x6F66DB40 (344 instructions) - a deep drain/teardown of the same
//  ActionQueueRecord2-shaped object (self+0x1EC array walk, self+0xCC
//  sub-object teardown, a Storm_403 type-descriptor-name assert on a
//  non-empty array), reaching five further undumped callees
//  (sub_6F66C880, sub_6F6641E0, sub_6F6696B0, sub_6F66BDA0 - none
//  reached anywhere else in this tree). Genuinely well beyond this
//  session's own scope at this size - a thunk. `retn` bare (0):
//  `__fastcall(this)`.
extern "C" __declspec(naked) void __fastcall Sub_6F66DB40(void*)
{
    __asm { mov eax, 0x6F66DB40 }
    __asm { jmp eax }
}

static int NormalizeBool(int v)
{
    //  `cmp eax,1 / sbb eax,eax / add eax,1` - 1 when v==1... actually this
    //  is the shipped body's own boolean-normalise idiom applied to
    //  `wasCreated`: 0 stays 0 (not-created), any nonzero value becomes 1.
    return v != 0;
}

//  Shared SignalOne call both handlers make at the end - a tiny wrapper so
//  each site reads identically to the shipped `push eax(normalized
//  wasCreated) / call ConditionVariable::SignalOne`.
static void g_actionQueueTable_Signal(int wasCreated)
{
    ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)NormalizeBool(wasCreated));
}

//  0x6F66E240 - case (see w3gsactionhandler2.cpp). No-fields validator
//  0x6F666350 (already reconstructed, w3gsaction_parsers.cpp); on success,
//  find-or-create this session's record and, only the first time state < 2
//  AND self matches the record's own +0xD4 key, run Sub_6F6697D0 and stamp
//  state = 2; the shared self+0x80 bit 0x400000 skips straight to
//  status 2 without any of that (matches gameaction0x26_handler.cpp's own
//  identical flag gate).
int __stdcall W3GSAction_6F66E240(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    void* unusedOut;
    if (!ParseNoFieldsAction_6F666350((void*)arg4, arg8, &unusedOut))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    ActionQueueRecord2* record = (ActionQueueRecord2*)
        g_actionQueueTable.FindOrCreateRecord(key, 1, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        if (*(unsigned int*)((char*)self + 0x80) & 0x400000)
        {
            status = 2;
        }
        else if (record->m_state < 2 && (void*)record->m_field0xD4 == key)
        {
            record->m_state = 2;
            Sub_6F6697D0(record->m_subObjectCC, &arg8);
            status = 2;
        }

        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}

//  0x6F66E430 - same shape as 0x6F66E240 with a different field
//  (ParseNoFieldsAction_6F666610) and a different trigger condition: state
//  must be exactly 3 and self+0x80 bit 0 clear, in which case bit 0 of
//  self+0x80 is set (latched) and BOTH Sub_6F66C470 and Sub_6F66DB40 run
//  (one per line in the dump) before status becomes 2. The self+0x80 bit
//  0x400000 fast path here reports status 2 immediately too, same as
//  0x6F66E240's own.
int __stdcall W3GSAction_6F66E430(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    void* unusedOut;
    if (!ParseNoFieldsAction_6F666610((void*)arg4, arg8, &unusedOut))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    int wasCreated = 0;
    ActionQueueRecord2* record = (ActionQueueRecord2*)
        g_actionQueueTable.FindOrCreateRecord(key, 1, &wasCreated, 0, 1);

    int status = 0;
    if (record)
    {
        unsigned int selfFlags = *(unsigned int*)((char*)self + 0x80);
        if (selfFlags & 0x400000)
        {
            status = 2;
        }
        else if (record->m_state == 3 && !(selfFlags & 1))
        {
            *(unsigned int*)((char*)self + 0x80) = selfFlags | 1;
            Sub_6F66C470(record, self);
            Sub_6F66DB40(record);
            status = 2;
        }
        else
        {
            status = 1;
        }

        if (wasCreated != -1)
            g_actionQueueTable_Signal(wasCreated);
    }
    return status;
}

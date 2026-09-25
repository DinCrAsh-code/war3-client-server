//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67F0E0. Same guarded-list lookup shape
//  as the rest of this batch (state<6 gate), but the shipped body carries
//  its OWN full SEH-plus-/GS-cookie frame this time (not only its field
//  validator's), the same unreproducible-under-this-build's-fixed-/GS-
//  shape w3gsaction_tier8.cpp's own header already documents for three
//  entries in this batch - documented, not skipped. A plain 3-dword local
//  scratch triple {ptr, count, allocated-ptr} is zeroed up front, handed
//  by address to the not-in-scope trigger Progress_6F65BAF0 on a match,
//  and its own third field is freed with SMemFree("AudistFileProg", -2, 0)
//  at BOTH of this function's exit points - the field-validator-failure
//  early return and the normal fall-through - even though the triple can
//  only ever be populated on the success path; the shipped body's own
//  duplicate check on the failure path is exactly that, a duplicate, kept
//  here rather than folded away since folding it changes nothing about
//  what a reader would see reconstructed from the two independent
//  occurrences in the disassembly.
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

struct GameRecord13
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x130 - 0x5C];
    int m_state;                // +0x130
};

//  Storm.dll ordinal 403 - same declaration shape every other TU that
//  calls it uses (e.g. agent_deleteself.cpp).
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  0x6F676000 - fastcall(ecx, edx, stack ptr), retn 4. Same plain-SEH-
//  plus-/GS-cookie `CDataStore` byte-read shape as every other
//  FieldValidator_ in this batch - out of scope, unreproducible under
//  this build's fixed /GS-.
extern "C" __declspec(naked) int __fastcall FieldValidator_6F676000(unsigned int, unsigned int, void*)
{
    __asm { mov eax, 0x6F676000 }
    __asm { jmp eax }
}

//  0x6F67EA80 - same callee w3gsaction_tier17.cpp already declares;
//  reused by forward declaration rather than re-defined here.
void* __fastcall FindByKey_6F67EA80(void*, unsigned int);

//  0x6F65BAF0 - thiscall(this=record's own +0x58 sub-object, parsedValue,
//  &scratchTriple), retn 8. A free function cannot be declared
//  `__thiscall` (only a native member function may), so this is a member
//  of a tiny placeholder type, the same pattern w3gsaction_tier8.cpp's
//  own FieldTarget already establishes. Out of scope for this batch.
struct ProgressSink
{
    void Report(unsigned int parsedValue, void* scratchTriple);
};
__declspec(naked) void ProgressSink::Report(unsigned int, void*)
{
    __asm { mov eax, 0x6F65BAF0 }
    __asm { jmp eax }
}

//  0x6F67F0E0 - case (see w3gsactionhandler2.cpp). On a record match
//  whose own state < 6, found via FindByKey_6F67EA80 keyed by the field
//  validator's own parsed dword: fires ProgressSink::Report through the
//  record's own +0x58 sub-object and reports status 2. Every other
//  outcome (no match, wrong state, or the validator itself failing)
//  reports status 0/1 as usual for this batch.
int __stdcall W3GSAction2_6F67F0E0(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned int parsed;
    void* scratch[3] = { 0, 0, 0 };

    if (!FieldValidator_6F676000(arg4, arg8, &parsed))
    {
        if (scratch[2])
            SMemFree(scratch[2], "AudistFileProg", -2, 0);
        return 1;
    }

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord13* rec = (GameRecord13*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord13*)rec->m_nextRecord;

        if (rec && rec->m_state < 6 && FindByKey_6F67EA80(rec, parsed))
        {
            ((ProgressSink*)rec->m_field58)->Report(parsed, scratch);
            status = 2;
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }

    if (scratch[2])
        SMemFree(scratch[2], "AudistFileProg", -2, 0);

    return (int)status;
}

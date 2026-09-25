//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67AC60. Same guarded-list lookup shape
//  as w3gsaction_tier3.cpp's own entries, then a second, ARRAY-indexed
//  inner search (not a `next` chase - see w3gsaction_tier10.cpp's own
//  ListWalker header for the same idiom elsewhere in this batch) over the
//  record's own +0x11C list, keyed by a per-caller player-id byte, before
//  flipping a bit on the matched candidate and running two not-in-scope
//  trigger calls.
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

//  The guarded-table record, carrying the fields this handler reads:
//  +0x58 (a sub-object `this` for both trigger calls), +0x114 (the inner
//  array's own base for the array-indexed walk), +0x11C (that walk's own
//  starting index/head), +0x130 (m_state) and +0x148 (a distinguished
//  candidate index compared against the match).
struct GameRecord5
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x114 - 0x5C];
    unsigned int m_field114;   // +0x114
    unsigned int m_field118;   // +0x118 (padding to reach 0x11C - not read)
    int m_field11C;            // +0x11C
    char m_reserved120[0x130 - 0x120];
    int m_state;                // +0x130
    char m_reserved134[0x148 - 0x134];
    int m_field148;             // +0x148
};

//  0x6F6753A0 - retn 4, same shape as this batch's other field validators.
extern "C" __declspec(naked) int __fastcall FieldValidator_6F6753A0(unsigned int, unsigned int, void*)
{
    __asm { mov eax, 0x6F6753A0 }
    __asm { jmp eax }
}

//  0x6F65AD90 - fastcall(this, stack arg8), retn 4. Its own body forwards
//  into a further out-of-scope callee (sub_6F6595C0) this session did not
//  chase, the same shape as w3gsaction_tier7.cpp's own Sub_6F65AE20.
extern "C" __declspec(naked) void __fastcall Sub_6F65AD90(void*, unsigned int)
{
    __asm { mov eax, 0x6F65AD90 }
    __asm { jmp eax }
}

//  0x6F67A9B0 - fastcall(this, edx), plain `retn`. Not in scope
//  (41 instructions).
extern "C" __declspec(naked) void __fastcall Sub_6F67A9B0(void*, void*)
{
    __asm { mov eax, 0x6F67A9B0 }
    __asm { jmp eax }
}

//  0x6F67AC60 - case (see w3gsactionhandler2.cpp). On a record match: walk
//  the record's own +0x11C array by the caller's own low player-id byte
//  (arg8's low byte, read once before the walk, not per-iteration); on a
//  candidate whose own +0x80 bit 0 is already set, skip it (not found);
//  when the candidate isn't the record's own distinguished +0x148 index,
//  additionally require its own +0x80 bit 0x10000; on a real match, set
//  bit 0 on the candidate's own flags and run both not-in-scope triggers.
int __stdcall W3GSAction2_6F67AC60(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!FieldValidator_6F6753A0(arg4, arg8, &arg8))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord5* rec = (GameRecord5*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord5*)rec->m_nextRecord;

        if (rec)
        {
            if (rec->m_state != 6)
            {
                status = 1;
            }
            else
            {
                int index = (int)MaskedPositive(rec->m_field11C);
                //  Every path out of this block that is not a real trigger
                //  reports status 1 - including the empty-list case (the
                //  shipped body's own outer `jle` falls straight into the
                //  same "edi=1" tail every rejected-candidate and
                //  loop-exhausted path shares), so it is the default here
                //  rather than something only the loop's own tail sets.
                status = 1;
                while (index > 0)
                {
                    char* candidateBase = (char*)index;
                    if (*(unsigned char*)(candidateBase + 0xB4) == (unsigned char)arg8)
                    {
                        unsigned int flags = *(unsigned int*)(candidateBase + 0x80);
                        if (!(flags & 1) &&
                            (index == rec->m_field148 || (flags & 0x10000)))
                        {
                            *(unsigned int*)(candidateBase + 0x80) = flags | 1;
                            Sub_6F65AD90(rec->m_field58, arg8);
                            Sub_6F67A9B0(rec, rec->m_field58);
                            status = 2;
                        }
                        break;
                    }

                    char* next = (char*)rec->m_field114 + index;
                    index = *(int*)(next + 4);
                }
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}

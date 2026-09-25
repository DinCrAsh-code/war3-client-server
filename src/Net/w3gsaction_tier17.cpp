//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67FC60. Same guarded-list lookup shape
//  as the rest of this batch, then a second search through the
//  not-in-scope Sub_6F67EA80 (a small masked-list walk off the record's
//  own +0x288 field, keyed by a dword the field validator parses into a
//  fresh local buffer) before two more not-in-scope calls gate on the
//  found candidate's own +0x240/+0x244 fields. The shipped body sets its
//  own return value to 2 the moment a record is found (BEFORE the second
//  search even runs) and only ever resets it back to 0 on one specific
//  path - a candidate that fails the +0x240/+0x244 gate, or one the
//  second search never finds at all, still returns 2 - so this
//  reconstruction mirrors that exactly rather than renaming it to a
//  "status" that implies every branch picks its own value.
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

struct GameRecord10
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
};

//  0x6F666B90 - already reconstructed for real (not a redirect) in
//  w3gsaction_parsers.cpp: ParseByteByteDwordDwordAction_6F666B90 parses
//  a byte, a byte, a dword and a dword into its own out-buffer, so this
//  call site reuses that name directly rather than re-declaring a naked
//  copy under a local one.
int __fastcall ParseByteByteDwordDwordAction_6F666B90(void*, unsigned int, unsigned char*);

//  0x6F67EA80 - fastcall(this=record, key), plain `retn`. A masked-list
//  search off record->0x288: walk the intrusive list (each node's own +4
//  "next", the classic "clamp negative head to 0" masked-positive shape
//  this file's own MaskedPositive already establishes) for the first node
//  whose own +8 dword equals `key`.
void* __fastcall FindByKey_6F67EA80(void* record, unsigned int key)
{
    int node = *(int*)((char*)record + 0x288);
    node = (node > 0) ? node : 0;
    while (node > 0)
    {
        if (*(unsigned int*)(node + 8) == key)
            return (void*)node;
        node = *(int*)(node + 4);
    }
    return 0;
}

//  0x6F679880 - fastcall(this=record, candidate), plus ONE stack byte
//  argument, retn 4.  If `candidate` is record's own current +0x148
//  target: a byte match at candidate->0xB4 against `byteArg` is a
//  deliberate non-match (returns 0, not the candidate) - only a *miss*
//  there falls through to a second masked-list search (record->0x11C,
//  walked through record->0x114 as a base rather than the node's own +4 -
//  the exact indexing scheme is a further, un-investigated subsystem, so
//  it is reproduced literally rather than renamed into invented semantics)
//  for a node whose own 0xB4 byte equals `byteArg`.  If `candidate` is
//  NOT record's own +0x148 target, the sense flips: a 0xB4 byte match
//  against `byteArg` *is* success (returns `candidate` itself).
int __fastcall Validate_6F679880(void* record, void* candidate, unsigned char byteArg)
{
    char* rec = (char*)record;
    if (candidate == *(void**)(rec + 0x148))
    {
        if (*(unsigned char*)((char*)candidate + 0xB4) == byteArg)
            return 0;

        int node = *(int*)(rec + 0x11C);
        node = (node > 0) ? node : 0;
        while (node > 0)
        {
            if (*(unsigned char*)((char*)node + 0xB4) == byteArg)
                return node;
            char* base = *(char**)(rec + 0x114);
            node = *(int*)(base + node + 4);
        }
        return 0;
    }

    if (*(unsigned char*)((char*)candidate + 0xB4) == byteArg)
        return (int)candidate;
    return 0;
}

//  0x679680 - fastcall(this=record, candidate) plus ONE stack argument
//  (the previous call's own return value), retn 4. The shipped call
//  site never reloads ecx before `call` - it still holds the record from
//  the immediately preceding Validate_6F679880 call - so this
//  declaration carries it explicitly rather than leaving it to arrive by
//  coincidence, the same hidden-register-argument shape
//  w3gsaction_tier16.cpp's own Trigger_6F67BC20 already documents.
extern "C" __declspec(naked) void __fastcall Commit_6F679680(void*, void*, int)
{
    __asm { mov eax, 0x6F679680 }
    __asm { jmp eax }
}

//  0x6F67FC60 - case (see w3gsactionhandler2.cpp). On a record match:
//  search Sub_6F67EA80 by the validator's own parsed dword; a found
//  candidate whose own +0x240 == 2 and +0x244 == (byte)arg8 runs
//  Validate_6F679880(record, self, (byte)arg8) and, only if that returns
//  nonzero, Commit_6F679680(record, candidate, thatReturnValue) - which
//  resets the return value to 0 only on Validate_6F679880 itself
//  returning 0. Every other outcome (no match, or +0x240/+0x244
//  mismatch) still returns 2, per the shipped body's own single
//  "found -> assume 2" store.
int __stdcall W3GSAction2_6F67FC60(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[12];

    if (!ParseByteByteDwordDwordAction_6F666B90((void*)arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord10* rec = (GameRecord10*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord10*)rec->m_nextRecord;

        if (rec)
        {
            unsigned int searchKey = *(unsigned int*)(buf + 2);
            status = 2;

            void* candidate = FindByKey_6F67EA80(rec, searchKey);
            if (candidate &&
                *(unsigned int*)((char*)candidate + 0x240) == 2 &&
                *(unsigned char*)((char*)candidate + 0x244) == (unsigned char)arg8)
            {
                int result = Validate_6F679880(rec, self, (unsigned char)arg8);
                if (result != 0)
                    Commit_6F679680(rec, candidate, result);
                else
                    status = 0;
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}

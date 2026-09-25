//============================================================================
//  batch-H-w3gsaction-and-replaystream - 0x6F67B710. A different guard
//  infrastructure from every other handler in this batch:
//  g_netClientRecordTableGuard/g_netClientRecordListHead
//  (netclient_recvappend.cpp's own unk_6FACFF90/dword_6FACFFA4), not
//  ActionQueueTable - this handler walks the NetClient connection-record
//  table by key (self+0x40) directly, guarded by Wait(1)/SignalOne(1)
//  rather than the ActionQueueTable family's own Wait(0)/SignalOne(0).
//
//  On a match, it scans the found record's own ring buffer at +0x1D8 (a
//  {capacity@+4, headIndex@+0xC, entries@+8} triple - FindNextRingEntry,
//  sub_6F665820) for entries not yet delivered (each entry's own +0xC
//  "sequence" compared against the parsed dword field, which the caller's
//  own arg4 stack slot doubles as storage for - the same stack-slot-
//  reuse idiom this whole batch's own handlers use throughout),
//  dispatching each qualifying one through DispatchVtableSlot11 with
//  `self`'s own +0x3C field as the target - not a list node's, unlike
//  every ActionQueueTable-backed handler - before advancing the record's
//  own state to 10 via the already-thunked Sub_6F67A960.
//============================================================================
//  0x6F675BE0 - already reconstructed for real (w3gsaction_parsers.cpp).
extern int __fastcall ParseDwordAction_6F675BE0(void* src, unsigned int declaredLength, unsigned int* out);

//  0x6F6D9820/0x6F6D8950 - already reconstructed for real elsewhere.
struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90 (netclient_recvappend.cpp)
extern int               g_netClientRecordListHead;     // dword_6FACFFA4 (netclient_recvappend.cpp)

//  Already a thunk (w3gsaction_tier13.cpp/tier15.cpp); re-declared
//  identically here for extern linkage - `extern "C"` matters, this
//  symbol has flip-flopped between C and C++ linkage across concurrent
//  sessions before (see CLAUDE.md's own git-conflict guidance).
extern "C" void __fastcall Sub_6F67A960(void* self, unsigned int newState);

//  0x6F6DAE20 - already reconstructed for real elsewhere in this tree;
//  re-declared identically here for extern linkage.
extern void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//----------------------------------------------------------------------------
//  0x6F665820 - fastcall(this=&out, stack=&ringBuf). Scans a circular
//  buffer (capacity@+4, headIndex@+0xC, entries@+8) starting from its own
//  head index for the first non-null entry, wrapping at most once; writes
//  {ringBufBase, foundOrWrappedIndex, capacity} into `out`.
//----------------------------------------------------------------------------
struct RingScanResult
{
    void* ringBuf;
    unsigned int index;
    unsigned int capacity;
};

void __fastcall FindNextRingEntry(RingScanResult* out, void* ringBuf)
{
    out->ringBuf = ringBuf;
    unsigned int index = *(unsigned int*)((char*)ringBuf + 0xC);

    for (;;)
    {
        void** entries = *(void***)((char*)ringBuf + 8);
        if (entries[index] != 0)
            break;

        index += 1;
        if (index == *(unsigned int*)((char*)ringBuf + 4))
            index = 0;

        if (index == *(unsigned int*)((char*)ringBuf + 0xC))
            break;
    }

    out->index = index;
    out->capacity = *(unsigned int*)((char*)ringBuf + 4);
}

//  0x6F67B710 - case (see w3gsactionhandler2.cpp).
int __stdcall W3GSAction2_6F67B710(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned int parsedValue;
    if (!ParseDwordAction_6F675BE0((void*)arg4, arg8, &parsedValue))
        return 1;

    unsigned int key = *(unsigned int*)((char*)self + 0x40);
    int status = 0;
    if (key != 0)
    {
        g_netClientRecordTableGuard.Wait(1);

        void* rec = MaskedPositive(g_netClientRecordListHead);
        while (rec && *(unsigned int*)((char*)rec + 0xC) != key)
            rec = MaskedPositive(*(int*)((char*)rec + 8));

        if (!rec)
        {
            g_netClientRecordTableGuard.SignalOne(1);
        }
        else
        {
            unsigned int startIndex = *(unsigned int*)((char*)rec + 0x1D4) + 1;

            RingScanResult scan;
            FindNextRingEntry(&scan, (char*)rec + 0x1D8);

            void** entries = *(void***)((char*)scan.ringBuf + 8);
            unsigned int idx = scan.index;
            unsigned int remaining = scan.capacity;
            while (remaining > 0)
            {
                void* entry = entries[idx];
                idx += 1;
                if (idx == *(unsigned int*)((char*)scan.ringBuf + 4))
                    idx = 0;
                remaining -= 1;

                if (entry)
                {
                    if ((int)(startIndex - parsedValue) > 0)
                    {
                        unsigned int bufferLen = *(unsigned int*)((char*)entry + 8);
                        void* bufferBase = (char*)entry + 0xC;
                        void* target = *(void**)((char*)self + 0x3C);
                        DispatchVtableSlot11(target, (int)bufferBase, bufferLen);
                    }
                    startIndex += 1;
                }
            }

            Sub_6F67A960(rec, 10);
            status = 2;
            g_netClientRecordTableGuard.SignalOne(1);
        }
    }
    return status;
}

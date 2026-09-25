//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F66E300. Sharing
//  gameaction0x26_handler.cpp's own ActionQueueTable/ConditionVariable
//  infrastructure (the same one w3gsaction_tier9.cpp's own 0x6F66DFE0
//  reuses), and the same ListWalker rooted at record+0xCC
//  w3gsaction_tier10.cpp's own 0x6F66E0E0 already establishes - reused by
//  forward declaration rather than re-defined here. Unlike every other
//  entry in this batch, this handler never touches unk_6FACFF90/
//  dword_6FACFFA4 at all: ActionQueueTable::FindOrCreateRecord is its own
//  guarded lookup, with its own `wasCreated`-gated SignalOne on
//  unk_6FACFF20 instead. The shipped body's own status defaults to 2 on
//  every path past the record's own +0x400000 self-flag/+0xD4-matches-
//  self gate - a found-and-notified node, `node == self`, and even an
//  empty or exhausted inner search all report 2 - and only the single
//  +0xD4-does-not-match-self path reports 1.
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

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  Same struct w3gsaction_tier10.cpp's own header already documents,
//  rooted at the record's own +0xCC; reused by forward declaration
//  rather than re-defined here.
struct ListWalker
{
    void* Step(void*);
};

//  0x6F666560 - already reconstructed for real (not a redirect) in
//  w3gsaction_parsers.cpp: ParseByteDwordAction_6F666560 parses a byte
//  and a dword into its own out-buffer.
int __fastcall ParseByteDwordAction_6F666560(void*, unsigned int, unsigned char*);

//  0x6F66A240 - fastcall(this=node->0x3C, edx=&scratch), plain `retn`.
//  Out of scope for this batch.
extern "C" __declspec(naked) void __fastcall Notify_6F66A240(void*, void*)
{
    __asm { mov eax, 0x6F66A240 }
    __asm { jmp eax }
}

//  0x6F6644B0 - already reconstructed for real (not a redirect) in
//  actionqueuerecord_timer.cpp: SetTimerDeadline(void* self, int delayMs).
void __fastcall SetTimerDeadline(void* self, int delayMs);

//  0x6F66E300 - case (see w3gsactionhandler2.cpp). On self->0x40 != 0
//  and a valid field-validator parse: looks up (or creates) the record
//  via FindOrCreateRecord. If self's own +0x80 bit 0x400000 is set,
//  reports status 2 outright. Otherwise, only when the record's own
//  +0xD4 (masked) equals self, searches the record's own +0xCC inner
//  list for a node whose own +0xB4 byte matches the validator's parsed
//  byte: on a real match that is not self, stamps the node's own +0x44
//  from the validator's parsed dword, fires Notify_6F66A240 through the
//  node's own +0x3C sub-object and SetTimerDeadline(node, 0x7D0).
//  Every one of those paths - including `node == self`, an empty list,
//  or a search that never matches - reports 2; only the outer +0xD4
//  mismatch reports 1. Signals unk_6FACFF20 with `wasCreated != 0`
//  whenever FindOrCreateRecord did not report -1.
int __stdcall W3GSAction_6F66E300(void* self, unsigned int arg4, unsigned int arg8)
{
    if (*(int*)((char*)self + 0x40) == 0)
        return 1;

    unsigned char buf[8];
    if (!ParseByteDwordAction_6F666560((void*)arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    void* record = g_actionQueueTable.FindOrCreateRecord(key, 0, (int*)&arg8, 0, 1);

    int status = 0;
    if (record)
    {
        if (*(unsigned int*)((char*)self + 0x80) & 0x400000)
        {
            status = 2;
        }
        else if (MaskedPositive(*(int*)((char*)record + 0xD4)) != self)
        {
            status = 1;
        }
        else
        {
            int index = (int)MaskedPositive(*(int*)((char*)record + 0xD4));
            unsigned char keyByte = buf[0];
            while (index > 0)
            {
                if (*(unsigned char*)((char*)index + 0xB4) == keyByte)
                    break;
                index = (int)((ListWalker*)((char*)record + 0xCC))->Step((void*)index);
            }

            if (index > 0 && (void*)index != self)
            {
                unsigned int parsedValue = *(unsigned int*)(buf + 1);
                *(unsigned int*)((char*)index + 0x44) = parsedValue;
                unsigned int scratch = parsedValue;
                Notify_6F66A240(*(void**)((char*)index + 0x3C), &scratch);
                SetTimerDeadline((void*)index, 0x7D0);
            }

            status = 2;
        }

        if ((int)arg8 != -1)
            ((ConditionVariable*)&g_actionQueueSignal)->SignalOne((unsigned int)(arg8 != 0));
    }
    return status;
}

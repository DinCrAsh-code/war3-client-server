//============================================================================
//  0x6F66BEF0 - reached from ServerTock_0x27_PacketHandler's own root
//  handler under the record's own lock, right after a Tock's worth of
//  queued actions is confirmed to exist for `self` (a session/player
//  object). record->0x1C8 is the same nullable "registered consumer"
//  pointer gameaction0x26_handler.cpp's own ActionQueueRecord struct
//  already documents; the fields this function reads/writes beyond that
//  (0xCC, 0xD4, 0xE4, 0x1A8) sit in the same struct but were not
//  established by that earlier session - kept as raw offsets rather than
//  guessed names. `record` arrives in ecx and `self` in edx (a genuine
//  __fastcall two-register signature, not an ordinary thiscall member -
//  the same non-standard-but-not-hand-written convention this function's
//  own two stack-arg-free callees, BuildRelay1 and the ActionQueueRecordArrays
//  finders, already use).
//============================================================================

//  __stdcall, not cdecl: kernel32 exports this as _GetTickCount@0, and
//  declaring it without the convention asked the linker for a plain
//  _GetTickCount that nothing provides (LNK2019).  The two declarations
//  below already had it right.
extern "C" unsigned int __stdcall GetTickCount();
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
void __stdcall SErrSetLastError(unsigned int code);
extern "C" long dword_6FACFEFC;
extern "C" long dword_6FACFEF8;
unsigned __int64 __fastcall Interlocked64DecrementAt(volatile unsigned __int64*);
int __fastcall BuildRelay1(void* self, void* arg);

class ActionQueueRecordArrays
{
public:
    int FindAndEraseFirst(unsigned char key);
    int FindAndEraseSecond(unsigned char key);
};

void __fastcall OnTockQueued(void* record, void* self, unsigned int hasPending)
{
    char* r = (char*)record;
    char* s = (char*)self;
    void* consumer = *(void**)(r + 0x1C8);
    if (consumer == 0)
        return;

    unsigned char playerId = *(unsigned char*)(s + 0xB4);
    int found = ((ActionQueueRecordArrays*)consumer)->FindAndEraseFirst(playerId);
    if (!found)
    {
        if (hasPending != 0)
        {
            unsigned char playerId2 = *(unsigned char*)(s + 0xB4);
            ((ActionQueueRecordArrays*)*(void**)(r + 0x1C8))->FindAndEraseSecond(playerId2);
        }
        return;
    }

    unsigned int now = GetTickCount();
    consumer = *(void**)(r + 0x1C8);
    int elapsed = (int)now - *(int*)((char*)consumer + 0xD0);
    if (elapsed < 0)
        elapsed = 0;
    elapsed += *(int*)(s + 0x88);
    if ((unsigned int)elapsed >= 0xAFC8)
        elapsed = 0xAFC8;
    *(int*)(s + 0x88) = elapsed;
    *(int*)(s + 0x74) = now;

    if (*(int*)(r + 0xE4) != 7)
    {
        int lastTick = *(int*)(r + 0x1A8);
        int node = *(int*)(r + 0xD4);
        if (node > 0)
        {
            do
            {
                if (*(int*)((char*)node + 0x64) == 0 && *(int*)((char*)node + 0x60) == lastTick)
                    *(unsigned int*)((char*)node + 0x5C) = now;
                node = *(int*)(r + 0xCC + node + 4);
            } while (node > 0);
        }

        struct { unsigned char playerId; int clamped; } arg;
        arg.playerId = *(unsigned char*)(s + 0xB4);
        arg.clamped = *(int*)(s + 0x88);
        BuildRelay1(r + 0xCC, &arg);
    }

    consumer = *(void**)(r + 0x1C8);
    if (*(int*)((char*)consumer + 0xC8) != 0)
        return;
    if (consumer != 0)
        SMemFree(consumer, "delete", -1, 0);
    *(void**)(r + 0x1C8) = 0;

    if (dword_6FACFEFC <= 4)
    {
        SErrSetLastError(0x57);
        return;
    }
    Interlocked64DecrementAt((volatile unsigned __int64*)((char*)&dword_6FACFEF8 + 0x20));
}

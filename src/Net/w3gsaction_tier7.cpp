//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67AE70, sharing the same guarded
//  singly-linked record table (unk_6FACFF90/dword_6FACFFA4/MaskedPositive)
//  w3gsaction_tier3.cpp's own three entries already reconstruct, gated on
//  a field validator this session did not otherwise need
//  (w3gsaction_tier3.cpp's own FieldValidator_* naming continued here).
//============================================================================
#include <windows.h>

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

//  A record in the same guarded table w3gsaction_tier3.cpp's own
//  GameRecord walks, extended with the fields this handler reads:
//  +0x58 (a sub-object `this` for two of the three trigger calls) and
//  +0x130 (a state field, same offset netclient_notifyevent_0x23.cpp's
//  own NetClientNotifyRecord names m_state, but a distinct table/record
//  type - the shared field position across otherwise-unrelated tables is
//  coincidence, not evidence of a shared base class).
struct GameRecord2
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x58 - 0x10];
    void* m_field58;           // +0x58
    char m_reserved05C[0x130 - 0x5C];
    int  m_state;               // +0x130
};

//  0x6F675500 - retn 4 (fastcall ecx/edx + one stack out-pointer). Not one
//  of this batch's own worklist addresses - out of scope, same
//  whole-function naked-redirect reasoning as every other out-of-scope
//  callee in this batch.
extern "C" __declspec(naked) int __fastcall FieldValidator_6F675500(void*, unsigned int, unsigned int*)
{
    __asm { mov eax, 0x6F675500 }
    __asm { jmp eax }
}

//  0x6F67A960 - fastcall(this=record, edx=newState).  This call site's own
//  `edx` argument was mis-declared as absent (a bare `void*`-only
//  signature) until batch-L-net-remainder made the real definition
//  available: the dump shows `mov edx, 5` immediately ahead of this site's
//  own `state < 5` compare, and nothing reloads `edx` between that compare
//  and the call - so the shipped code passes the same literal 5 the state
//  gate just tested against, not an omitted argument.  Real definition and
//  full walkthrough now live in w3gsaction_tier13.cpp (also reused,
//  correctly, by w3gsaction_tier15.cpp and w3gsaction_tier24.cpp);
//  forward-declared here rather than re-defined, matching those two.
extern "C" void __fastcall Sub_6F67A960(void*, unsigned int);

//  0x6F65AE20 - fastcall(this, arg), retn 4 (one stack argument beyond
//  ecx). Its own body forwards into a further out-of-scope callee
//  (sub_6F6597A0) this session did not chase.
extern "C" __declspec(naked) void __fastcall Sub_6F65AE20(void*, unsigned int)
{
    __asm { mov eax, 0x6F65AE20 }
    __asm { jmp eax }
}

//  0x6F6736E0 - fastcall(this, deltaMs), plain `retn`. Small enough to
//  reconstruct for real rather than redirect: stamps `this+0x16C` with
//  `GetTickCount() + deltaMs`, and if that sum happens to be exactly 0,
//  stamps 1 instead (the `jnz` after the store tests the *add*'s own
//  flags, not the store - a plain `mov` never touches flags - so this is
//  "avoid a zero deadline", not a redundant re-test of the store).
void __fastcall SetDeadlineFromNow(void* self, unsigned int deltaMs)
{
    unsigned int deadline = GetTickCount() + deltaMs;
    *(unsigned int*)((char*)self + 0x16C) = deadline;
    if (deadline == 0)
        *(unsigned int*)((char*)self + 0x16C) = 1;
}

//  0x6F67AE70 - case (see w3gsactionhandler2.cpp). Validate via
//  FieldValidator_6F675500, then under the shared guarded-list lock look
//  the record up by self+0x40; when found and its own state < 5, run the
//  record's two-step "reset" (Sub_6F67A960(record) then
//  Sub_6F65AE20(record->m_field58, 1)) and, only when self's own +0x80 bit
//  0x100000 is set, additionally push the record's own deadline out 5000ms
//  (SetDeadlineFromNow(record, 0x1388)) before reporting status 2; state
//  >= 5 reports status 1 with none of that.
int __stdcall W3GSAction2_6F67AE70(void* self, unsigned int arg4, unsigned int arg8)
{
    if (!FieldValidator_6F675500((void*)arg4, arg8, &arg8))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord2* rec = (GameRecord2*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord2*)rec->m_nextRecord;

        if (rec)
        {
            if (rec->m_state < 5)
            {
                //  The shipped body reads record->m_field58 into a
                //  callee-saved register BEFORE calling Sub_6F67A960 (which
                //  could, for all this call tree establishes, mutate it),
                //  and only THEN calls Sub_6F65AE20 with that captured
                //  value - so this reconstruction captures it into a local
                //  first rather than re-reading the field after the first
                //  call returns, which would silently pass a possibly-
                //  different value.
                void* field58 = rec->m_field58;
                Sub_6F67A960(rec, 5);
                Sub_6F65AE20(field58, 1);

                if (*(unsigned int*)((char*)self + 0x80) & 0x100000)
                    SetDeadlineFromNow(rec, 0x1388);

                status = 2;
            }
            else
            {
                status = 1;
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}

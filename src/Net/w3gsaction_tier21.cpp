//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67EE10. Same shape as
//  w3gsaction_tier19.cpp's own 0x6F67EF90 sibling right up through the
//  ListWalker::Step inner search (a byte parsed by the field validator
//  into a 12-byte local buffer this time, alongside two dwords: one for
//  FindByKey_6F67EA80, one stored into the candidate's own +0x248), with
//  two real differences from that sibling: the deadline-style gate here
//  is `candidate->+0x23C > threshold` (strictly, `jbe` to reject rather
//  than tier19's `<`), and the FixedPatternKey check on a "not already
//  flagged" match reads SELF's own +0x90, not the matched node's - easy
//  to misread as the same check tier19 makes, and NOT the same field.
//  A found-and-accepted match also fires the not-in-scope
//  Commit_6F679680 (w3gsaction_tier17.cpp's own callee) eight times in a
//  row with the identical three arguments each time - not a mistake in
//  this reconstruction, the shipped body's own loop reloads nothing
//  between iterations either.
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

struct GameRecord14
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x11C - 0x10];
    int m_field11C;              // +0x11C (inner list head)
    char m_reserved120[0x130 - 0x120];
    int m_state;                // +0x130
};

//  Same struct w3gsaction_tier10.cpp's own header already documents,
//  here rooted at the record's own +0x114.
struct ListWalker
{
    void* Step(void*);
};

//  0x6F675EA0 - fastcall(ecx, edx, stack ptr), retn 4. Same plain-SEH-
//  plus-/GS-cookie `CDataStore` byte-read shape as every other
//  FieldValidator_ in this batch, parsing a dword, a byte and a second
//  dword into its own 12-byte out-buffer - out of scope, unreproducible
//  under this build's fixed /GS-.
extern "C" __declspec(naked) int __fastcall FieldValidator_6F675EA0(unsigned int, unsigned int, void*)
{
    __asm { mov eax, 0x6F675EA0 }
    __asm { jmp eax }
}

//  0x6F67EA80 - same callee w3gsaction_tier17.cpp already declares;
//  reused by forward declaration rather than re-defined here.
void* __fastcall FindByKey_6F67EA80(void*, unsigned int);

//  0x6F6506A0 - already reconstructed for real (not a redirect) in
//  netprovider_keypattern.cpp: FixedPatternKey::Matches().
struct FixedPatternKey
{
    int Matches();
};

//  0x6F679630 - same callee w3gsaction_tier18.cpp already declares;
//  reused by forward declaration rather than re-defined here.  `extern
//  "C"`, matching the real (naked, extern "C") definition there - a
//  plain-linkage re-declaration here mangles to a different symbol and
//  leaves the .mix with an unresolved external (link_check.py catches it;
//  see batch-J's own commit 20d29ab and this session's own commit fixing
//  the same regression a second time after a concurrent edit reintroduced
//  it).
extern "C" void __fastcall Notify_6F679630(void*, void*);

//  0x6F679680 - same callee w3gsaction_tier17.cpp already declares
//  (Commit_6F679680); reused by forward declaration rather than
//  re-defined here.
extern "C" void __fastcall Commit_6F679680(void*, void*, void*);

//  0x6F67EE10 - case (see w3gsactionhandler2.cpp). On a record match
//  whose own state < 6: search FindByKey_6F67EA80 by the validator's
//  first parsed dword; a found candidate sets status to 2. If the
//  candidate's own +0x240 == 1 and its own +0x23C is strictly above the
//  validator's second parsed dword, search the record's own +0x114
//  inner list for a node whose own +0xB4 byte matches the validator's
//  parsed byte:
//    - an empty/exhausted search, or the one match being self, resets
//      status to 0;
//    - a real match whose own +0x5C bit 1 is already set, or whose own
//      +0x90 field (SELF's, not the match's) satisfies FixedPatternKey,
//      stamps the candidate's own +0x240/+0x244/+0x248 fields and fires
//      Commit_6F679680(record, candidate, node) eight times running;
//    - anything else (the +0x240/+0x23C gate itself failing, or a real
//      match whose own +0x5C bit 1 is clear and self's own
//      FixedPatternKey does not match) fires Notify_6F679630 instead.
//  Every one of those last three outcomes leaves status at 2.
int __stdcall W3GSAction2_6F67EE10(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[12];

    if (!FieldValidator_6F675EA0(arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord14* rec = (GameRecord14*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord14*)rec->m_nextRecord;

        if (rec && rec->m_state < 6)
        {
            void* candidate = FindByKey_6F67EA80(rec, *(unsigned int*)buf);
            if (candidate)
            {
                status = 2;

                if (*(unsigned int*)((char*)candidate + 0x240) == 1 &&
                    *(unsigned int*)((char*)candidate + 0x23C) > *(unsigned int*)(buf + 5))
                {
                    int index = (int)MaskedPositive(rec->m_field11C);
                    unsigned char keyByte = buf[4];
                    while (index > 0)
                    {
                        if (*(unsigned char*)((char*)index + 0xB4) == keyByte)
                            goto foundNode;
                        index = (int)((ListWalker*)((char*)rec + 0x114))->Step((void*)index);
                    }
                    status = 0;
                    goto innerSearchDone;

                foundNode:
                    if ((void*)index == self)
                    {
                        status = 0;
                    }
                    else if ((*(unsigned char*)((char*)index + 0x5C) & 2) ||
                             ((FixedPatternKey*)((char*)self + 0x90))->Matches())
                    {
                        *(unsigned int*)((char*)candidate + 0x240) = 2;
                        *(unsigned char*)((char*)candidate + 0x244) = keyByte;
                        *(unsigned int*)((char*)candidate + 0x248) = *(unsigned int*)(buf + 5);

                        for (int i = 8; i != 0; i--)
                            Commit_6F679680(rec, candidate, (void*)index);
                    }
                    else
                    {
                        Notify_6F679630(rec, candidate);
                    }
                innerSearchDone:;
                }
                else
                {
                    Notify_6F679630(rec, candidate);
                }
            }
        }

        g_netClientRecordTableGuard.SignalOne(1);
    }
    return (int)status;
}

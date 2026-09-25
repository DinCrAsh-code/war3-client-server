//============================================================================
//  batch-B-w3gsaction_thunks - 0x6F67EF90. Same guarded-list lookup shape
//  as the rest of this batch (state<6 gate this time), then a search
//  through the not-in-scope FindByKey_6F67EA80 (same callee
//  w3gsaction_tier17.cpp already declares) keyed by the field validator's
//  own parsed dword. A found candidate sets the shipped body's own return
//  value to 2 - unconditionally, before any of the finer gates below run -
//  and it is reset to 0 on exactly one path: the inner byte-keyed list
//  search (record's own +0x114 sub-object, the same ListWalker::Step
//  w3gsaction_tier10.cpp's own header documents, here based at a
//  different record offset) coming up empty, or its one match being the
//  handler's own self pointer. Every other outcome - including the
//  "reject" path that fires Notify_6F679630 (w3gsaction_tier18.cpp's own
//  callee) - leaves the return value at 2.
//============================================================================

#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F684200 - already reconstructed (netmsgio_684.cpp): read a dword
//  then a byte, the read-side mirror of 0x6F6841E0.
extern CDataStore* __fastcall NetMsgReadDwordByte_6F684200(CDataStore* self, unsigned char* dst);

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

struct GameRecord12
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x11C - 0x10];
    int m_field11C;              // +0x11C (inner list head; +0x114 is the
                                  // ListWalker::Step sub-object's own base,
                                  // and this field is literally that
                                  // sub-object's own +8)
    char m_reserved120[0x130 - 0x120];
    int m_state;                // +0x130
};

//  Same struct w3gsaction_tier10.cpp's own header already documents, here
//  rooted at a different record offset (+0x114, not +0xCC).
struct ListWalker
{
    void* Step(void*);
};

//  0x6F675F50 - fastcall(ecx=src, edx=declaredLength, stack out), retn 4.
//  Same family as w3gsaction_tier3.cpp's own FieldValidator_6F675B30/
//  FieldValidator_6F675D40 - a real CDataStore local wrapping the
//  already-reconstructed NetMsgReadDwordByte_6F684200 (a dword then a
//  byte), success iff m_readPos == m_field10, wrapped in the unified
//  SEH/`/GS`-cookie frame docs/msvc-vc8-idioms.md documents as
//  unreproducible under this build's fixed `/GS-`.
int __fastcall FieldValidator_6F675F50(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDwordByte_6F684200(&local, out);
    return local.m_readPos == local.m_field10;
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

//  0x6F67EF90 - case (see w3gsactionhandler2.cpp). On a record match
//  whose own state < 6: search FindByKey_6F67EA80 by the validator's
//  parsed dword; a found candidate sets status to 2. If the candidate's
//  own +0x240 == 1 and its own +0x23C is below its own +0x214 (a
//  deadline-style gate), search the record's own +0x114 inner list
//  (ListWalker::Step) for a node whose own +0xB4 byte matches the
//  validator's key byte:
//    - an empty/exhausted search, or the one match being `self`, resets
//      status to 0;
//    - a real match whose own +0x5C bit 1 is already set, or whose own
//      +0x90 FixedPatternKey matches, stamps the candidate's own +0x240
//      to 4 and +0x244 to the match's own +0xB4 byte;
//    - anything else (the +0x240 gate itself failing, or a real match
//      whose own +0x5C bit 1 is clear and its own FixedPatternKey does
//      not match) fires Notify_6F679630(record, candidate) instead.
//  Every one of those last three outcomes leaves status at 2.
int __stdcall W3GSAction2_6F67EF90(void* self, unsigned int arg4, unsigned int arg8)
{
    unsigned char buf[8];

    if (!FieldValidator_6F675F50((void*)arg4, arg8, buf))
        return 1;

    void* key = *(void**)((char*)self + 0x40);
    unsigned int status = 0;
    if (key)
    {
        g_netClientRecordTableGuard.Wait(1);

        GameRecord12* rec = (GameRecord12*)MaskedPositive(g_netClientRecordListHead);
        while (rec && (void*)rec->m_recordKey != key)
            rec = (GameRecord12*)rec->m_nextRecord;

        if (rec && rec->m_state < 6)
        {
            void* candidate = FindByKey_6F67EA80(rec, *(unsigned int*)buf);
            if (candidate)
            {
                status = 2;

                if (*(unsigned int*)((char*)candidate + 0x240) == 1 &&
                    *(unsigned int*)((char*)candidate + 0x23C) < *(unsigned int*)((char*)candidate + 0x214))
                {
                    //  Initial index straight off the record's own +0x11C
                    //  field (masked inline, the ListWalker idiom
                    //  w3gsaction_tier12.cpp's own search already uses);
                    //  every SUBSEQUENT step, unlike that search, goes
                    //  through the real out-of-line ListWalker::Step call
                    //  the shipped body itself keeps as a `call`, not
                    //  inlined base+index arithmetic.
                    int index = (int)MaskedPositive(rec->m_field11C);
                    unsigned char keyByte = buf[4];
                    while (index > 0)
                    {
                        if (*(unsigned char*)((char*)index + 0xB4) == keyByte)
                            goto foundNode;
                        index = (int)((ListWalker*)((char*)rec + 0x114))->Step((void*)index);
                    }
                    //  Loop-exhausted and empty-list share this one
                    //  "not found" outcome with the shipped body's own
                    //  single shared jump target - not re-derived by a
                    //  second null test, the same as `node == self`
                    //  below shares it too.
                    status = 0;
                    goto innerSearchDone;

                foundNode:
                    if ((void*)index == self)
                    {
                        status = 0;
                    }
                    else if ((*(unsigned char*)((char*)index + 0x5C) & 2) ||
                             ((FixedPatternKey*)((char*)index + 0x90))->Matches())
                    {
                        *(unsigned int*)((char*)candidate + 0x240) = 4;
                        *(unsigned char*)((char*)candidate + 0x244) = *(unsigned char*)((char*)index + 0xB4);
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

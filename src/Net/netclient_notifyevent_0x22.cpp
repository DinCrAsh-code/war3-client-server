//============================================================================
//  0x6F67A5F0 - Net::NetClient vtable slot 14, its own callee 0x6F677F70,
//  and the payload writer 0x6F684360 (networking wave 7,
//  agent-networking-wave3-2026-08-29's claim).
//
//  Last traced member of the seven-function "post a connection-lifecycle
//  notification frame" family wave 5 found (slots 9/11/12/13/14/15/16, see
//  netclient_notifyevent_0x1f.cpp's own header for the full family table
//  and the shared shape). This is the member wave 6 flagged as having the
//  family's most complex gating condition - not the simple `[eax+130h]`
//  state test every other member starts from, but a *nested* record walk
//  once that test passes.
//
//  Read as-is off the listing rather than from a guessed-at named idiom:
//  once the usual state-in-[4,inf)/flag244 gate passes, the slot walks a
//  second, per-record table - `rec->m_tableHead` (+0x11C) through the same
//  MaskedPositive(...) normalisation the outer record-table walk uses
//  (rec->0x11C > 0, else null), then a loop testing each `entry`'s own
//  byte at +0xB4 against the caller's own byte argument. The "next" step
//  is the part that does not obviously match a plain linked list: it is
//  not `entry->next` at a fixed offset, but
//  `*(int*)((char*)rec->m_tableBase /* +0x114 */ + (int)entry + 4)` - the
//  *current* entry value re-added to a second base pointer (+0x114)
//  before the next-pointer read. Transcribed literally rather than
//  reshaped into an `entry->next` a normal struct would have, since
//  nothing in this call tree establishes what `entry` actually points
//  *into* (this rebasing only makes sense if `entry` is itself an offset
//  into the same `m_tableBase` region, i.e. the "pointers" this table
//  stores are relative to +0x114, not absolute - consistent with the
//  outer table needing its own MaskedPositive tagging trick, just with a
//  different sentinel scheme). This is the concrete open question wave 6
//  flagged and this session did not resolve either; the instruction
//  stream matches regardless of which of those two readings is true,
//  since both compile to the identical pointer arithmetic.
//
//  On a match, the caller's own {byte, dword} pair is packed into a local
//  5-byte struct and handed to the family helper exactly like every other
//  member - `retn 8` (two stack arguments: the byte and the dword, not
//  the family's usual single scalar), matching this member's own final
//  callee (0x6F684360: WriteByte(byte) + WriteDword(dword), a genuine
//  two-field payload where every other traced member writes at most one
//  value).
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

struct ConditionVariable
{
    void Wait(unsigned int value);
    void SignalOne(unsigned int value);
};

//  netclient_recvappend.cpp - the same global guard and list head every
//  member of this family walks.
extern ConditionVariable g_netClientRecordTableGuard;   // unk_6FACFF90
extern int               g_netClientRecordListHead;     // dword_6FACFFA4

void __fastcall DispatchVtableSlot11(void* self, int edxArg, unsigned int arg0);

//  Redeclared at file scope exactly as netclient_notifyevent_0x1f.cpp
//  declares it - the real constructor/destructor bodies live in
//  cdatastorecache.cpp and bind through the linker.
class CDataStoreCache1460 : public CDataStore
{
public:
    CDataStoreCache1460();
    ~CDataStoreCache1460();

    char m_inlineBuffer[1460];
};

static void* MaskedPositive(int v)
{
    return (v > 0) ? (void*)v : 0;
}

//  Same field layout as netclient_notifyevent_0x1f.cpp's own
//  NetClientNotifyRecord, plus this member's own two extra fields -
//  redeclared locally per CLAUDE.md's rule against inventing a shared
//  header from independent partial views.
struct NetClientNotifyRecord
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x114 - 0x10];
    void* m_tableBase;         // +0x114 - see the file header note
    char m_reserved118[0x11C - 0x118];
    int  m_tableHead;          // +0x11C
    char m_reserved120[0x130 - 0x120];
    int  m_state;              // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;          // +0x148
    char m_reserved14C[0x244 - 0x14C];
    int  m_flag244;            // +0x244
};

//  The {byte, dword} pair the slot packs before handing it to the family
//  helper - `mov [esp-8],bl / mov [esp-7],ecx`, a 5-byte, unaligned
//  struct (no padding the shipped code respects between the two fields).
#pragma pack(push, 1)
struct BytePlusDword
{
    unsigned char byteValue;
    unsigned int  dwordValue;
};
#pragma pack(pop)

//  0x6F684360 - write both fields of the packed pair: WriteByte(byte)
//  then WriteDword(dword) - the family's only two-field payload. Defined
//  in its own TU (writebyteplusdwordfromptr_0x22.cpp) - a `static` copy
//  here gets inlined back into its only caller by this repo's own /O2,
//  where the shipped body is a genuine out-of-line call.
CDataStoreScratch* __fastcall WriteBytePlusDwordFromPtr_0x22(CDataStoreScratch* writer, BytePlusDword* passthrough);

//  0x6F677F70 - see the family note above: {0xF7, 0x22} header, length
//  placeholder, the two-field payload, patch the length in, dispatch
//  through vtable slot 11.  SEH_-framed like slots 12/25's own family
//  helpers.
int __fastcall NetClientNotifyEvent_0x22(void* target, BytePlusDword* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x22);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WriteBytePlusDwordFromPtr_0x22(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F67A5F0 - the vtable slot itself.  `state >= 4` (no upper bound,
//  unlike every other traced member's own [4,X) range) plus the usual
//  flag244 gate, then the nested per-record table walk described above.
struct NetClientNotifySelf
{
    void PostEvent_0x22(unsigned char key, unsigned int payload);
};

void NetClientNotifySelf::PostEvent_0x22(unsigned char key, unsigned int payload)
{
    void* self = this;
    void* recKey = *(void**)((char*)self + 4);
    if (!recKey)
        return;

    g_netClientRecordTableGuard.Wait(0);

    NetClientNotifyRecord* rec = (NetClientNotifyRecord*)MaskedPositive(g_netClientRecordListHead);
    while (rec && (void*)rec->m_recordKey != recKey)
        rec = (NetClientNotifyRecord*)MaskedPositive((int)rec->m_nextRecord);

    if (!rec)
    {
        g_netClientRecordTableGuard.SignalOne(0);
        return;
    }

    if (rec->m_state >= 4 && rec->m_flag244 != 0)
    {
        //  Only the head of this list runs through the full
        //  MaskedPositive() normalisation; the shipped code's own "next"
        //  step is a plain positive test with no re-masking (`test
        //  ecx,ecx / jg`). This build's compiler folds the equivalent
        //  ternary into a second branchless mask instead - tried the
        //  explicit two-exit (found/not-found) loop shape the asm
        //  actually has, which scored *worse* (an extra redundant
        //  test), so kept as the simpler ternary; the residual gap here
        //  is this loop's own branch shape, not a different condition,
        //  field, or call - every value and call target past it agrees.
        void* entry = MaskedPositive(rec->m_tableHead);
        while (entry && *((unsigned char*)entry + 0xB4) != key)
        {
            int next = *(int*)((char*)rec->m_tableBase + (int)entry + 4);
            entry = (next > 0) ? (void*)next : 0;
        }

        if (entry)
        {
            BytePlusDword packed;
            packed.byteValue = key;
            packed.dwordValue = payload;

            void* provider = rec->m_provider;
            void* target = *(void**)((char*)provider + 0x3C);
            NetClientNotifyEvent_0x22(target, &packed);
        }
    }

    g_netClientRecordTableGuard.SignalOne(0);
}

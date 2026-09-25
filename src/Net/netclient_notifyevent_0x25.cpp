//============================================================================
//  0x6F67A410 - Net::NetClient vtable slot 11, its own callee 0x6F677C70,
//  and the two leaves 0x6F3A1830 / 0x6F685050 (networking wave 7,
//  agent-networking-wave3-2026-08-29's claim).
//
//  Third traced member of the seven-function "post a connection-lifecycle
//  notification frame" family wave 5 found (slots 9/11/12/13/14/15/16, see
//  netclient_notifyevent_0x1f.cpp's own header for the full family table
//  and the shared shape). This is the one member wave 6 flagged as real
//  scope, not mechanical: it is the only member whose payload is a
//  caller-supplied *string* rather than a fixed-size scalar.
//
//  The slot itself (`retn 4`) receives one stack argument - a pointer to a
//  word-length-prefixed buffer ({len:u16}{len bytes}, no NUL) - and, once
//  past the same record-table walk and [4,6)/flag244 gate every other
//  member shares, copies it through 0x6F3A1830 into a local 0x84-byte
//  stack buffer before handing that copy's address to the family helper
//  (0x6F677C70). /GS is active here (unlike the scalar members: a local
//  array big enough to overrun is what turns the cookie back on for one
//  member of an otherwise cookie-free family), and 0x6F677C70 itself is
//  SEH_-framed for the same reason - matching the other SEH_-framed family
//  member already reconstructed (slot 12, netclient_notifyevent_0x1f.cpp).
//
//  0x6F3A1830 is a small standalone leaf, not part of this family's own
//  address neighbourhood (0x6F3AxxNN vs. the family's 0x6F67Axxxx/
//  0x6F677xxx/0x6F683xxx/0x6F684xxx/0x6F685xxx ranges) - a generic
//  word-length-prefixed buffer copy with no other caller in any dump this
//  repo has fetched, so it is named and scored from its own shape alone:
//  read the u16 length at `src`, write it at `dest`, then memcpy the
//  `len` bytes that follow. Returns `dest`, matching this class's own
//  writer/copy fluent-`this` convention.
//
//  0x6F685050 is the family's own per-member payload writer for this tag:
//  WriteWord(the u16 length already sitting at the front of the copied
//  buffer) followed by TailCallWriteRaw(the len bytes right after it) -
//  the same {WriteWord(len), raw bytes} shape a length-prefixed string
//  gets everywhere else in this codebase (WriteString's own sibling
//  shape, cdatastorescratch.h).
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int count);

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
//  NetClientNotifyRecord - redeclared locally per CLAUDE.md's rule
//  against inventing a shared header from independent partial views.
struct NetClientNotifyRecord
{
    char m_reserved000[0x8];
    void* m_nextRecord;        // +0x08
    unsigned int m_recordKey;  // +0x0C
    char m_reserved010[0x130 - 0x10];
    int  m_state;              // +0x130
    char m_reserved134[0x148 - 0x134];
    void* m_provider;          // +0x148
    char m_reserved14C[0x244 - 0x14C];
    int  m_flag244;            // +0x244
};

//  0x6F3A1830 - copy a word-length-prefixed buffer: `*dest = *src` (the
//  u16 length), then `memcpy(dest+2, src+2, *src)`.  Generic leaf, no
//  other caller in any dump this repo has fetched.  `retn 4` - a real
//  __thiscall (dest in ecx, src on the stack), not __fastcall; declared
//  as a one-argument member so the compiler puts `dest` in ecx and `src`
//  on the stack the same way.
struct LenPrefixedCopier
{
    void* Copy(const void* src);
};

void* LenPrefixedCopier::Copy(const void* src)
{
    void* dest = this;
    unsigned short len = *(const unsigned short*)src;
    *(unsigned short*)dest = len;
    memcpy((char*)dest + 2, (const char*)src + 2, len);
    return dest;
}

//  0x6F685050 - write the length-prefixed payload the local buffer above
//  now holds: WriteWord(len) + TailCallWriteRaw(the len bytes after it).
//  Defined in its own TU (writelenprefixedfromptr_0x25.cpp) - the shipped
//  body is a genuine out-of-line call, and a `static` copy here gets
//  inlined back into its only caller by this repo's own /O2. See that
//  file's own header for why.
CDataStoreScratch* __fastcall WriteLenPrefixedFromPtr_0x25(CDataStoreScratch* writer, void* passthrough);

//  0x6F677C70 - see the family note above: {0xF7, 0x25} header, length
//  placeholder, the length-prefixed payload, patch the length in,
//  dispatch through vtable slot 11.  SEH_-framed like slot 12's own
//  family helper - the local 0x5D8-byte buffer (CDataStoreCache1460's own
//  1460-byte inline storage plus frame furniture) is what keeps /GS live
//  for this member.
int __fastcall NetClientNotifyEvent_0x25(void* target, void* passthrough)
{
    CDataStoreCache1460 cache;
    CDataStoreScratch* writer = (CDataStoreScratch*)&cache;

    writer->WriteByte(0xF7);
    writer->WriteByte(0x25);
    unsigned int lengthFieldPos = (unsigned int)writer->m_field10;
    writer->WriteWord(0);

    WriteLenPrefixedFromPtr_0x25(writer, passthrough);

    unsigned int endPos = (unsigned int)writer->m_field10;
    writer->WriteWordAt(lengthFieldPos, (unsigned short)endPos);

    void* bufferBase;
    int bufferLen;
    writer->QueryBufferState(&bufferBase, &bufferLen, 0);

    DispatchVtableSlot11(target, (int)bufferBase, (unsigned int)bufferLen);

    return bufferLen;
}

//  0x6F67A410 - the vtable slot itself.  Same [4,6)/flag244 gate every
//  scalar-payload member of the family uses; the string copy into a local
//  0x84-byte buffer is what this member alone needs before handing off to
//  the family helper.
struct NetClientNotifySelf
{
    void PostEvent_0x25(void* lenPrefixedString);
};

void NetClientNotifySelf::PostEvent_0x25(void* lenPrefixedString)
{
    void* self = this;
    void* key = *(void**)((char*)self + 4);
    if (!key)
        return;

    g_netClientRecordTableGuard.Wait(0);

    NetClientNotifyRecord* rec = (NetClientNotifyRecord*)MaskedPositive(g_netClientRecordListHead);
    while (rec && (void*)rec->m_recordKey != key)
        rec = (NetClientNotifyRecord*)MaskedPositive((int)rec->m_nextRecord);

    if (!rec)
    {
        g_netClientRecordTableGuard.SignalOne(0);
        return;
    }

    if (rec->m_state >= 4 && rec->m_state < 6 && rec->m_flag244 != 0)
    {
        char localBuffer[0x84];
        ((LenPrefixedCopier*)localBuffer)->Copy(lenPrefixedString);

        void* provider = rec->m_provider;
        void* target = *(void**)((char*)provider + 0x3C);
        NetClientNotifyEvent_0x25(target, localBuffer);
    }

    g_netClientRecordTableGuard.SignalOne(0);
}

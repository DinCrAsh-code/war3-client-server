//============================================================================
//  0x6F54BAE0 / 0x6F54B880 - the replay stream's real open/header-parse
//  entry point: open the CDataStore-wrapped stream embedded at +0x618 of
//  the session (the same object PauseCallChain's own TLS-indexed record
//  reaches, see pausecallchainthunks.cpp for the identical slot-0xD/+0x10/
//  +0x8 lookup this file's own wrapper reuses via NetDataRecords()), read a
//  four-field version quad off the stream itself, and - only on what looks
//  like the very first open (the version quad's masked value is still
//  zero) - build a companion path and run it through a decompression pass
//  before the replay's own record loop starts.
//
//  That record loop is the .w3g header proper: a leading type-0x10 record
//  (a full save-game header block plus a small player-data quad), zero or
//  more type-0x16 records right after it, and a single trailing type-0x19
//  length-prefixed buffer that ends the header and flips the session into
//  REPLAY_STATE_LOADING (CNetData::SetReplayState(1) - see
//  netdatareplaystate.cpp).  Any other tag - including a stream that does
//  not open on a type-0x10 record at all - rewinds the stream (vtable slot
//  7, the same "start again at the front" slot NetDataDiscardTempFile uses
//  on the sibling record stream) and answers failure.
//
//  sub_6F7E2EF0 (the decompression pass itself) is reconstructed for real
//  below - a follow-up session fetched a fresh, deeper dump
//  (asm/sub_6F7E2EF0_0x6F7E2EF0_calltree_asm.md, depth 2) that turned up
//  full bodies for its four direct callees (sub_6F7E2B10/2B50/3120/3150)
//  and, three levels down, a complete public-domain-shaped SHA-1
//  implementation (sub_6F00B850/890/9B0 - Init/Update/Final; the 80-round
//  core transform, sub_6F00B3A0, is ~300 instructions of pure bit-twiddling
//  with zero gameplay-behaviour stake and is kept a thunk).  See
//  docs/targets/CNetData_ReplayStreamInit.md for the restart-path notes an
//  earlier session left alongside this one.
//============================================================================
#include "netdata.h"                    // NetDataRecords(), CNetData
#include "netdata_replayrecords.h"      // ReadReplayType0x10Record/0x16Record
#include "netdata_appendplayerdata.h"   // SNetPlayerDataTable
#include "savegame.h"                   // Net::GAMESETUP_SLOTRECORD
#include "os.h"                         // Storm_281, Storm_501, Storm_572
#include "storm.h"                      // SMemFree
#include <string.h>                     // memcpy
#include <malloc.h>                     // _alloca

//  0x6F443720 - Game/savegameslotrecordread.cpp.
CDataStore* __fastcall SaveGameReadSlotRecord(CDataStore* store,
                                              Net::GAMESETUP_SLOTRECORD* out);

//  0x6F6C5A00 - Net/netdata_replayfilenamemunge.cpp.
void __fastcall ExtractPathTail(const char* src, char* dest, unsigned int size);

//  0x6F537D20 - Net/netdatareplaystate.cpp.  A local, method-only view: the
//  method is defined there for real: this repeats the same minimal
//  declaration netdatadiscardtemp.cpp and netevent_dispatch.cpp already
//  each carry their own copy of, purely so the mangled name matches.
struct SNetDataStates
{
    int SetReplayState(unsigned int state);
};

//  0x6F7E1059 already exists as SecurityCheckCookie elsewhere in the tree;
//  this target's own frame carries the same VC8 /GS buffer cookie every
//  stack-array function in this codebase does, which this build's fixed
//  /GS- cannot reproduce - see docs/msvc-vc8-idioms.md.

int __stdcall Storm_280(void* buffer);

//  unk_6F957EF4/unk_6F957EF8 - see tools/funcmap.py's DATA table entry for
//  why these are opaque, address-only externs.
extern const char g_replayDecompressParam;   // 0x6F957EF4
extern const char g_replayDecompressTable;    // 0x6F957EF8

//  0x6F00B3A0 - the SHA-1 round transform (80 rounds of bit rotation over
//  16 message words expanded to 80) - ~300 instructions of pure
//  cryptographic bit-twiddling with no dump in this call tree and zero
//  gameplay-behaviour stake (it only ever feeds a decompressed-signature
//  comparison below). Left a correctly-ABI'd naked thunk: fastcall(ecx=
//  state[5], edx=block[64 bytes]), confirmed `retn` with no operand off
//  its own tail byte (0xC3).
extern "C" __declspec(naked) void __fastcall
Sha1Transform(unsigned int* state, const unsigned char* block)
{
    __asm { mov eax, 0x6F00B3A0 }
    __asm { jmp eax }
}

//  A textbook SHA-1 context (0x6F00B850/890/9B0 - Init/Update/Final):
//  count[lo,hi] as a 64-bit running bit-count, five state words, a 64-byte
//  partial-block buffer.  Reached only from ComputeContentDigest below.
class SHA1_CTX
{
public:
    void Init();
    void Update(const void* data, unsigned int len);
    void Final(unsigned char digest[20]);

    unsigned int count_lo;
    unsigned int count_hi;
    unsigned int state[5];
    unsigned char buffer[64];
};

//----------------------------------------------------------------------------
//  0x6F00B850
//----------------------------------------------------------------------------
void SHA1_CTX::Init()
{
    count_lo = 0;
    count_hi = 0;
    state[0] = 0x67452301;
    state[1] = 0xEFCDAB89;
    state[2] = 0x98BADCFE;
    state[3] = 0x10325476;
    state[4] = 0xC3D2E1F0;
}

//----------------------------------------------------------------------------
//  0x6F00B890 - thiscall(this, data, len), retn 8: the real call site never
//  loads edx at all, so this is a plain thiscall member, not a fastcall.
//  Classic incremental-hash bookkeeping: fold `len` into the 64-bit bit
//  count, top up any partial block already buffered, transform every full
//  64-byte block that follows, and buffer whatever is left over.
//----------------------------------------------------------------------------
void SHA1_CTX::Update(const void* dataIn, unsigned int len)
{
    const unsigned char* data = (const unsigned char*)dataIn;
    unsigned int j = count_lo;

    unsigned long long bits = (unsigned long long)len * 8;
    unsigned long long* count = (unsigned long long*)&count_lo;
    *count += bits;

    j = (j >> 3) & 63;
    unsigned int i;
    if ((j + len) > 63)
    {
        i = 64 - j;
        memcpy(&buffer[j], data, i);
        Sha1Transform(state, buffer);
        for (; i + 63 < len; i += 64)
            Sha1Transform(state, &data[i]);
        j = 0;
    }
    else
    {
        i = 0;
    }
    memcpy(&buffer[j], &data[i], len - i);
}

//----------------------------------------------------------------------------
//  0x6F00B9B0 - pads with 0x80 then zeros up to a 56-mod-64 boundary
//  (computed directly from the running byte count rather than looped one
//  byte at a time), appends the 64-bit bit count big-endian, then copies
//  the five state words out big-endian.  `unk_6FA4F798` is the static
//  {0x80, 0, 0, ...} padding table the single bulk Update call below reads
//  from.
//----------------------------------------------------------------------------
extern "C" const unsigned char g_sha1Padding[64];   // unk_6FA4F798
void SHA1_CTX::Final(unsigned char digest[20])
{
    unsigned char finalCount[8];
    for (int i = 0; i < 8; i++)
        finalCount[7 - i] = (unsigned char)((i < 4 ? count_lo : count_hi) >> ((i & 3) * 8));

    unsigned int byteCount = count_lo >> 3;
    unsigned int padLen = ((~9u - byteCount) & 0x3F) + 1;
    Update(g_sha1Padding, padLen);
    Update(finalCount, 8);

    for (int i = 0; i < 5; i++)
    {
        digest[i * 4 + 0] = (unsigned char)(state[i] >> 24);
        digest[i * 4 + 1] = (unsigned char)(state[i] >> 16);
        digest[i * 4 + 2] = (unsigned char)(state[i] >> 8);
        digest[i * 4 + 3] = (unsigned char)(state[i]);
    }
}

void __stdcall Storm_510(char* buf);
int __stdcall Storm_609(unsigned int handle, const void* data, unsigned int len);
int __stdcall Storm_624(unsigned int* outHandle);
void __stdcall Storm_606(unsigned int handle);
int __stdcall Storm_628(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3);
int __stdcall Storm_638(unsigned int handle, void* buf, unsigned int bufSize, unsigned int* lenInOut);

//----------------------------------------------------------------------------
//  0x6F7E2B10 - fastcall(handle, contentLen) + (sigSize, &outOffset),
//  retn 8.  The trailing signature block is `sigSize` bytes, immediately
//  preceded by a 4-byte "NGIS" tag (0x5349474E as a little-endian dword -
//  i.e. the literal bytes 'N','G','I','S' in address order); this reports
//  whether that tag is present and, either way, writes the byte offset of
//  the tag itself (contentLen - sigSize - 4) through outOffset - the real
//  content length once the trailer is excluded.
//----------------------------------------------------------------------------
int __fastcall CheckSignatureTrailer(void* handle, unsigned int contentLen,
                                     unsigned int sigSize, unsigned int* outOffset)
{
    unsigned int offset = contentLen - sigSize - 4;
    *outOffset = offset;
    if (contentLen < sigSize + 4)
        return 0;
    return *(unsigned int*)((char*)handle + offset) == 0x5349474Eu;
}

//----------------------------------------------------------------------------
//  0x6F7E2B50 - fastcall(pathBuf, handle) + (contentLen, digestOut,
//  prefixLen), retn 0Ch.  Hashes [handle, handle+contentLen) - split into a
//  tail chunk [handle+prefixLen, contentLen) hashed FIRST and a head chunk
//  [handle, prefixLen) hashed SECOND when prefixLen is nonzero, one single
//  chunk otherwise - then a copy of `pathBuf` (run through Storm_501/
//  Storm_510 first) into the same running digest, and writes the 20-byte
//  SHA-1 result into the first 20 bytes of `digestOut` (the caller's own
//  0xBB-filled, 0x0B-terminated scratch buffer - untouched past byte 19).
//----------------------------------------------------------------------------
void __fastcall ComputeContentDigest(void* pathBuf, void* handle, unsigned int contentLen,
                                     unsigned char* digestOut, unsigned int prefixLen)
{
    SHA1_CTX ctx;
    ctx.Init();
    if (prefixLen != 0)
    {
        ctx.Update((char*)handle + prefixLen, contentLen - prefixLen);
        ctx.Update(handle, prefixLen);
    }
    else
    {
        ctx.Update(handle, contentLen);
    }

    char pathCopy[0x400];
    Storm_501(pathCopy, (const char*)pathBuf, 0x400);
    Storm_510(pathCopy);

    unsigned int len = 0;
    if (pathCopy[0] != 0)
    {
        len = 1;
        while (pathCopy[len] != 0)
            len++;
    }
    ctx.Update(pathCopy, len);

    ctx.Final(digestOut);
}

//  A pair of Storm codec handles - the two adjacent stack dwords
//  sub_6F7E3120/3150 both dereference as `this[0]`/`this[1]`.
struct DecodeHandlePair
{
    unsigned int h0;
    unsigned int h1;

    void RegisterBlobs(void* tableTag, unsigned int sigSize, void* paramTag, unsigned int reserved10);
    void DecompressChunk(void* buf, unsigned int size);
};

//----------------------------------------------------------------------------
//  0x6F7E3120 - thiscall(this) + (tableTag, sigSize, paramTag, reserved10),
//  retn 10h: register each of the two caller-supplied blobs (the
//  decompression table and its own small parameter blob) into its own
//  codec handle.
//----------------------------------------------------------------------------
void DecodeHandlePair::RegisterBlobs(void* tableTag, unsigned int sigSize, void* paramTag, unsigned int reserved10)
{
    Storm_609(h0, tableTag, sigSize);
    Storm_609(h1, paramTag, reserved10);
}

//----------------------------------------------------------------------------
//  0x6F7E3150 - thiscall(this) + (buf, size), retn 8: feed `buf` into a
//  fresh codec handle, combine it with the table/param handles into a
//  fourth (output) handle through Storm_628, run the actual decompression
//  through Storm_638 back into `buf` (in place - `size` is in/out, the
//  actual byte count produced), zero-pad anything Storm_638 left short,
//  and release both handles this call allocated.
//----------------------------------------------------------------------------
void DecodeHandlePair::DecompressChunk(void* buf, unsigned int size)
{
    unsigned int original = size;

    unsigned int hData = 0;
    Storm_624(&hData);
    Storm_609(hData, buf, size);

    unsigned int hOut = 0;
    Storm_624(&hOut);

    Storm_628(hOut, hData, h1, h0);

    Storm_638(hOut, buf, size, &size);
    if (size < original)
        memset((char*)buf + size, 0, original - size);

    Storm_606(hOut);
    Storm_606(hData);
}

//----------------------------------------------------------------------------
//  0x6F7E2EF0 - verify the file's embedded, compressed signature block:
//  locate it (CheckSignatureTrailer), independently recompute what it
//  should hash to (ComputeContentDigest, into an 0xBB/0x0B-padded local
//  buffer the size of the real signature), decompress the embedded block
//  itself into a second local of the same size, and report whether the
//  two agree byte for byte.  Register/stack split (ecx, edx, then six
//  stack dwords) and retn 0x18 both read directly off the call site in
//  LoadReplayHeader above.
//----------------------------------------------------------------------------
int __fastcall Sub_6F7E2EF0(void* pathBuf, void* handle, unsigned int contentLen,
                            const void* tableTag, unsigned int sigSize,
                            const void* paramTag, unsigned int reserved10,
                            unsigned int prefixLen)
{
    unsigned int sigOffset;
    if (!CheckSignatureTrailer(handle, contentLen, sigSize, &sigOffset))
        return 0;

    unsigned char* computedSig = (unsigned char*)_alloca(sigSize);
    memset(computedSig, 0xBB, sigSize);
    computedSig[sigSize - 1] = 0x0B;
    ComputeContentDigest(pathBuf, handle, sigOffset, computedSig, prefixLen);

    void* embeddedSig = _alloca(sigSize);
    memcpy(embeddedSig, (char*)handle + sigOffset + 4, sigSize);

    DecodeHandlePair handles;
    Storm_624(&handles.h0);
    Storm_624(&handles.h1);
    handles.RegisterBlobs((void*)tableTag, sigSize, (void*)paramTag, reserved10);
    handles.DecompressChunk(embeddedSig, sigSize);

    int result = memcmp(embeddedSig, computedSig, sigSize) == 0;

    Storm_606(handles.h1);
    Storm_606(handles.h0);

    return result;
}

//----------------------------------------------------------------------------
//  0x6F6527E0 - a trivial passthrough onto SaveGameReadSlotRecord, kept as
//  its own reconstruction (not inlined away) because the dump gives it its
//  own address and its own three-instruction body distinct from the real
//  reader's.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadReplaySlotRecord(CDataStore* store,
                                            Net::GAMESETUP_SLOTRECORD* out)
{
    SaveGameReadSlotRecord(store, out);
    return store;
}

//----------------------------------------------------------------------------
//  The version-info quad CNetData::LoadReplayHeader reads straight off the
//  stream object before the record loop, plus everything the record loop
//  itself fills in - all contiguous from +0x960 to +0xAC8 of the session.
//----------------------------------------------------------------------------
struct SReplayHeaderInfo
{
    int                  m_field230;    // +0x960
    int                  m_field234;    // +0x964
    int                  m_version;     // +0x968 - masked to 31 bits
    //  +0x96C is the very next byte the record loop below starts filling in
    //  (memcpy target `this + 0x96C`) - no padding gap here at all, so
    //  unlike the other structs in this file there is nothing to name.
};

//----------------------------------------------------------------------------
//  0x6F54B880
//----------------------------------------------------------------------------
struct SNetReplaySession
{
    int LoadReplayHeader(const char* filename);
};

int SNetReplaySession::LoadReplayHeader(const char* filename)
{
    void* stream = (char*)this + 0x618;
    CDataStore* ds = (CDataStore*)stream;

    typedef int  (__thiscall *ReplayStreamOpenFn)(void*, const char*);
    typedef void (__thiscall *ReplayStreamRewindFn)(void*);

    int opened = ((ReplayStreamOpenFn)(*(void***)stream)[0x3C / 4])(stream, filename);
    if (!opened)
        return 0;

    SReplayHeaderInfo* info = (SReplayHeaderInfo*)((char*)this + 0x960);
    info->m_field230 = ds->GetField230();
    info->m_field234 = ds->GetField234();
    info->m_version  = ds->GetVersion() & 0x7FFFFFFF;

    int field23C = ds->GetField23C();
    *(int*)((char*)this + 0xAB8) = field23C;
    *(int*)((char*)this + 0xAC0) = field23C;
    *(unsigned int*)((char*)this + 0xAC4) = (unsigned int)ds->GetVersion() >> 31;
    *(int*)((char*)this + 0xABC) = 0;

    if (info->m_version == 0)
    {
        void* handle = 0;
        unsigned int size = 0;
        if (Storm_281(0, filename, &handle, &size, 0, 1, 0))
        {
            char pathBuf[0x104];
            ExtractPathTail(filename, pathBuf, 0x104);

            unsigned int field95C = *(unsigned int*)((char*)this + 0x95C);
            int rc = Sub_6F7E2EF0(pathBuf, handle, size,
                                  &g_replayDecompressTable, 0x80,
                                  &g_replayDecompressParam, 4,
                                  field95C);
            *(int*)((char*)this + 0xABC) = rc;
            Storm_280(handle);
        }
    }

    SNetPlayerDataTable* table = (SNetPlayerDataTable*)((char*)this + 0xAA8);
    if (table->m_data != 0)
    {
        //  Net::PLAYERDATA - the RTTI-shaped tag SMemFree's own type
        //  descriptor argument carries (`.?AUPLAYERDATA@Net@@`).
        static const char s_typeName[] = ".?AUPLAYERDATA@Net@@";
        SMemFree(table->m_data, s_typeName, -2, 0);
    }
    table->m_capacity = 0;
    table->m_count = 0;
    table->m_data = 0;

    unsigned char recordType = 0;
    ds->ReadByte(&recordType);

    if (recordType == 0x10)
    {
        unsigned char rec[0xDC];
        rec[0x05] = 0;
        rec[0x15] = 0;
        rec[0x20] = 0;
        rec[0x40] = 0;
        rec[0x50] = 0;
        ReadReplayType0x10Record(ds, (SReplayType0x10Record*)rec);
        memcpy((char*)this + 0x96C, rec + 0x20, 0xB8);
        table->Append(1, rec + 5);

        ds->ReadByte(&recordType);
        while (recordType == 0x16)
        {
            unsigned char rec16[0x20];
            rec16[0x01] = 0;
            rec16[0x11] = 0;
            ReadReplayType0x16Record(ds, (SReplayType0x16Record*)rec16);
            table->Append(1, rec16 + 1);

            ds->ReadByte(&recordType);
        }

        if (recordType == 0x19)
        {
            Net::GAMESETUP_SLOTRECORD slotRec;
            slotRec.m_length = 0;
            ReadReplaySlotRecord(ds, &slotRec);

            *(unsigned short*)((char*)this + 0xA24) = slotRec.m_length;
            memcpy((char*)this + 0xA26, slotRec.m_data, slotRec.m_length);

            *(int*)((char*)this + 0x62C) = 0;
            ((SNetDataStates*)this)->SetReplayState(1);
            return 1;
        }
    }

    ((ReplayStreamRewindFn)(*(void***)stream)[0x1C / 4])(stream);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F54BAE0 - the TLS-indexed wrapper: look up this thread's own CNetData
//  record (the same slot-0xD/+0x10/+0x8 sequence NetDataRecords() already
//  names, see netdata.h) and forward straight into LoadReplayHeader above.
//----------------------------------------------------------------------------
int __fastcall LoadReplayHeaderFromTls(const char* filename)
{
    SNetReplaySession* session = (SNetReplaySession*)NetDataRecords();
    return session->LoadReplayHeader(filename);
}

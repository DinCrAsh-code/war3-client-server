//============================================================================
//  See cdatastore.cpp for the class-level notes.
//============================================================================
#ifndef CDATASTORE_H
#define CDATASTORE_H

//----------------------------------------------------------------------------
//  CDataStore (`??_7CDataStore@@6B@` in the dump) - a small streaming byte
//  reader/writer view over externally-pool-owned storage, reached from
//  ContainsW3GSBytePacketCheck's own w3gs-header parse
//  (ReadByte/ReadWord/EnsureReadable/~CDataStore) and, in the sibling
//  NetProviderLTCP__Method_0xC dump, from a write-side family this session
//  did not reconstruct (sub_6F658030 and friends - see
//  docs/targets/NetProviderLTCP__Method_0xC.md).
//
//  Field layout is exact but the *meaning* of m_field8/m_field0xC/m_field10
//  is not fully rederived - EnsureReadable's own growth-vs-refill logic
//  (cdatastorereadensure.cpp) makes internal sense read as "already-known
//  length" (m_field10), "backing capacity" (m_field8) and a third bound
//  (m_field0xC) it compares against together, but nothing in this call tree
//  proves what real quantities those are, only how they interact.
//----------------------------------------------------------------------------
class CDataStore
{
public:
    //  The implicit default constructor every other TU in this repo
    //  already relies on (a `CDataStoreCache1460`/`CDataStore` local with
    //  no initializer) - declaring the two-argument overload below
    //  suppresses the compiler-generated one, so it has to be spelled out
    //  explicitly.  Trivial on purpose: it stamps only the vtable (the
    //  compiler's own job for any polymorphic class), exactly what the
    //  implicit one already did everywhere else in this repo compiles it.
    CDataStore() {}

    //  0x6F4434A0 is NOT a real two-argument constructor overload - see
    //  Net/netdata_recordview_ctor.cpp's own header note.  The dump's own
    //  calling convention (self in ecx, edx unused, `(ptr, len)` both on
    //  the stack, `retn 8`) and its real call sites
    //  (Net/pausecallchainthunks.cpp's CNetData_DispatchOneQueuedRecord,
    //  building a short-lived second view over a record it already wrapped
    //  once) are a placement-construct-then-field-assign helper, not a
    //  member function the compiler could have emitted this shape for -
    //  modelled there as `ConstructRecordView_6F4434A0` instead.

    //  0x6F2C8A10 - if m_field0xC != -1 (this instance owns pool-recycled
    //  storage), hands the pool back &m_field4/&m_field8/&m_field0xC
    //  through the cross-module recycle pointer (off_6F932C0C - the same
    //  one the not-reconstructed write-side family uses to check a buffer
    //  back in).
    ~CDataStore();

    virtual void m_reservedSlot0() {}
    virtual void m_reservedSlot1() {}
    //  vtable+8 (slot 2) - EnsureReadable's own "not enough room" fallback:
    //  asks for more backing storage for [offset, offset+size), given
    //  read/write access to m_field4/m_field8/m_field0xC. Nothing in
    //  either call tree reaches a concrete override's body.
    virtual int Grow(unsigned int offset, unsigned int size, void** pField4, int* pField8, int* pField0xC) { return 0; }

    //  0x6F4C2B30 - returns 1 if [offset, offset+size) already sits inside
    //  the current buffer with nothing to do, 0 if m_readPos/m_field10 was
    //  just adjusted instead (either because there is not yet enough
    //  declared data, or Grow() was asked and declined) - the caller's own
    //  raw pointer read is only valid to perform when this returns 0.
    int EnsureReadable(unsigned int offset, unsigned int size);

    //  0x6F4C2C10 - read one byte at m_readPos into *out, advancing
    //  m_readPos, if EnsureReadable(m_readPos, 1) allows it. Returns
    //  `this` either way (unconditionally, even when the read was
    //  skipped) - matches a stream-style `>>` return.
    CDataStore* ReadByte(unsigned char* out);

    //  0x6F4C2C70 - the same shape as ReadByte for a 2-byte read.
    CDataStore* ReadWord(unsigned short* out);

    //  0x6F4C2C40 - a *second* two-byte reader at its own address, the same
    //  shape as ReadWord.  Same /OPT:ICF-less duplication as
    //  ReadDwordAlt/ReadDwordAlt2/ReadDwordAlt3 below.  Reached from
    //  CWidget::Load.  See cdatastore_readwordalt.cpp.
    CDataStore* ReadWordAlt(unsigned short* out);

    //  0x6F4C2D30 - the same shape as ReadByte for a 4-byte read.
    CDataStore* ReadDword(unsigned int* out);

    //  0x6F4C2CD0 - a *second* four-byte reader at its own address, the
    //  mirror of CDataStoreScratch::WriteDwordAlt on the write side and
    //  reached from the same save/load family (ReadCFloat,
    //  CWidget::LoadHeader).  Two four-byte readers rather than one is
    //  ordinary for this image, which was linked without /OPT:ICF
    //  (agentdefaults.cpp).  See cdatastore_readdwordalt.cpp.
    CDataStore* ReadDwordAlt(unsigned int* out);

    //  0x6F4C2CA0 - a *third* four-byte reader, again byte for byte the
    //  same body, reached from the item load path (item_load.cpp).  Four
    //  identical four-byte writers exist on the scratch side for the same
    //  reason (cdatastorescratch.h).
    CDataStore* ReadDwordAlt2(unsigned int* out);

    //  0x6F4C2D00 - a *fourth*, reached only from ReadHandleTriple
    //  (item_loadhandletriple.cpp) for the third of its three dwords.  Not
    //  in any dump this repo has, so it is redirected rather than
    //  reconstructed - see cdatastore_readdwordalt2.cpp.
    CDataStore* ReadDwordAlt3(unsigned int* out);

    //  0x6F4C2DE0 - the float twin of ReadDword: the same EnsureReadable
    //  guard and the same 4-byte advance, but the value moves through the
    //  x87 stack (`fld` / `fstp [out]`) because it is a `float`.  Reached
    //  from the sprite load path (LoadSprite, 0x6F4D9940) and 0x6F290850.
    CDataStore* ReadFloat(float* out);

    //  0x6F4C2E10 - retn 8.  Read a NUL-terminated string into `dest`,
    //  never writing more than `size` bytes and always terminating.  The
    //  copy runs in whatever contiguous run EnsureReadable hands back and
    //  re-asks for more when it runs out mid-string.
    CDataStore* ReadString(char* dest, unsigned int size);

    //  0x6F4C2FD0 - retn 8.  Read `size` raw bytes into `dest`, again a
    //  contiguous run at a time.
    CDataStore* ReadRaw(void* dest, unsigned int size);

    //  0x6F4C3420 - a bare one-instruction `jmp` onto ReadRaw, the linker's
    //  alias for a second inline expansion.  A **member**, not a free
    //  `__fastcall` taking the store: the shipped body inherits ReadRaw's
    //  own `retn 8`, every call site pushes both arguments and leaves `this`
    //  in ecx, and declared the other way round it took `dest` in edx,
    //  cleaned four bytes too few, and could not tail-call (see
    //  cdatastore_readrawat18.cpp).
    CDataStore* ReadRawAlias(void* dest, unsigned int size);

    //  0x6F4C32A0 - a dword-granularity twin of ReadRaw: `count` dwords
    //  (`count * 4` bytes) read a contiguous run at a time, but the minimum
    //  and the rounding are 4 bytes rather than ReadRaw's 1, so a run is
    //  always dword-aligned and never splits a dword across two calls.
    //  Reached from CCameraWar3::Load for its raw flag-byte arrays (nine
    //  bytes' worth of `SCameraProp` flag words, one dword each).  See
    //  cdatastore_readdwordarray.cpp.
    CDataStore* ReadDwordArray(void* dest, unsigned int count);

    //  0x6F4C3120 - a fifth /OPT:ICF-less duplicate of ReadDwordArray's own
    //  body, reached from the JASS compiler's own function-table load
    //  (src/Jass/jassloadfuncnames.cpp).
    CDataStore* ReadDwordArrayAlt(void* dest, unsigned int count);

    //  0x6F534A40 - the save-stream format version, a plain field read at
    //  +0x238.  The object the load path passes around is bigger than the
    //  0x18 bytes named below - see datastoreversion.cpp, which declares
    //  the reach rather than padding this class out with 0x220 bytes of
    //  reserved block nothing else in the codebase touches.
    int GetVersion();

    //  0x6F534A20 / 0x6F534A30 / 0x6F534A60 - three more fields of the same
    //  versioned-stream overlay GetVersion (+0x238) already reaches, all
    //  plain reads at +0x230/+0x234/+0x23C.  Reached only from
    //  CNetData::LoadReplayHeader (Net/netdata_versionfields.cpp), which is
    //  its own translation unit for the same reason GetVersion's is: its
    //  caller reaches it with a real call rather than getting it inlined.
    //  Meaning not derived past "read together as one version-info quad
    //  right before the replay header's own record loop starts".
    int GetField230();
    int GetField234();
    int GetField23C();

    void* m_field4;    // +0x04
    int   m_field8;    // +0x08
    int   m_field0xC;  // +0x0C
    int   m_field10;   // +0x10
    int   m_readPos;   // +0x14
};

#endif

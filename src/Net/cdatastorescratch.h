//============================================================================
//  See cdatastorescratch.cpp for the class-level notes.
//============================================================================
#ifndef CDATASTORESCRATCH_H
#define CDATASTORESCRATCH_H

//----------------------------------------------------------------------------
//  CDataStoreScratch (`??_7CDataStoreScratch@@6B@` in the dump) - a growable
//  write buffer reached from GameAction_0x26_PacketHandler
//  (gameaction0x26_handler.cpp) while appending one player's action bytes
//  into a shared queue. Field layout mirrors CDataStore's own append-buffer
//  shape (m_field4/m_field8/m_field0xC/m_field10) closely enough that it is
//  almost certainly a sibling/derivative of the same original class, but the
//  vtable slots this class actually calls do not line up with CDataStore's
//  own (slot 0 from the constructor, slot 3 from every writer) - modelled
//  as its own independent class rather than guessed at as an inheritance
//  relationship the dump does not establish.
//
//  Two virtual slots are known to be *called* (slot 0's own real override,
//  and slot 3's, the "grow to fit N more bytes" the writers all share) but
//  not what either *does* - not reconstructed this session, left as
//  placeholder overrides so the constructor's and writers' own call shapes
//  still compile and match instruction-for-instruction.
//----------------------------------------------------------------------------
class CDataStoreScratch
{
public:
    //  0x6F654190
    CDataStoreScratch(void* field18, void* field1C);
    //  0x6F654270
    ~CDataStoreScratch();

    //  vtable slot 0 - called once, from the constructor, on this object's
    //  own not-yet-fully-constructed vtable (so it resolves to this class's
    //  own override, not any base class's - a legal, if unusual, call from
    //  a constructor). Real body not reconstructed - the disassembly of
    //  sub_6F654190 itself proves the constructor zeroes m_field4/8/0xC/10
    //  and only ever *stores* field18/field1C into m_field18/m_field1C, so
    //  nothing else in this class's own compiled body ever points the
    //  buffer at the caller-provided [field18, field18+field1C) region -
    //  that has to be this call's job. An empty placeholder here left every
    //  WriteByte/WriteWord/WriteRaw call writing through a null base
    //  pointer (m_field4 stays 0), which this codebase's own build only
    //  ever *compiles* - it never links, so nothing caught it. The
    //  injection repo's build does link and run it for real, where a
    //  near-null write is a live bug, not a no-op. Implemented here from
    //  the only consistent reading of m_field18/m_field1C's own field
    //  layout (they exist for no other purpose in this class's tree) -
    //  not verified against sub_6F654190's own real body, which this
    //  session never located a dump for.
    virtual void OnConstruct(void** pField4, int* pField8, int* pField0xC)
    {
        *pField4 = m_field18;
        *pField8 = 0;
        *pField0xC = (int)(unsigned int)m_field1C;
    }

    //  Slots 1 and 2 - never observed called from anything this call tree
    //  reaches, but Grow's own real vtable slot (3, confirmed by every
    //  writer's `[vtable+0xC]` call) proves two more virtuals sit here.
    //  Placeholders only, purely to hold Grow at the right offset.
    virtual void Reserved1() {}
    virtual void Reserved2() {}

    //  vtable slot 3 - "grow the buffer so at least `size` more bytes fit
    //  at `offset`", called from every writer (WriteByte/WriteWord/
    //  WriteDword/WriteRaw/EnsureWritable). Real body not reconstructed
    //  (ServerTock_0x27_PacketHandler's own dump proves the two trailing
    //  dwords are a real, callsite-varying pair of arguments, not always 0 -
    //  EnsureWritable (sub_6F2C8380) forwards its own two extra parameters
    //  straight through). Returns int (every caller tests eax against 0),
    //  kept `int` rather than a guessed-at enum.
    virtual int Grow(unsigned int offset, unsigned int size, void** pField4,
                      int* pField8, int* pField0xC, int arg8, int argC) { return 0; }

    //  0x6F4C2160
    CDataStoreScratch* WriteByte(unsigned char value);
    //  0x6F4C2210
    CDataStoreScratch* WriteWord(unsigned short value);
    //  0x6F4C21B0 - a *second* two-byte writer at its own address, the same
    //  shape as WriteWord.  Same /OPT:ICF-less duplication as
    //  WriteDwordAlt/WriteDwordAlt2/WriteDwordAlt3 below.  Reached from
    //  CWidget::Save.  See cdatastorescratch_writewordalt.cpp.
    CDataStoreScratch* WriteWordAlt(unsigned short value);
    //  0x6F4C2360 - same shape as WriteByte/WriteWord, one size up again (a
    //  4-byte write).
    CDataStoreScratch* WriteDword(unsigned int value);
    //  0x6F4C22C0 - a *second* four-byte writer at its own address, six
    //  instructions shorter than WriteDword and reached from a different
    //  set of callers (the widget/agent save path: WriteCFloat,
    //  CWidget::SaveHeader).  Two four-byte writers rather than one is
    //  ordinary for this image, which was linked without /OPT:ICF
    //  (agentdefaults.cpp) - see cdatastorescratch_writedwordalt.cpp.
    CDataStoreScratch* WriteDwordAlt(unsigned int value);

    //  0x6F4C2310 / 0x6F4C2270 - two *more* four-byte writers, byte for
    //  byte the same body again, at two more addresses.  The item save path
    //  (item_save.cpp) calls all three plus WriteDword itself, which is what
    //  makes them four genuinely separate functions rather than one the
    //  linker failed to fold: this image was linked without /OPT:ICF (see
    //  agentdefaults.cpp), so identical bodies at different addresses stay
    //  identical bodies at different addresses.  Each gets a translation
    //  unit of its own for the same reason WriteDwordAlt does.
    CDataStoreScratch* WriteDwordAlt2(unsigned int value);
    CDataStoreScratch* WriteDwordAlt3(unsigned int value);
    //  0x6F664A40 - fastcall(this, kind byte in edx) + two stack dwords,
    //  retn 8: the first stack dword (closest to the return address, so the
    //  first C++ parameter after the register one) is a 16-bit length
    //  promoted to a dword; the second is pushed by every known call site
    //  but never read inside this function's own body - real stack-argument
    //  padding, not a guess, the same way other unread pushed values are
    //  elsewhere in this tree. Writes a fixed three-part record header: a
    //  literal 0xF7 tag byte, the caller-supplied `kind` byte, then the
    //  length - the same WriteByte/WriteByte/WriteWord fluent chain every
    //  other writer in this class uses (see w3gsaction_tier26.cpp, its own
    //  translation unit per this file's placement rule for callers still
    //  reachable from a different original module).
    CDataStoreScratch* __fastcall WriteRecordHeader(unsigned char kind, unsigned short length, unsigned int unused);

    //  0x6F4C25A0 - chunked write: writes `size` bytes from `src` in pieces
    //  no larger than m_field0xC each (the same "chunk" bound EnsureWritable
    //  and the per-write growth checks all compare against), growing the
    //  buffer (through the virtual Grow, not EnsureWritable) whenever a
    //  chunk does not already fit. Skips the memcpy on the degenerate
    //  dest==src case the shipped code itself special-cases. Returns `this`,
    //  matching WriteByte/WriteWord's own fluent shape (formerly declared
    //  `void`, before this session located this function's own dump).
    CDataStoreScratch* WriteRaw(const void* src, unsigned int size);
    //  0x6F4C2B20 - a bare `jmp sub_6F4C25A0` tail-calling WriteRaw with the
    //  same two arguments; declared as its own method purely so its own
    //  address gets its own funcmap.py entry and score.
    CDataStoreScratch* TailCallWriteRaw(const void* src, unsigned int size);

    //  0x6F4C2940 - WriteRaw's dword-granularity twin, the write-side mirror
    //  of CDataStore::ReadDwordArray (cdatastore.h): `count` dwords
    //  (`count * 4` bytes), the same chunked-copy loop as WriteRaw but with
    //  a 4-byte floor and every chunk rounded down to a multiple of 4, so a
    //  run never splits a dword across two Grow calls.  Reached from
    //  CCameraWar3::Save for its own raw 9-dword blocks (camerasaveload.cpp)
    //  - the same call sites ReadDwordArray's own header note cites on the
    //  read side.
    CDataStoreScratch* WriteDwordArray(const void* src, unsigned int count);

    //  0x6F4C2BA0 - retn 4.  Append a NUL-terminated string, terminator
    //  included; a null one sets Storm's last error to 0x57 and writes
    //  nothing.
    CDataStoreScratch* WriteString(const char* src);

    //  0x6F2C8380 - the non-virtual "does [offset, offset+size) already fit"
    //  check every writer's own inlined guard shares, factored out here as
    //  its own out-of-line function (same EnsureReadable/EnsureWritable
    //  shape as CDataStore::EnsureReadable) and called once, up front, by
    //  WriteRaw before its own per-chunk loop. `arg8`/`argC` are forwarded
    //  verbatim to Grow, same as Grow's own trailing two parameters.
    int EnsureWritable(unsigned int offset, unsigned int size, int arg8, int argC);

    //  0x6F4C1BB0 - three independent, optional out-params reading
    //  m_field4/m_field10/m_field0xC (in that order - not the field's own
    //  declaration order). No caller in this call tree passes all three, or
    //  even two; kept general since nothing establishes a narrower shape.
    void QueryBufferState(void** outField4, int* outField10, int* outField0xC);

    //  0x6F4C1FB0 - the dword twin of WriteWordAt: a random-access 4-byte
    //  write at an explicit caller-supplied offset rather than an append at
    //  m_field10.  Same growth check, same fluent `this` return.  Reached
    //  from CAgent::Save's preamble (0x6F472300) and CAgent::Save itself,
    //  both of which reserve a length slot and patch it afterwards.
    CDataStoreScratch* WriteDwordAt(unsigned int offset, unsigned int value);

    //  0x6F4C2470 - WriteDword's float twin: appends 4 bytes at m_field10
    //  with the same growth check, but the value arrives on the x87 stack
    //  (`fld [esp+arg_0]` / `fstp dword ptr [...]`) rather than in a GP
    //  register, which is what a `float` by value gets.
    CDataStoreScratch* WriteFloat(float value);

    //  0x6F4C1E70 - a random-access word write: like WriteWord, but at an
    //  explicit caller-supplied offset rather than appending at m_field10 -
    //  used to patch a length field into a slot already reserved earlier in
    //  the same caller. Same growth check as every other writer, same
    //  fluent `this` return.
    CDataStoreScratch* WriteWordAt(unsigned int offset, unsigned short value);

    void* m_field4;    // +0x04
    int   m_field8;    // +0x08
    int   m_field0xC;  // +0x0C
    int   m_field10;   // +0x10
    int   m_field14;   // +0x14, initialised to -1
    void* m_field18;   // +0x18, the constructor's first argument
    void* m_field1C;   // +0x1C, the constructor's second argument
};

#endif

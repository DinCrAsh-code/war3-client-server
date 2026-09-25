//============================================================================
//  CAnimCameraObj - RTTI name only (agent_worktrees/classes/0x6F955758.json,
//  vtable @ 0x6F955758, ??_7CAnimCameraObj@@6B@).  A one-slot vtable and a
//  flat, callee-free constructor - no other dump reaches this class this
//  session, so only what the constructor and the one vtable slot actually
//  touch is declared; nothing past +0xD8 is established, and the whole
//  +0x21..+0x6F range (and the three isolated dwords at +0x0C/+0x84/+0xA0/
//  +0xC8) is left untouched by the constructor itself, so it is left
//  unnamed reserved space here too rather than invented.
//
//  No calls at all in the constructor's own raw_asm (worktree_store.py show
//  0x6F51CD10) - not even a base-class one - so this is either the root of
//  its own hierarchy or derives from something with a wholly-elided,
//  no-op default constructor; either way there is no base-class subobject
//  for this file to declare.
//
//  The RTTI type-descriptor string table also carries a *second*, unrelated
//  name - `??_R0?AUCAnimCameraObjStatus@@@8` / ".?AUCAnimCameraObjStatus@@"
//  (agent_worktrees/names.json) - a plain `struct` (the `U` tag), not this
//  class's own `??_R0?AUCAnimCameraObj@@@8`.  Nothing in this constructor's
//  raw_asm references it (no `typeid`/RTTI cast instructions appear in a
//  flat field-init body), and grepping asm/, src/ and user_knowledge.json
//  for either address or name turns up nothing - see the write-up.  +0x20
//  is the one field a caller can reach at all (through the vtable slot
//  below), and its own store is a single byte (`mov [eax+20h], cl`), which
//  is what CAnimCameraObjStatus is taken to be here: a byte-sized status/
//  state enum embedded directly in the object rather than a named nested
//  type this build can spell in pre-C++11 syntax, so it stays a plain
//  `unsigned char` with a name that says what it is.
//============================================================================

class CAnimCameraObj
{
public:
    //  slot 0 / +0x00 (0x6F51CDC0) - `lea eax,[ecx+0x20]; retn`.  A trivial
    //  accessor into this object's own +0x20 status byte - the only field
    //  this class's single vtable slot can reach, and the only field this
    //  constructor sets with anything other than a 4-byte store.  Named for
    //  the CAnimCameraObjStatus RTTI string sitting nearby in the type-
    //  descriptor table (see the file header) rather than left as a bare
    //  offset.
    virtual unsigned char* GetStatus();

    CAnimCameraObj();

    //  +0x04..+0x1C - four reachable dwords ahead of the status byte.
    //  +0x0C is left unset by the constructor (a real gap, not an
    //  omission - see the file header) and is not named.
    int            m_field04;      // +0x04 - 0
    int            m_field08;      // +0x08 - 0
    char           m_reserved0C[4];// +0x0C - untouched by the constructor
    int            m_field10;      // +0x10 - 0
    int            m_field14;      // +0x14 - 0
    int            m_field18;      // +0x18 - -1 (an "unset handle/index" sentinel, the same -1 this constructor repeats three more times below)
    int            m_field1C;      // +0x1C - 1

    unsigned char  m_status;       // +0x20 - see GetStatus() above

    //  +0x21..+0x6F - untouched by the constructor.
    char           m_reserved21[0x70 - 0x21];

    //  +0x70..+0x9C - three floats, then a -1/1 sentinel pair bracketed by
    //  plain zeroed dwords, with one more untouched dword (+0x84) in the
    //  middle of the run.  The same -1/1 pair recurs twice more below
    //  (+0xAC/+0xB0 and +0xD4/+0xD8); nothing in this pass ties the three
    //  occurrences to a shared sub-struct - see the file header.
    float          m_field70;      // +0x70 - 0.0f
    float          m_field74;      // +0x74 - 0.0f
    float          m_field78;      // +0x78 - 0.0f
    int            m_field7C;      // +0x7C - 0
    int            m_field80;      // +0x80 - 0
    char           m_reserved84[4];// +0x84 - untouched by the constructor
    int            m_field88;      // +0x88 - 0
    int            m_field8C;      // +0x8C - 0
    int            m_field90;      // +0x90 - -1
    int            m_field94;      // +0x94 - 1
    int            m_field98;      // +0x98 - 0
    int            m_field9C;      // +0x9C - 0

    //  +0xA0..+0xD8 - the same shape again: a -1/1 sentinel pair, three
    //  floats, then a zero/zero/[gap]/zero/zero/-1/1 tail.
    char           m_reservedA0[4];// +0xA0 - untouched by the constructor
    int            m_fieldA4;      // +0xA4 - 0
    int            m_fieldA8;      // +0xA8 - 0
    int            m_fieldAC;      // +0xAC - -1
    int            m_fieldB0;      // +0xB0 - 1
    float          m_fieldB4;      // +0xB4 - 0.0f
    float          m_fieldB8;      // +0xB8 - 0.0f
    float          m_fieldBC;      // +0xBC - 0.0f
    int            m_fieldC0;      // +0xC0 - 0
    int            m_fieldC4;      // +0xC4 - 0
    char           m_reservedC8[4];// +0xC8 - untouched by the constructor
    int            m_fieldCC;      // +0xCC - 0
    int            m_fieldD0;      // +0xD0 - 0
    int            m_fieldD4;      // +0xD4 - -1
    int            m_fieldD8;      // +0xD8 - 1
};

//  0x6F51CDC0.
unsigned char* CAnimCameraObj::GetStatus()
{
    return &m_status;
}

//  0x6F51CD10.  Forty instructions, no calls: a flat field-init constructor
//  written out here in the dump's own store order.  DIFFERS, not EXACT -
//  there is no `asm/` dump for this address (see the file header), so
//  nothing scores it automatically; this was checked by hand against the
//  build's own `build/animcameraobj.asm` listing versus
//  `worktree_store.py show 0x6F51CD10`'s raw_asm, instruction by
//  instruction.  Every one of the 40 instructions is present with the
//  right opcode, offset and (after ordering the two int-literal
//  assignments `m_field18 = -1;` before `m_field1C = 1;` to match, worth
//  getting exactly right - it flips which of ecx/edx/esi this compiler
//  picks for the shared 0/-1/1 constants) the right register - `ecx=0`,
//  `esi=-1`, `edx=1`, one `fldz` left on the FPU stack across all six
//  float stores, popped only by the last. What does not match is pure
//  instruction *scheduling*: this compiler groups all four `esi`-valued
//  (-1) stores together right after the float block, ahead of the vtable
//  stamp, where the shipped stream interleaves each of them into its own
//  field's own local block (mirrored by the `edx`-valued stores forming a
//  second such batch afterward) - the same "independent stores batched by
//  which register already holds their value" reordering
//  docs/msvc-vc8-idioms.md's own "Independent chains, reordered" entry
//  already documents as not source-addressable. Same instructions, same
//  registers, same values, different order - DIFFERS pending angr
//  IDENTICAL verification, not EXACT.
CAnimCameraObj::CAnimCameraObj()
{
    m_field04 = 0;
    m_field08 = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = -1;
    m_field1C = 1;

    //  +0x00 - this class's own vtable, stamped here (mid-body in the
    //  shipped stream, after the four leading dwords and the -1/1 pair)
    //  rather than at entry, matching the dump's own instruction order.

    m_field70 = 0.0f;
    m_field74 = 0.0f;
    m_field78 = 0.0f;
    m_field7C = 0;
    m_field80 = 0;
    m_field88 = 0;
    m_field8C = 0;
    m_field90 = -1;
    m_field94 = 1;
    m_field98 = 0;
    m_field9C = 0;

    m_fieldA4 = 0;
    m_fieldA8 = 0;
    m_fieldAC = -1;
    m_fieldB0 = 1;
    m_fieldB4 = 0.0f;
    m_fieldB8 = 0.0f;
    m_fieldBC = 0.0f;
    m_fieldC0 = 0;
    m_fieldC4 = 0;
    m_fieldCC = 0;
    m_fieldD0 = 0;
    m_fieldD4 = -1;
    m_fieldD8 = 1;

    m_status = 0;
}

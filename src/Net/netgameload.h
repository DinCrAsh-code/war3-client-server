//============================================================================
//  NetGameLoadSource / NetGameLoadFile - where a saved game or a replay is
//  read from.  Both names are stamped: 0x6F5341D0 writes
//  `??_7NetGameLoadFile@@6B@` (0x6F957B38) on the way in and 0x6F534240
//  writes `??_7NetGameLoadSource@@6B@` (0x6F957B28) on the way out, which is
//  the derived-then-base pair a destructor makes.  An `ida_query vtable` on
//  each says the base's three slots are all `_purecall` and the file's are
//  0x6F5342A0 / 0x6F5342B0 / 0x6F5342E0, so the base is abstract with
//  exactly three virtuals and nothing else:
//
//      [0] Open()                       - is the source usable?
//      [1] Read(void* dest, unsigned n) - read n bytes
//      [2] Size()                       - 64 bits, in edx:eax
//
//  A third derived class, `NetGameLoadStore` (0x6F957B48), exists in the
//  image and is not reached from these dumps.
//
//  No `virtual` is declared here and none of the three slots is
//  reconstructed: neither the base's own vtable nor the file's is in any
//  dump, so the reads go through hand-written slot dispatches at the one
//  call site that makes them (gamedataloadheader.cpp), which is the shape
//  tools/vtable_dispatch_audit.py judges.
//============================================================================
#ifndef NETGAMELOAD_H
#define NETGAMELOAD_H

//: ??_7NetGameLoadFile@@6B@ / ??_7NetGameLoadSource@@6B@ - stamped, never
//: dispatched through from this build.  Local, for the reason
//: src/Game/savegameglobals.cpp gives.
extern void* g_vftableNetGameLoadFile[1];
extern void* g_vftableNetGameLoadSource[1];

struct NetGameLoadSource
{
    void* m_vtable;     // +0x00
};

struct NetGameLoadFile
{
    void* m_vtable;     // +0x00
    void* m_handle;     // +0x04 - OsCreateFile's answer, or INVALID_HANDLE

    //  0x6F5341D0 - open `path` for reading.
    NetGameLoadFile(const char* path);
    //  0x6F534240 - close it, unless it never opened.
    ~NetGameLoadFile();
};

//----------------------------------------------------------------------------
//  The header at the front of one saved game.  0x44 bytes in the version the
//  image writes; the older one is 0x40 and has no +0x30, which is why
//  0x6F534750 reads it into a local of its own shape and scatters the six
//  dwords into the five fields past +0x28 with a zero in the hole.
//----------------------------------------------------------------------------
//  The 0x28 bytes both versions share, as its own type: 0x6F534750 copies
//  them out of the caller's header into its local with one `rep movsd`,
//  which is a struct assignment and not ten field copies.
struct SGameLoadHeaderPrefix
{
    char         m_magic[0x1C];     // +0x00 - "Warcraft III recorded game\x1A"
    unsigned int m_headerSize;      // +0x1C - 0x40 or 0x44, and checked
    unsigned int m_dataSize;        // +0x20 - refused if it exceeds the file
    unsigned int m_version;         // +0x24 - 0 or 1, and nothing else
};

struct SGameLoadHeader
{
    SGameLoadHeaderPrefix m_prefix; // +0x00
    unsigned int m_word28;          // +0x28
    unsigned int m_word2C;          // +0x2C
    unsigned int m_word30;          // +0x30 - zero in a version 0 header
    unsigned int m_word34;          // +0x34
    unsigned int m_word38;          // +0x38
    unsigned int m_word3C;          // +0x3C
    unsigned int m_crc;             // +0x40 - over the header with this field
                                    //         zeroed
};

//  The version 0 header on disk: the same 0x28-byte prefix, then six dwords
//  with no hole, and the CRC last.  0x40 bytes, which is what its own CRC
//  covers.
struct SGameLoadHeaderV0
{
    SGameLoadHeaderPrefix m_prefix; // +0x00
    unsigned int m_word28;          // +0x28
    unsigned int m_word2C;          // +0x2C
    unsigned int m_word34;          // +0x30 - lands at +0x34 in the new one
    unsigned int m_word38;          // +0x34
    unsigned int m_word3C;          // +0x38
    unsigned int m_crc;             // +0x3C
};

//  0x6F534750 - Net/gamedataloadheader.cpp.  0 on success; 8..0xF name which
//  step refused.
int __fastcall GameDataReadHeader(NetGameLoadSource* source, SGameLoadHeader* header);

//  0x6F535050 - the block loop the load runs.  Not reconstructed; redirected
//  (Game/gamerestartthunks.cpp).  A **member**: the shipped body is `retn 4`
//  against a receiver in ecx and one push, so a free `__fastcall` taking
//  both in registers pushes nothing where the callee cleans four bytes.
struct SGameDataLoad
{
    int LoadBlocks(NetGameLoadSource* source);

    //  0x6F535410 - Net/gamedataloadfile.cpp.  Also a member, and for the
    //  same reason: `retn 4` against a receiver in ecx and one push.  It was
    //  a free `__fastcall(void*, const char*)` for one build, which
    //  abi_audit.py's FATAL bucket caught - "target cleans 4 bytes, this
    //  build cleans 0".
    int LoadFile(const char* path);
};

//  0x6F534A70 - Net/gamedatapeekheader.cpp.  Open `path` far enough to read
//  its header and hand back five of its fields; every out-pointer optional.
int __fastcall GameDataPeekHeader(const char* path, unsigned int* outWord30,
                                  unsigned int* outWord34, unsigned int* outWord38,
                                  unsigned int* outWord3C, unsigned int* outCrc);

//  0x6F4434D0 - Game/savegameprobe.cpp.
int __fastcall SaveGameHeaderProbe(const char* path, unsigned int maxVersion);

#endif

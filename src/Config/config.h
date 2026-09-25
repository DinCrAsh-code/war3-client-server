//============================================================================
//  The named game-configuration lookup: section -> key -> indexed values.
//
//  What the call tree proves about the shape of this subsystem:
//
//    * a CConfigFile owns one name-keyed hash table of sections at +0x08;
//    * a section owns a second name-keyed hash table of entries at +0x18;
//    * an entry carries its value string at +0x18 and a link to the next
//      value of the same key at +0x1C, so one key can hold a list of values
//      that callers index into (`ValueAt`);
//    * a CConfigSource is a thin wrapper holding one CConfigFile at +0x04,
//      and CMiscCustom holds two CConfigSources that are tried in order.
//
//  The two hash tables are the two byte-identical lookups at 0x6F4D05B0 and
//  0x6F4D1D30 that docs/targets/sub_6F4D05B0.md reconstructed, instantiated
//  on the two node types this target finally identified: before this dump
//  nothing said what either table held, so they were named HashKeyNodeA and
//  HashKeyNodeB.  0x6F4D1E60 chains them - table-of-sections first, then the
//  found section's own table - which is what named them.
//
//  Container definitions stay deliberately separate from `src/storm.h`: see
//  the header comment in src/hashfind.cpp for why that file does not use the
//  canonical TSHashTable.
//============================================================================
#ifndef CONFIG_H
#define CONFIG_H

#include "CFloat.h"

//  Storm.dll, imported by ordinal.  509 is a bounded case-insensitive string
//  compare (SStrnICmp); 590 is the string hash the tables key on - no ordinal
//  table found so far names it, so it keeps a descriptive name and the
//  ordinal in the comment rather than a guessed one.
//
//  Declared as ordinary C++ rather than `extern "C"`, like storm.h's SMem*:
//  the pipeline recovers readable names from the comment MASM writes beside a
//  mangled EXTRN, and an undecorated C symbol has none.
int __stdcall SStrnICmp(const char* a, const char* b,
                        unsigned int count);            // ordinal 509
unsigned int __stdcall Storm_590(const char* s);        // string hash

//----------------------------------------------------------------------------
//  The intrusive link, in the encoding AgileTypeIsDerivedFrom established:
//  `m_prevlink` holds the predecessor's *object* pointer when positive and
//  the complement of the terminator's own address when negative, so one sign
//  test both identifies the terminator and ends a walk.
//----------------------------------------------------------------------------
struct SNameLink
{
    SNameLink*  m_next;
    int         m_prevlink;
};

//----------------------------------------------------------------------------
//  One hash bucket: an explicit list whose nodes are found by adding a
//  per-list link offset (held at run time, not baked into the type) to the
//  node's own address.
//----------------------------------------------------------------------------
template <class T>
struct SNameBucket
{
    int         m_linkoffset;
    SNameLink   m_terminator;

    //  Link(0) is load-bearing: routing the terminator through the same
    //  accessor as every other node - rather than naming `&m_terminator`
    //  directly - is what puts the shipped code's separate `lea` of the
    //  terminator address in the stream.  See docs/msvc-vc8-idioms.md,
    //  "Storm's intrusive containers".
    SNameLink* Link(T* node) const
    {
        return node ? (SNameLink*)((char*)node + m_linkoffset)
                    : (SNameLink*)&m_terminator;
    }

    //  Masked, for entering a walk: positive is the last node, negative the
    //  terminator, i.e. the bucket is empty.
    T* Tail() const
    {
        int prevlink = Link(0)->m_prevlink;
        return prevlink > 0 ? (T*)prevlink : 0;
    }

    //  Raw mid-walk, because the loop tests the sign itself and the shipped
    //  code does not mask it there.
    T* Prev(T* node) const
    {
        return (T*)Link(node)->m_prevlink;
    }
};

//----------------------------------------------------------------------------
//  Chained hash table, keyed by name.  The bucket count is always a power of
//  two and only the mask is stored; -1 means the table was never allocated.
//
//  Ptr() is declared here and defined only in src/hashfind.cpp, which
//  explicitly instantiates it: every other translation unit therefore sees a
//  declaration and has to emit the call the shipped code has.
//----------------------------------------------------------------------------
template <class T>
class CNameHashTable
{
public:
    T* Ptr(const char* name) const;

private:
    char            m_reserved00[0x1C];  // vftable, all-nodes list, probe run
    SNameBucket<T>* m_buckets;           // 0x1C - the array's m_data
    char            m_reserved20[4];     // the array's m_chunk
    unsigned int    m_mask;              // 0x24 - 0xFFFFFFFF = never allocated
};

//----------------------------------------------------------------------------
//  One value of one key.  A key with several values keeps them on the
//  m_nextValue chain, oldest first as far as the call tree can tell: nothing
//  here builds the chain, only walks it.
//----------------------------------------------------------------------------
struct CConfigEntry
{
    unsigned int    m_hashval;           // 0x00
    SNameLink       m_link;              // 0x04
    char            m_reserved0C[0x08];
    const char*     m_key;               // 0x14
    const char*     m_value;             // 0x18
    CConfigEntry*   m_nextValue;         // 0x1C
};

//----------------------------------------------------------------------------
//  One `[Section]` of a configuration file, holding its own table of keys.
//----------------------------------------------------------------------------
struct CConfigSection
{
    unsigned int                m_hashval;      // 0x00
    SNameLink                   m_link;         // 0x04
    char                        m_reserved0C[0x08];
    const char*                 m_key;          // 0x14
    CNameHashTable<CConfigEntry> m_entries;     // 0x18
};

//----------------------------------------------------------------------------
//  A parsed configuration file.
//
//  Every member below is __fastcall, which is what puts `this` in ecx and the
//  first argument in edx.  The shipped build went one better and passed some
//  of these arguments in eax/esi, a register choice no calling convention
//  offers and only a whole-program (LTCG) code generator can make - see
//  docs/msvc-vc8-idioms.md, "Arguments in eax and esi".  __fastcall is the
//  closest reachable convention and lines up ecx and the first argument.
//----------------------------------------------------------------------------
class CConfigFile
{
public:
    //  0x6F4D1E60
    CConfigEntry* __fastcall FindEntry(const char* section,
                                       const char* key) const;
    //  0x6F4D1EB0 - `key` first because the shipped code takes it in the
    //  argument register and reads `section` back off the stack.
    const char* __fastcall ValueAt(const char* key, const char* section,
                                   unsigned int index) const;
    //  0x6F4D20C0
    const char* __fastcall GetValue(const char* section, const char* key,
                                    unsigned int index) const;
    //  0x6F4D2040
    int __fastcall GetFloat(const char* section, const char* key,
                            CFloat* out, unsigned int index) const;
    //  0x6F4D21B0
    int __fastcall HasKey(const char* section, const char* key) const;
    //  0x6F4D21A0 - does this file carry the named section at all.  Callers
    //  outside this call tree; kept here since it reaches into m_sections
    //  the same way every other member does.
    int __fastcall HasSection(const char* name) const;

    //  0x6F4D1F80/0x6F4D2000 - resolve-and-parse in one step: look the value
    //  up with ValueAt and, only if found, replace the caller-supplied
    //  default with the parsed result.  `key` first, `section` second - the
    //  same order ValueAt itself takes, and the order the shipped call site
    //  (edx register = key, one stack dword = section) actually uses, unlike
    //  the section-first order every *other* member of this class has.  Both
    //  reached only through CWidget__MoveForward's call tree via Storm's own
    //  singleton bootstrap (ConstructStormSingletonA querying a game-config
    //  value at construction time) rather than through any widget/pathing
    //  code directly - see docs/targets/CWidget__MoveForward.md session 6.
    //
    //  0x6F4D1F80 - the int sibling, built on ParseConfigInt.
    int __fastcall GetIntValue(const char* key, const char* section,
                               int* out, unsigned int index) const;
    //  0x6F4D2000 - the CFloat sibling, built on Storm's own string-to-float
    //  ordinal (Storm_574) rather than this codebase's CFloatFromString -
    //  the only place so far that reaches Storm for this instead of parsing
    //  it in Game.dll's own code.
    int __fastcall GetFloatValue(const char* key, const char* section,
                                 CFloat* out, unsigned int index) const;

private:
    char                            m_reserved00[8];
    CNameHashTable<CConfigSection>  m_sections;     // 0x08
};

//----------------------------------------------------------------------------
//  One source of configuration values, wrapping one file.
//----------------------------------------------------------------------------
class CConfigSource
{
public:
    //  0x6F6F3410
    int HasValue(const char* section, const char* key) const;
    //  0x6F6F34F0
    CFloat GetFloat(const char* section, const char* key, int index) const;
    //  0x6F6F3490 - the int sibling of GetFloatRaw, and like it a member
    //  that takes the *key* first: it forwards straight into
    //  CConfigFile::GetIntValue, whose own argument order is the inverted
    //  one this header notes at 0x6F4D1F80.  configsourcegetint.cpp.
    int GetInt(const char* key, const char* section, int index) const;

    //  0x6F6F3570 - the string sibling of GetInt/GetFloat, and the one
    //  CMiscCustom::GetString forwards to.  A four-instruction forwarder
    //  into CConfigFile::GetValue; batch G of the JASS_PauseGame closure
    //  owns its body, so Misc/pausegamecalleethunks.cpp redirects it.
    const char* GetString(const char* section, const char* key,
                          int index) const;

    //  0x6F6F34C0 - the plain-hardware-float sibling of GetFloat, and the
    //  one member of this class that takes (key, section) rather than
    //  (section, key): it forwards straight into CConfigFile::GetFloatValue
    //  without renaming either register, exactly as GetDefaultConfigFloat
    //  (gamedefaultconfig.cpp) does.  It hands the value back through ST(0)
    //  rather than through a CFloat return buffer, so the caller gets a real
    //  `float` and never sees the deterministic encoding.
    float GetFloatRaw(const char* key, const char* section,
                      unsigned int index) const;

    //  0x6F6FA990 - a plain (non-virtual) checksum fold over this source's
    //  own file: `CGameDataContextChecksumProviderHash`-shaped provider
    //  glue, called by CMiscCustom::ComputeChecksum (CMiscCustom.cpp) for
    //  each of its two sources and folded through the same
    //  rol(hash ^ contribution, 3) combine every other checksum-provider
    //  class in this family already uses.  Left a naked thunk: its own
    //  body walks CConfigFile's section/entry hash tables through a
    //  generic callback-taking enumerator (sub_6F4D0570) this batch judged
    //  out of scope - see configsourcechecksum.cpp for the detail.
    unsigned int ComputeChecksum() const;

private:
    //  This class's own vtable pointer - confirmed, not guessed: reading
    //  CMiscCustom's own teardown helper (0x6F002130,
    //  CMiscCustom::TeardownSources in CMiscCustom.cpp) shows both
    //  m_pPrimary and m_pSecondary released through an indirect call on
    //  `*(void**)source` (`mov eax,[ecx] ; mov edx,[eax] ; call edx`), the
    //  same virtual-slot-0-with-flags-argument shape this repo's own
    //  DeleteSelf overrides already use elsewhere (e.g.
    //  Item/abilitycustomdata_deleteself.cpp). CConfigSource's own vtable
    //  is not otherwise reconstructed by this batch, so the field stays a
    //  reserved gap rather than a named method here.
    char            m_reserved00[4];
    CConfigFile*    m_pFile;            // 0x04
};

//  0x6F6F0520 - "12.5" and friends into the deterministic float.
CFloat __fastcall CFloatFromString(const char* text);

#endif

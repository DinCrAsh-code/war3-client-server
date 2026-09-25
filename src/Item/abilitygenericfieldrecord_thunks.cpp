//============================================================================
//  Naked redirects for the callees abilitygenericfieldrecord.cpp declares
//  and calls but does not reconstruct - all out of scope for the CAbility
//  depth<=5 closure batch that wrote that file, per its own header note.
//  Every argument count is the shipped call site's own push count, read
//  off each callee's own `retn` in its asm/ dump.
//============================================================================

struct SGenericValue;

class SGenericValueDecodeTarget
{
public:
    int  DecodeBool(SGenericValue* value, int flag);
    int  DecodeIntA(SGenericValue* value);
    void DecodePtr(void* out, SGenericValue* value);
    int  DecodeIntB(SGenericValue* value);
};

struct SGenericFieldValueTableFull
{
    void         FillValue(const char* fieldName, unsigned int tag0, unsigned int* out);
    unsigned int ProbeAlternateTag(unsigned int tag);
};

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F707DB0 (`retn 8`) - ecx = decode target, two stack args (the value
//  pointer, the bool flag).
ADDR_THUNK(int SGenericValueDecodeTarget::DecodeBool(SGenericValue* value, int flag), 0x6F707DB0)

//  0x6F70C490 (`retn 4`) - ecx = decode target, one stack arg (the value
//  pointer).
ADDR_THUNK(int SGenericValueDecodeTarget::DecodeIntA(SGenericValue* value), 0x6F70C490)

//  0x6F70C550 (`retn 8`) - ecx = decode target, two stack args (the out
//  pointer, the value pointer).
ADDR_THUNK(void SGenericValueDecodeTarget::DecodePtr(void* out, SGenericValue* value), 0x6F70C550)

//  0x6F708040 (`retn 4`) - ecx = decode target, one stack arg (the value
//  pointer).
ADDR_THUNK(int SGenericValueDecodeTarget::DecodeIntB(SGenericValue* value), 0x6F708040)

//  0x6F005E00 and 0x6F4C9060 are NOT here: both are real, already-scored
//  reconstructions under other names elsewhere in src/
//  (SFieldNameSource::ResolveFormatted lives in abilitygenericfieldrecord.cpp
//  itself now; 0x6F4C9060 is Widget/widget_selectiondepthcache.cpp's own
//  ForwardTaggedFieldTriple) - see CLAUDE.md's "one shipped address, two
//  C++ names" defect class.  A naked thunk here would have been a silent
//  duplicate of real work, caught by tools/gen_rename_from_funcmap.py's own
//  conflict check.

//  0x6F25B160 (`retn 0x0C`) - ecx = the table, three stack args (field
//  name, tag0, the caller's 5-word out buffer).
ADDR_THUNK(void SGenericFieldValueTableFull::FillValue(const char* fieldName, unsigned int tag0, unsigned int* out), 0x6F25B160)

//  0x6F25B1D0 (`retn 4`) - ecx = the table, one stack arg (the tag to
//  probe).
ADDR_THUNK(unsigned int SGenericFieldValueTableFull::ProbeAlternateTag(unsigned int tag), 0x6F25B1D0)

#undef ADDR_THUNK

//  abilitygenericfieldrecord.cpp declares these two `extern "C"` (no
//  C++ mangling), so the thunk's own declaration has to match exactly or
//  it binds under a different, unreferenced symbol.  A third,
//  0x6F4C8790, was here too (as `FetchFieldValue`) but is NOT: it is a
//  real, already-scored reconstruction under a different name
//  (Widget/widget_selectiondepthcache.cpp's own QueryTaggedFieldAsInt) -
//  see the note above FillValue/ProbeAlternateTag.
extern "C" {

#define ADDR_THUNK_FASTCALL(sig, addr)  \
    __declspec(naked) sig               \
    {                                   \
        __asm { mov eax, addr }         \
        __asm { jmp eax }               \
    }

//  0x6F25B100 (`retn` bare, 0 stack args) - a plain byte-swap of its one
//  ecx argument; edx never read.
ADDR_THUNK_FASTCALL(unsigned int __fastcall DecodeSingleTag(unsigned int tag), 0x6F25B100)

//  0x6F251C60 (`retn` bare, 0 stack args) - ecx = field name, edx = the
//  0x104-byte output buffer.
ADDR_THUNK_FASTCALL(void __fastcall FormatFieldNameIntoBuffer(const char* fieldName, void* buffer), 0x6F251C60)

#undef ADDR_THUNK_FASTCALL

}  // extern "C"

//  LazyInitGameDataCacheField14 (0x6F25EFE0) and LazyInitGameDataCacheField40
//  (0x6F25AC40) used to be naked redirects here.  Both are real
//  reconstructions now - Item/abilitydatabase_lazyinit.cpp and
//  Item/abilitycustomdata_lazyinit.cpp - see
//  docs/notes/checksum-provider-registry.md (batch D).

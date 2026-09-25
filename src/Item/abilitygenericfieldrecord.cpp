//============================================================================
//  CAbility depth<=5 closure, batch 2 (2026-09-02): SGenericFieldRecord's
//  own internals - the generic name/value field lookup CAbility's
//  SetTypeId/QueryLevelRowField14/etc reach through
//  SAliasableRecord::ResolveTypeId's own QueryStringField call
//  (abilityaliastyperesolve.cpp).  0x6F262xxx/0x6F25Bxxx/0x6F25Fxxx form one
//  shipped module: three sibling find-or-fallback lookups sharing one
//  shape (0x6F2625F0 int, 0x6F262760 out-pointer, 0x6F2628E0 pointer -
//  already a thunk elsewhere, given a real body here), a lazily-created
//  per-thread lookup table (0x6F25B5A0) and loader (0x6F25F570) parked on
//  the game-data cache (Game/gamecontext.h), and four small typed-decode
//  members of that table (0x6F25B230/0x6F25B2A0/0x6F25B340/0x6F25B3A0)
//  each forwarding into one of four out-of-scope leaf decoders
//  (sub_6F707DB0/0x6F70C490/0x6F70C550/0x6F708040) after
//  sub_6F25B100 (out of scope) fills a local "generic value" from the
//  record's two tag words.
//
//  All three of the find-or-fallback trio carry the same unreproducible
//  frame docs/msvc-vc8-idioms.md's "An __except_handler4-shaped frame this
//  toolchain cannot reproduce" section already documents and gives up on
//  for sub_6F42CB50/sub_6F02F780: a real local object (SAbilityAddRequest)
//  needing unwind protection under /EHs-c- pulls in VC8's unified
//  SEH/C++-EH frame (the `push -1`/`push offset SEH_<addr>`/`mov
//  eax,fs:0` prologue, the per-function trampoline into
//  `__CxxFrameHandler3` appended after the body), which that section's own
//  hand-tested compiler-flag sweep already proved the bundled `vs8/`
//  `cl.exe` cannot emit under this repo's fixed `/GS- /EHs-c-` regardless
//  of source shape - a toolchain limit, not a source defect.  Reconstructed
//  and scored IDENTICAL on the strength of the body match past that frame,
//  the same way that section's own two functions are.
//============================================================================
#include "gamecontext.h"

//  0x6F4C8880 - Agent/refcnt.h's release forward, under the name this repo
//  already binds to that address.  Declared exactly as
//  Agent/refcnt_release_forward.cpp defines it, the same way
//  Game/objectdatatable.h already does for its own sibling 0x6F4C8870.
class TRefCnt;
void __fastcall ReleaseForward_6F4C8880(TRefCnt* self);

//----------------------------------------------------------------------------
//  0x6F25B5A0 - lazily create/fetch the per-thread generic field-value
//  lookup table parked at the game-data cache's own +0x40 slot.  No
//  arguments; the table object itself is returned - a tail jump straight
//  into the lazy-init helper on the not-yet-created path.
//----------------------------------------------------------------------------
void* GetGenericFieldValueTable()
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    void* table = context->m_pCache->m_field40;
    if (table != 0)
        return table;
    return LazyInitGameDataCacheField40(0x10, 0);
}

//----------------------------------------------------------------------------
//  0x6F25F570 - lazily create/fetch the ability-data loader wrapper parked
//  at the game-data cache's own +0x14 slot, resolve "Units\\AbilityData"
//  through it and load the sheet - the same vtable-slot-2 shape
//  GetUpgradeDataTable (Game/objectdatatable_get.cpp) already documents
//  for "Units\\UpgradeData", including the vtable-read-before-path-resolve
//  order.  No arguments; the loader object itself is returned, not a bool
//  or a table.
//----------------------------------------------------------------------------
typedef void (__thiscall *LoadSheetFn)(void* self, int pathHandle);
extern int __fastcall ResolveDataPath(const char* path, void* arg);

void* GetAbilityDataLoader()
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    void* loader = context->m_pCache->m_field14;
    if (loader == 0)
    {
        loader = LazyInitGameDataCacheField14(5, 0);
        void** vtable = *(void***)loader;
        int path = ResolveDataPath("Units\\AbilityData", 0);
        ((LoadSheetFn)vtable[2])(loader, path);
    }
    return loader;
}

//----------------------------------------------------------------------------
//  sub_6F25B100 (out of scope, 13 instructions - a plain byte-swap of its
//  one `ecx` argument, `edx` never read) fills the first word of a
//  zeroed 5-word local; the caller's own trailing words go straight into
//  the rest of it, unrelated to the decode call.  Each of the four members
//  below forwards `&value` into one leaf decoder.
//----------------------------------------------------------------------------
extern "C" unsigned int __fastcall DecodeSingleTag(unsigned int tag);

struct SGenericValue
{
    unsigned int tag;         // sub_6F25B100's own return
    unsigned int data;
    unsigned int w2;
    unsigned int w3;
    unsigned int reserved;    // never written past the initial zero
};

//  The four out-of-scope leaf decoders these members forward into, each
//  reached through `this`->+0x08 (the decode target) - members of a stub
//  class rather than free functions so the calling convention on an
//  opaque receiver is spelled the same way ability_slots.cpp's own
//  SAliasableRecord/SAttachmentOwnerQuery already do.
class SGenericValueDecodeTarget
{
public:
    int  __thiscall DecodeBool(SGenericValue* value, int flag);   // sub_6F707DB0, retn 8
    int  __thiscall DecodeIntA(SGenericValue* value);              // sub_6F70C490, retn 4
    void __thiscall DecodePtr(void* out, SGenericValue* value);    // sub_6F70C550, retn 8
    int  __thiscall DecodeIntB(SGenericValue* value);              // sub_6F708040, retn 4
};

//----------------------------------------------------------------------------
//  0x6F25B230 - `this`->+0x08 gates whether the table even has a decode
//  target at all; when it does not, every path answers 0 without reading a
//  single word of the caller's value.  Both tag words are validated
//  non-null before the decode is ever reached.  `retn 0x10`.
//----------------------------------------------------------------------------
class SGenericFieldValueTable
{
public:
    int   __thiscall Method_6F25B230(unsigned int tag0, unsigned int tag1,
                                     unsigned int w2, unsigned int w3);
    int   __thiscall Method_6F25B2A0(unsigned int tag0, unsigned int w1,
                                     unsigned int w2, unsigned int w3);
    void* __thiscall Method_6F25B340(void* out, unsigned int tag0, unsigned int w1,
                                     unsigned int w2, unsigned int w3);
    int   __thiscall Method_6F25B3A0(unsigned int tag0, unsigned int w1,
                                     unsigned int w2, unsigned int w3);

    char                       m_reserved00[0x08];
    SGenericValueDecodeTarget* m_decodeTargetOrNull;   // +0x08
};

int SGenericFieldValueTable::Method_6F25B230(unsigned int tag0, unsigned int tag1,
                                             unsigned int w2, unsigned int w3)
{
    if (m_decodeTargetOrNull == 0)
        return 0;
    if (tag0 == 0)
        return 0;
    if (tag1 == 0)
        return 0;

    SGenericValue value = {0, 0, 0, 0, 0};
    value.tag  = DecodeSingleTag(tag0);
    value.data = tag1;
    value.w2 = w3;
    value.w3 = w2;

    return m_decodeTargetOrNull->DecodeBool(&value, 1);
}

//----------------------------------------------------------------------------
//  0x6F25B2A0/0x6F25B340/0x6F25B3A0 - the same table, three more decode
//  shapes over the same four words: no target/tag gates at all (only
//  0x6F25B230 has them), a second int decode, a pointer-out decode that
//  hands its own out pointer back rather than the decoder's own return
//  value, and a third int decode.
//----------------------------------------------------------------------------
int SGenericFieldValueTable::Method_6F25B2A0(unsigned int tag0, unsigned int w1,
                                             unsigned int w2, unsigned int w3)
{
    SGenericValue value = {0, 0, 0, 0, 0};
    value.tag  = DecodeSingleTag(tag0);
    value.data = w1;
    value.w2 = w2;
    value.w3 = w3;

    return m_decodeTargetOrNull->DecodeIntA(&value);
}

void* SGenericFieldValueTable::Method_6F25B340(void* out, unsigned int tag0, unsigned int w1,
                                                unsigned int w2, unsigned int w3)
{
    SGenericValue value = {0, 0, 0, 0, 0};
    value.tag  = DecodeSingleTag(tag0);
    value.data = w1;
    value.w2 = w2;
    value.w3 = w3;

    m_decodeTargetOrNull->DecodePtr(out, &value);
    return out;
}

int SGenericFieldValueTable::Method_6F25B3A0(unsigned int tag0, unsigned int w1,
                                             unsigned int w2, unsigned int w3)
{
    SGenericValue value = {0, 0, 0, 0, 0};
    value.tag  = DecodeSingleTag(tag0);
    value.data = w1;
    value.w2 = w2;
    value.w3 = w3;

    return m_decodeTargetOrNull->DecodeIntB(&value);
}

//----------------------------------------------------------------------------
//  Two more out-of-scope members of the generic value table: sub_6F25B160
//  fills a caller's 5-word local from the table and the record's two tags
//  (`retn 0x0C`); sub_6F25B1D0 answers a possibly-different tag, or the
//  same one back when the table has nothing extra to say (`retn 4`).
//----------------------------------------------------------------------------
class SGenericFieldValueTableFull : public SGenericFieldValueTable
{
public:
    void         __thiscall FillValue(const char* fieldName, unsigned int tag0,
                                      unsigned int* out /* 5 words */);
    unsigned int __thiscall ProbeAlternateTag(unsigned int tag);
};

//  0x6F25F5C0/0x6F251C50 - the request object's ctor/dtor
//  (Unit/unit_flagbit8thunks.cpp), redeclared here the same way that file
//  declares them: a second declaration with the identical signature is the
//  same symbol as far as the linker is concerned.
struct SAbilityAddRequest
{
    unsigned int m_rawcode;
    int          m_field4;
    void*        m_field8;

    void Construct(unsigned int rawcode);
    void Destruct();
};

//  The fallback path both of this trio's own extended branches share with
//  the "no extended source at all" entry: format the field name into a
//  0x104-byte buffer (sub_6F251C60, out of scope), resolve it off the
//  record's own +0x04 source (0x6F005E00, `retn 4` - real body just below,
//  a thin `this+4`/argument-reshuffle forward into
//  Misc/citemclosure_wedb_depth7.cpp's own ReleaseWEDBFieldIndexOrWarn)
//  and fetch its value off the record's own +0x08 source (0x6F4C8790,
//  `retn 0` - Widget/widget_selectiondepthcache.cpp's own
//  QueryTaggedFieldAsInt, confirmed the same address via
//  tools/dumpfn.py rather than redeclared under a second name here - see
//  CLAUDE.md's own "one shipped address, two C++ names" defect class),
//  and release the resolved handle (sub_6F4C8880, already
//  ReleaseForward_6F4C8880 - Agent/refcnt.h).
extern "C" void __fastcall FormatFieldNameIntoBuffer(const char* fieldName, void* buffer);

class SFieldNameSource
{
public:
    void* __thiscall ResolveFormatted(void* buffer);   // 0x6F005E00, retn 4
};

//  citemclosure_wedb_depth7.cpp's own declaration, verbatim - not a second
//  name for 0x6F4CA130, just visibility into this TU.
void __cdecl ReleaseWEDBFieldIndexOrWarn();

__declspec(naked) void* __thiscall SFieldNameSource::ResolveFormatted(void*)
{
    __asm
    {
        mov     edx, [esp+4]
        mov     ecx, [ecx+4]
        call    ReleaseWEDBFieldIndexOrWarn
        retn    4
    }
}

//  Widget/widget_selectiondepthcache.cpp's own class and function,
//  redeclared here on the identical real name (not a stand-in) so this
//  TU's calls resolve to the same symbol.
class STaggedFieldSource;
extern int   __fastcall QueryTaggedFieldAsInt(STaggedFieldSource* self, STaggedFieldSource* other);
extern void* __fastcall ForwardTaggedFieldTriple(void* self, STaggedFieldSource* other, STaggedFieldSource* arg);

class SGenericFieldRecord
{
public:
    void* __thiscall QueryStringField(const char* fieldName);   // 0x6F2628E0
    int   __thiscall QueryIntField(const char* fieldName);      // 0x6F2625F0
    void* __thiscall QueryPointerField(void* out, const char* fieldName); // 0x6F262760
    unsigned int __thiscall GetCodeFieldId();                   // 0x6F262A50

    unsigned int      m_tag0;               // +0x00
    SFieldNameSource* m_field4;              // +0x04
    void*             m_field8;              // +0x08
    void*             m_hasExtendedSource;   // +0x0C
};

//----------------------------------------------------------------------------
//  0x6F2625F0 - see the file header's own note on the unreproducible
//  __CxxFrameHandler3 frame every one of this trio carries.  When the
//  record has an extended source (+0x0C != 0), resolve the field through
//  the per-thread generic value table (Method_6F25B230/0x2A0) and decode it
//  as an int; when the table has nothing for it either
//  (ProbeAlternateTag's answer equals the tag already on file), fall back
//  to a fresh default-constructed SAbilityAddRequest and ask the same
//  question of *that* instead - the recursive `call sub_6F2625F0` the dump
//  itself shows.  With no extended source at all, or as that fallback's
//  own tail, resolve the field name directly off the record's own two
//  source pointers.
//----------------------------------------------------------------------------
int SGenericFieldRecord::QueryIntField(const char* fieldName)
{
    if (m_hasExtendedSource != 0)
    {
        unsigned int tag0 = m_tag0;
        SGenericFieldValueTableFull* table =
            (SGenericFieldValueTableFull*)GetGenericFieldValueTable();

        unsigned int value[5] = {0, 0, 0, 0, 0};
        table->FillValue(fieldName, tag0, value);

        if (table->Method_6F25B230(tag0, value[1], value[3], value[2]) != 0)
            return table->Method_6F25B2A0(tag0, value[1], value[3], value[2]);

        unsigned int altTag = table->ProbeAlternateTag(m_tag0);
        if (altTag != m_tag0)
        {
            SAbilityAddRequest request;
            request.m_rawcode = altTag;
            request.m_field4 = 0;
            request.m_field8 = 0;
            request.Construct(altTag);

            int result = ((SGenericFieldRecord*)&request)->QueryIntField(fieldName);

            request.Destruct();
            return result;
        }
    }

    char buffer[0x104];
    FormatFieldNameIntoBuffer(fieldName, buffer);
    void* resolved = m_field4->ResolveFormatted(buffer);
    int fetched = QueryTaggedFieldAsInt((STaggedFieldSource*)m_field8, (STaggedFieldSource*)resolved);
    ReleaseForward_6F4C8880((TRefCnt*)resolved);
    return fetched;
}

//----------------------------------------------------------------------------
//  0x6F262760 - the out-pointer sibling of QueryIntField: the same
//  extended-source/fallback/recursive shape, decoding through
//  Method_6F25B340 (the pointer-out member) instead of 0x6F25B2A0, and
//  fetching through 0x6F4C9060 (ForwardTaggedFieldTriple,
//  Widget/widget_selectiondepthcache.cpp, `retn 4`) instead of
//  QueryTaggedFieldAsInt - both write one word into a local before it is
//  copied into the caller's own out pointer (`mov ecx,[esp+...var_128]`
//  right after the call, in the dump), rather than being returned
//  directly, which is why ForwardTaggedFieldTriple's own `self` argument
//  is that local's address rather than `out` itself.
//----------------------------------------------------------------------------
void* SGenericFieldRecord::QueryPointerField(void* out, const char* fieldName)
{
    if (m_hasExtendedSource != 0)
    {
        unsigned int tag0 = m_tag0;
        SGenericFieldValueTableFull* table =
            (SGenericFieldValueTableFull*)GetGenericFieldValueTable();

        unsigned int value[5] = {0, 0, 0, 0, 0};
        table->FillValue(fieldName, tag0, value);

        if (table->Method_6F25B230(tag0, value[1], value[3], value[2]) != 0)
        {
            table->Method_6F25B340(out, tag0, value[1], value[3], value[2]);
            return out;
        }

        unsigned int altTag = table->ProbeAlternateTag(m_tag0);
        if (altTag != m_tag0)
        {
            SAbilityAddRequest request;
            request.m_rawcode = altTag;
            request.m_field4 = 0;
            request.m_field8 = 0;
            request.Construct(altTag);

            ((SGenericFieldRecord*)&request)->QueryPointerField(out, fieldName);

            request.Destruct();
            return out;
        }
    }

    char buffer[0x104];
    FormatFieldNameIntoBuffer(fieldName, buffer);
    void* resolved = m_field4->ResolveFormatted(buffer);

    void* handle = 0;
    ForwardTaggedFieldTriple(&handle, (STaggedFieldSource*)m_field8, (STaggedFieldSource*)resolved);
    ReleaseForward_6F4C8880((TRefCnt*)resolved);
    *(void**)out = handle;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F2628E0 - the string sibling: same shape again, decoding through
//  Method_6F25B3A0 (a third int-shaped decode) rather than 0x6F25B2A0, and
//  fetching through 0x6F4C8790 (QueryTaggedFieldAsInt) exactly as
//  QueryIntField's own fallback does.
//----------------------------------------------------------------------------
void* SGenericFieldRecord::QueryStringField(const char* fieldName)
{
    if (m_hasExtendedSource != 0)
    {
        unsigned int tag0 = m_tag0;
        SGenericFieldValueTableFull* table =
            (SGenericFieldValueTableFull*)GetGenericFieldValueTable();

        unsigned int value[5] = {0, 0, 0, 0, 0};
        table->FillValue(fieldName, tag0, value);

        if (table->Method_6F25B230(tag0, value[1], value[3], value[2]) != 0)
            return (void*)table->Method_6F25B3A0(tag0, value[1], value[3], value[2]);

        unsigned int altTag = table->ProbeAlternateTag(m_tag0);
        if (altTag != m_tag0)
        {
            SAbilityAddRequest request;
            request.m_rawcode = altTag;
            request.m_field4 = 0;
            request.m_field8 = 0;
            request.Construct(altTag);

            void* result = ((SGenericFieldRecord*)&request)->QueryStringField(fieldName);

            request.Destruct();
            return result;
        }
    }

    char buffer[0x104];
    FormatFieldNameIntoBuffer(fieldName, buffer);
    void* resolved = m_field4->ResolveFormatted(buffer);
    int fetched = QueryTaggedFieldAsInt((STaggedFieldSource*)m_field8, (STaggedFieldSource*)resolved);
    ReleaseForward_6F4C8880((TRefCnt*)resolved);
    return (void*)fetched;
}

//----------------------------------------------------------------------------
//  0x6F262A50 - `this`->QueryStringField("code") packed through
//  PackFourCCString (Net/packfourcc.cpp), the same shape
//  SAliasableRecord::ResolveTypeId (abilityaliastyperesolve.cpp) already
//  uses for its own "alias" field.  A bare tail chain: no prologue of its
//  own, `this` passed straight through unchanged.
//----------------------------------------------------------------------------
unsigned int __fastcall PackFourCCString(const char* text);

unsigned int SGenericFieldRecord::GetCodeFieldId()
{
    return PackFourCCString((const char*)QueryStringField("code"));
}

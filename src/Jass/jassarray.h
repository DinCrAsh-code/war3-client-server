//============================================================================
//  JassArray - the JASS interpreter's own array handle (Jass2/Instance.h),
//  real class name from its own vftable (`??_7JassArray@@6B@`, stamped by
//  sub_6F45A3D0 - jassarrayref.cpp).  A vtable pointer plus one
//  TSGrowableArray<void*>-shaped element vector, laid out in place (not as
//  a named member) rather than through a class boundary: sub_6F45A100's own
//  `lea ecx,[this+4]` and sub_6F3A3BE0's own callers reach it at the flat
//  offsets +4/+8/+0xC/+0x10, never through anything that looks like a
//  qualified member access, and JassArray::SetCount (below) has its own
//  0x2000 clamp and memset-not-placement-new shape the generic
//  Containers/tsarray.inl template does not - so it is not reused as-is.
//============================================================================
#ifndef JASSARRAY_H
#define JASSARRAY_H

//  ??_7JassArray@@6B@ - stamped in place rather than declared through a
//  `virtual`: JassArray's real vtable has however many slots this
//  closure's own one call site (0x6F45A100 through JassArrayRefSite's own
//  vtable-slot-0 dispatch) does not reach, and none of them are in this
//  dump, so declaring the `virtual`s here would make this build emit its
//  own, shorter vtable and fail to link - the vtable-you-cannot-declare
//  idiom (docs/msvc-vc8-idioms.md).  funcmap.DATA binds the real address.
extern void* const g_vftJassArray;

class JassArray
{
public:
    //  0x6F45A100 - grow/shrink the element vector to `count`, clamped to
    //  0x2000 elements, zero-filling any newly-uncovered slots.  `retn 4`.
    void SetCount(unsigned int count);

    //  0x6F459710 - `retn 8`, bounds-checked element store: silently no-ops
    //  if index >= m_elementCount.  jassarrayaccess.cpp.
    void SetElement(unsigned int index, void* value);
    //  0x6F459730 - `retn 4`, bounds-checked element load: returns null if
    //  index >= m_elementCount.  jassarrayaccess.cpp.
    void* GetElement(unsigned int index);

    void*         m_vtable;        // +0x00
    unsigned int  m_elementAlloc;  // +0x04
    unsigned int  m_elementCount;  // +0x08
    void**        m_elementData;   // +0x0C
    unsigned int  m_elementChunk;  // +0x10
};

//  0x6F3A3BE0 - free `elements.m_elementData` if set and zero
//  alloc/count/data (not m_elementChunk).  Takes a pointer to the
//  three-word {alloc,count,data} sub-object directly (`this+4` of a
//  JassArray, per sub_6F45A3D0's own call site) rather than a whole
//  JassArray, so it is declared free rather than as a JassArray member.
//  `retn 0`.
struct JassArrayElements
{
    unsigned int  m_alloc;
    unsigned int  m_count;
    void**        m_data;
};
void __fastcall JassArrayReleaseElements(JassArrayElements* elements);

//----------------------------------------------------------------------------
//  The one AST node shape this session's own slice of the JASS evaluator
//  reaches: a variable reference that may be an array, caching the
//  JassArray it lazily creates at +0x20.  Real class name not recoverable
//  from this closure (no vtable of its own, and the source file tag on the
//  neighbouring string literal is generic ".\\Instance.cpp"); named for
//  what it is rather than guessed as anything from Jass2/Nodes.h.
//----------------------------------------------------------------------------
class JassArrayRefSite
{
public:
    //  0x6F45A3D0 - drop the cached array (if `forceNew` is false, the kind
    //  is array-shaped [+0x18 >= 9] and one is already cached) and/or
    //  lazily create/resize it to `count` elements.  `retn 8`.
    void AcquireArray(unsigned int count, int forceNew);

    //  0x6F4594D0 - `retn 8`.  Stamps m_kind and m_field1C together; every
    //  call site this closure reaches passes them as a pair, which is the
    //  only reason m_field1C is not still "reserved".
    //  jassarrayrefsiteaccessors.cpp.
    void SetKindAndField1C(int kind, int field1C);

    //  0x6F459500 - `retn 4`.  True when this site is array-shaped
    //  (m_kind >= 9) *and* its base element kind (m_kind - 5) equals
    //  `baseKind` - array kinds run 5 past their scalar counterpart in this
    //  closure's own numbering (see AcquireArray's own `>= 9` test), so
    //  `-5` recovers the scalar kind an array-of-that-kind was tagged with.
    //  jassarrayrefsiteaccessors.cpp.
    int IsArrayOfKind(unsigned int baseKind);

    //  0x6F459520 - `retn 0`.  The cached JassArray*, or null if none has
    //  been created yet.  jassarrayrefsiteaccessors.cpp.
    JassArray* GetArray();

    //  0x6F459530 - `retn 4`.  Same value as GetArray(), handed back
    //  through an out-parameter instead of the return register; no evidence
    //  in this closure for why a caller would want it this way round rather
    //  than just calling GetArray().  jassarrayrefsiteaccessors.cpp.
    void GetArrayInto(JassArray** out);

    //  0x6F459540 - `retn 0`.  A second field this closure's own dump never
    //  sees written - only read straight back through this accessor - so
    //  its meaning past "the dword right after m_array" is unrecovered.
    //  jassarrayrefsiteaccessors.cpp.
    int GetField24();

    //  0x6F4596F0 - `retn 0`.  The cached array's own element count
    //  (JassArray::m_elementCount) when this site is array-shaped and an
    //  array actually exists, else 0.  jassarrayrefsiteaccessors.cpp.
    unsigned int GetArrayCount();

    char          m_reserved00[0x18];
    int           m_kind;          // +0x18
    int           m_field1C;       // +0x1C - see SetKindAndField1C above
    JassArray*    m_array;         // +0x20
    int           m_field24;       // +0x24 - see GetField24 above
};

#endif

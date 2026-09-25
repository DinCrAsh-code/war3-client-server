//============================================================================
//  CFogMask's own row-lookup/lazy-row-creation member and two of its
//  siblings in the same shipped module (createunit-H2 follow-up on G4's
//  leftover CFogMask cluster - see fogmasktypes.h and fogmaskptrarray.cpp/
//  fogspanarray.cpp/fognodearray.cpp/nodemapdeparray.cpp for the rest of
//  this class's already-reconstructed TSGrowableArray<T> members).
//
//  0x6F409D00 - `GetOrCreateRow(row, param)`.  `m_rows` (this class's own
//  TSGrowableArray<CFogMask*>, already established by fogmaskptrarray.cpp
//  at +0x04 - "CFogMask's own array of child/sub-mask pointers") is grown,
//  the same rounding-up-to-chunk arithmetic tsarray.inl's own SetCount uses
//  (row+1 as the trigger, row+2 as the actual new count - one row of
//  lookahead past the one being asked for), the freshly-uncovered slots are
//  zeroed with an explicit memset rather than a per-element placement `new`
//  (a raw-pointer element has no constructor for the generic template to
//  call, so the shipped code does not route this growth through
//  TSGrowableArray<CFogMask*>::SetCount - no such instantiation exists in
//  this call tree at all, only ComputeChunk/SetAlloc do), then the row at
//  `row` is returned if already populated, otherwise a fresh 0x40-byte
//  CFogMask is allocated (through Storm, tagged with Storm's own template
//  header/line the way Render/filecachefind.cpp's SMemAlloc call already
//  is - not this class's own file/line) and placement-constructed via
//  0x6F409120.
//
//  0x6F409120 - CFogMask's own constructor (721 instructions: stamps
//  `??_7CFogMask@@6B@`, stores its `row` argument, and initialises every
//  sibling array this class owns, spans included). Left a naked redirect
//  to the real, unhooked body - far beyond this session's budget, and
//  nothing the caller above needs beyond its address and its
//  two-stack-dword ABI (retn 8). No vtable slot of CFogMask's own table is
//  touched here, per this session's scope.
//
//  0x6F408FD0 - a second CFogMask/Span helper G4's own report called "a
//  smaller CFogMask helper (111 instructions)"; reading it shows it is not
//  a leaf at all. It resets/repopulates a TSGrowableArray<Span> (the
//  "aAuspan"-tagged array fogspanarray.cpp already owns) and then calls
//  0x6F408F80, which is not part of CFogMask's own module - it indexes a
//  *global* row-cache table (`dword_6FAB6A58`/`dword_6FAB6A5C`/
//  `dword_6FAB6A60`, tagged `aG_4`) through two more unreconstructed
//  functions (0x6F408800, which itself calls 0x6F34C1E0, and 0x6F408ED0's
//  own sibling chain: sub_6F4072B0/sub_6F4086F0). That is five-plus
//  unresolved callees and three unresolved globals outside this cluster's
//  own scope for what would still only be one helper, so it is left a
//  naked redirect too rather than pulled into this session's budget; a
//  genuine reconstruction of it belongs with whoever eventually takes on
//  that global row-cache table.
//============================================================================
#include "storm.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

class CFogMask;

#include "fogmasktypes.h"

class CFogMask
{
public:
    CFogMask* GetOrCreateRow(int row, int param);

    //  0x6F409120 - CFogMask's own constructor.  A real `__thiscall` member
    //  (not a free function forced through `__fastcall`) because both `row`
    //  and `param` are genuine *stack* arguments (`retn 8`) - a free
    //  function cannot be spelled `__thiscall` in this compiler
    //  (Agent/agentbaseabsnotify.cpp's own note on the same restriction).
    CFogMask* Construct(int row, int param);

    void*                       m_vtable;          // +0x00  ??_7CFogMask@@6B@
    TSGrowableArray<CFogMask*>  m_rows;            // +0x04  child/sub-mask row pointers
    char                        m_reserved14[0x2C]; // +0x14 - the Span/FogNode/
                                                     //  NodeMapDep arrays and other
                                                     //  fields the constructor
                                                     //  (0x6F409120, not
                                                     //  reconstructed this session)
                                                     //  would establish; unread by
                                                     //  this file's own function
};

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig             \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  Out of this session's scope - naked redirects to the real, unhooked
//  shipped bodies. Argument shapes are read off this file's own call site.
ADDR_THUNK(CFogMask* __thiscall CFogMask::Construct(int row, int param),
           0x6F409120)
ADDR_THUNK(void __fastcall ResetFogMaskSpanRow(int count, void* spanArray),
           0x6F408FD0)

//  What the shipped SMemAlloc call site tags itself with - Storm's own
//  template header/line, the same convention Render/filecachefind.cpp's
//  FindOrLoadPrefetchNode already established for an allocation made from
//  inside a Storm template rather than from this file directly.
static const char kStormTemplateHeader[] =
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h";

CFogMask* CFogMask::GetOrCreateRow(int row, int param)
{
    unsigned int threshold = (unsigned int)row + 1;

    if (m_rows.m_count <= threshold)
    {
        unsigned int need = threshold + 1;

        if (need > m_rows.m_alloc)
        {
            unsigned int chunk = m_rows.m_chunk;
            if (chunk == 0)
                chunk = m_rows.ComputeChunk(need);

            unsigned int alloc = need;
            unsigned int over = need % chunk;
            if (over != 0)
                alloc = need + (chunk - over);

            m_rows.SetAlloc(alloc);
        }

        memset(&m_rows.m_data[m_rows.m_count], 0,
               (need - m_rows.m_count) * sizeof(CFogMask*));
        m_rows.m_count = need;
    }

    CFogMask* existing = m_rows.m_data[row];
    if (existing != 0)
        return existing;

    CFogMask* fresh = (CFogMask*)SMemAlloc(sizeof(CFogMask),
                                            kStormTemplateHeader, 98, 0);
    if (fresh != 0)
        fresh = fresh->Construct(row, param);

    m_rows.m_data[row] = fresh;
    return fresh;
}

//============================================================================
//  See observer.h.  0x6F629A70 / 0x6F62A9D0.
//============================================================================
#include "observer.h"
#include "observereventreg.h"
#include "dataallocator.h"

//  unk_6FACE9D8 - see funcmap.py's DATA table; a CDataAllocator whose
//  elements are CObserverEventList-shaped resources (m_resource points into
//  it), distinct from observereventreg.cpp's own two pools (which the
//  *contents* of one such resource are released back to, not the resource
//  header itself).
//
//  **`extern`, never a definition.**  This global is the *game's* pool, at
//  its own address in game.dll's data section, and the only thing that
//  connects this name to that address is build_mix.py's generated
//  gen_data.asm - which binds a DATA-table entry only when the symbol comes
//  back as EXTRN from `build/*.asm`:
//
//      mangled = extern_map.get(cpp)
//      if not mangled:
//          continue        # <- a symbol *defined* here is skipped entirely
//
//  So defining it does not just add a redundant copy, it silently cancels
//  the binding: the .mix gets its own zero-initialised CDataAllocator, and
//  the funcmap.py DATA row for unk_6FACE9D8 stops meaning anything.  Live,
//  CObserver::EnsureResource then hands that empty allocator to the real
//  shipped pool entry point (0x6F629E90) as `this`, which walks its
//  never-initialised chunk pointers - and the game crashed on the first
//  dispatch through a hooked observer.
//
//  It was a definition once, to get past a link error ("a bare `extern` no
//  longer links once CAgent has a real constructor").  That traded a link
//  error for a live crash, and it was not even needed: gen_data.obj is what
//  resolves this symbol in the real build.  tools/link_check.py deliberately
//  links *without* gen_data.obj, so seeing this name unresolved there is the
//  expected result and is checked against the DATA table by name - not a
//  reason to define it.
extern CDataAllocator g_observerResourcePool;   // unk_6FACE9D8

//----------------------------------------------------------------------------
//  0x6F629A70.  No owned resource yet.
//----------------------------------------------------------------------------
CObserver::CObserver()
{
    m_refcount = 0;
    m_resource = 0;
}

//----------------------------------------------------------------------------
//  0x6F62A9D0.  If a resource is owned, release its event list and free
//  the resource itself back to its pool.
//
//  Both this function and CAgent::CAgent() (agent_ctor.cpp) and
//  CWidget__Constructor (widget.cpp) show the same SEH-frame mismatch
//  documented in docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped
//  frame this toolchain cannot reproduce": a destructor whose base class
//  (TRefCnt) has its own destructor to run, wrapped around a call the
//  compiler cannot prove is `noexcept`, is enough on its own for MSVC to
//  emit this frame - no explicit `__try`/`__finally` needed in the source,
//  which is why none appears above despite the dump showing the same
//  cookie-XORed, frame-pointer-omitted shape as the two functions that
//  case study did write an explicit `__try`/`__finally` for. Under this
//  build's fixed /GS- /EHs-c- the bundled compiler still only reaches the
//  older, EBP-based `__except_handler3` frame - not reproducible, per the
//  same doc section - so the mismatch here is the frame furniture only;
//  the body (the null test, the two calls, the vtable resets at entry and
//  exit) matches instruction for instruction.
//----------------------------------------------------------------------------
CObserver::~CObserver()
{
    void* resource = m_resource;
    if (resource)
    {
        ((CObserverEventList*)resource)->ReleaseThunk();
        g_observerResourcePool.Free(resource, 0, 0);
    }
}

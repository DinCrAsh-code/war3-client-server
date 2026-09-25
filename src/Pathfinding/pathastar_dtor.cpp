//============================================================================
//  0x6F488BE0 - NIpse::CLrPathingAcc::~CLrPathingAcc() (CPathAStar), reached
//  through slot 2's scalar deleting destructor (pathastar_scalardtor.cpp).
//
//  Tears down three CMemBlock-derived member sub-objects in address order -
//  +0x70 (m_heap, a polymorphic NTempest::CPriorityQ<CLrOpenNode,CLrOpenNode>
//  instance - the dump stamps that class's own vtable into +0x70 right
//  before releasing it, the standard defensive most-derived-class restamp a
//  trivial-but-for-its-base-teardown destructor gets), then +0x50 (m_nodes)
//  and +0x30 (the SAStarLinkArray this batch's own Apply/CancelAndUnregister/
//  Save/Load already reach - see pathastar.h's own header comment on that
//  class for why +0x30 is `m_links`'s real storage) - both of the latter two
//  reached with no extra stamp of their own, i.e. they are themselves plain,
//  no-further-derived NTempest::CMemBlock instances. All three go through
//  the same real, unhooked, already-in-this-tree
//  ReleaseCMemBlockBase/sub_6F4B3770 (Pathfinding/cellbuffer.cpp) every
//  other polymorphic-CMemBlock teardown in this family uses (CRlAgent::
//  ~CRlAgent, crlagent_dtor.cpp). Finishes with the standard base-class
//  defensive restamp to `??_7CPresence@NTempest@@6B@` before returning -
//  CPresenceTagged's own trivial destructor folded into this same function
//  with no stamp of its own, the same "a ctor and a dtor for the same chain
//  are not obliged to inline the same way" shape crlagent_dtor.cpp's own
//  header already documents for the sibling class.
//
//  THE SEH FRAME ITSELF IS NOT REPRODUCIBLE WITH THIS TOOLCHAIN'S FIXED
//  /GS- /EHs-c-.  The dump's own prologue/epilogue (push -1 / push offset
//  SEH_6F488BE0 / mov eax,fs:0 / push eax / ... / a GS cookie XORed straight
//  into ESP with no EBP frame at all / a scope-table trampoline appended far
//  away in the image, ending in a call to SecurityCheckCookie then a tail
//  jump into __CxxFrameHandler3) is exactly the "unified" VC8 EH4 frame
//  shape docs/msvc-vc8-idioms.md's own "An `__except_handler4`-shaped frame
//  this toolchain cannot reproduce" section already establishes: the
//  bundled `cl.exe` in `vs8/`, at every `/GS`x`/EHsc` combination this repo
//  can select, only ever reaches the OLDER, EBP-framed, cookie-less
//  `__except_handler3` shape for a real `__try`/`__finally`, never this
//  one - confirmed there against two other, unrelated functions in this same
//  binary carrying the identical frame idiom (`sub_6F42CB50`/`sub_6F02F780`,
//  reconstructed from real `__try`/`__finally` C++ that matches the dump
//  past the frame furniture and is filed `DIFFERS` for exactly this reason).
//  The body below is written the same way - real `__try`/`__finally` around
//  the three real teardown calls, matching the dump's own instruction order
//  and operands past the prologue/epilogue - and is expected to land in the
//  same low-score-but-verified-correct-body bucket for the identical,
//  already-diagnosed reason, not a new, guessed-at one.
//============================================================================
#include "pathastar.h"
#include "cellbuffer.h"

//  See crlagent_dtor.cpp's own StampVtable for why this goes through a
//  volatile store rather than a plain literal assignment.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

CPathAStar::~CPathAStar()
{
    __try
    {
        //  ??_7?$CPriorityQ@VCLrOpenNode@NIpse@@V12@@NTempest@@6B@ - m_heap's
        //  own most-derived vtable, defensive-restamped before the base
        //  (NTempest::CMemBlock) part is torn down.
        StampVtable((char*)this + 0x70, (void*)0x6F9525BC);
        ReleaseCMemBlockBase((char*)this + 0x70);

        ReleaseCMemBlockBase((char*)this + 0x50);

        ReleaseCMemBlockBase((char*)this + 0x30);
    }
    __finally
    {
    }

    //  ??_7CPresence@NTempest@@6B@ - defensive base restamp on the way out.
    StampVtable(this, (void*)0x6F951CA0);
}

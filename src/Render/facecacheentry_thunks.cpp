//============================================================================
//  citem-batch-J's own not-reconstructed callees in the FACEDATA/
//  FACECACHEENTRY closure (Render/facedata.h) - redirects into the real,
//  unhooked shipped bodies at their real addresses.  Same shape and same
//  rules as Misc/unreconstructed_thunks.cpp: NOT reconstructions, and the
//  declared argument bytes have to agree with each one's real `retn <n>`
//  (tools/thunk_abi_audit.py).
//
//  Each entry below says in one line why it stayed a redirect: every one of
//  them is either the giant `__CxxFrameHandler3`/security-cookie EH frame
//  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" already gives up on (0x6F7C5090,
//  0x6F7C4FC0 - FACECACHEENTRY's own dtor/ctor, both well past this
//  session's budget at 240 and 87 instructions apiece with an
//  `eh vector constructor/destructor iterator` pair inside), a deep
//  floating-point font-metrics or hash-table-lookup routine reaching six or
//  more callees outside this batch's own dump (0x6F7C55C0, 0x6F7BCFD0,
//  0x6F7C9210, 0x6F7C8430, 0x6F7C4950), or a three-argument list-splice
//  member this session did not fully crack (0x6F7B9990).
//============================================================================
#include "facedata.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F7C5090 - `retn 0`, thiscall.  FACECACHEENTRY::~FACECACHEENTRY: 240
//  instructions under the SEH/cookie frame, unlinking the entry's three
//  bucket tables (Render/facechartable.cpp and siblings), a fourth
//  TSExplicitList<T> at +8 (TSExplicitList<T>::UnlinkAll at 0x6F7BAE30,
//  reconstructed - Render/faceunlinklist.cpp), a TSLink<T> at +0x14, an
//  8-element `eh vector destructor iterator` array at +0x1C8, and finally
//  its own membership link at +0x00.
ADDR_THUNK(FACECACHEENTRY::~FACECACHEENTRY(), 0x6F7C5090)

//  0x6F7C4FC0 - `retn 0`, thiscall.  FACECACHEENTRY::FACECACHEENTRY: the
//  mirror construct, 87 instructions ending in an
//  `eh vector constructor iterator` over the same 8-element array with a
//  rollback destructor, and a derived-class vtable restamp
//  (`??_7?$TAllocatorHashTable@UKERNNODE@@VKERNINGHASHKEY@@$0IA@@@6B@`)
//  over the KERNNODE table at +0x6C right after constructing it as a plain
//  KERNNODE_TABLE.
ADDR_THUNK(FACECACHEENTRY::FACECACHEENTRY(), 0x6F7C4FC0)

//  0x6F7C4950 - `retn 0`, thiscall.  The rest of FACECACHEENTRY's teardown
//  past its own m_glyphCache release (Render/facecacheentry.cpp's own
//  ReleaseGlyphCache tail-calls this): an 8-iteration per-slot loop at
//  +0x1D0 releasing a further ref-counted block and clearing three more
//  fields, then both CHARCODEDESC_TABLE::Clear/GLYPHBITMAP_TABLE::Clear and
//  a per-bucket walk over the third table's `TAllocatorHashTable` vftable,
//  ending in KERNNODE_TABLE::Clear itself.
ADDR_THUNK(void FACECACHEENTRY::ReleaseDetail(), 0x6F7C4950)

//  0x6F7C55C0 - `retn 0Ch`, thiscall + three stack args (a name string, an
//  int, and a float point size).  Font-size-in-pixels computation: builds a
//  cache-key path through Storm_578, clamps a scaled size through
//  sub_6F7B8B70 (reconstructed, Render/facefloathelpers.cpp) twice, and
//  ends by looking the resulting size up (sub_6F7C9210) and evicting the
//  old cached size (sub_6F7BCFD0) on a change - both callees left thunked
//  below for the same reason.
ADDR_THUNK(void FACECACHEENTRY::SetSize(const char*, int, float), 0x6F7C55C0)

//  0x6F7BCFD0 - `retn 0`, thiscall.  Recomputes the entry's own line-height/
//  ascent/descent metrics (offsets +0xA4/+0xA8/+0x1B0/+0x1B4/+0x1C4) off
//  whatever sub_6F7C7E60's own lookup (already reconstructed,
//  FieldHolder_6F7C7E60::Get) currently returns, through the same
//  sub_6F7B8B70/sub_6F7B8B10 clamp/rounding pair SetSize above uses and a
//  call into sub_6F7CBBF0 this batch's dump does not reach.
ADDR_THUNK(void FACECACHEENTRY::RecomputeMetrics(), 0x6F7BCFD0)

//  0x6F7C9210 - `retn 0`, __fastcall(ecx,edx).  The face-cache lookup-or-
//  create FACEDATA itself (Render/facecacheentry.cpp) comes from: hashes
//  the (name, size) pair (sub_6F7C8430, thunked below), and on a miss
//  allocates a fresh FACEDATA (sub_6F7C85F0, reconstructed) under the
//  "HFACE" tag and files it into the table via sub_6F7C91B0/sub_6F4C40C0/
//  sub_6F4C40E0.
ADDR_THUNK(void* __fastcall LookupOrCreateFace(const char* name, void* table),
           0x6F7C9210)

//  0x6F7C8430 - `retn 4`, thiscall + one stack arg (the requested size).
//  The (name,size) hash bucket walk LookupOrCreateFace above and
//  RecomputeMetrics's own caller both probe first, over the global face
//  table `unk_6FAE71F4` this batch's dump never resolves a type for.
ADDR_THUNK(void* SFaceTableProbeThunk::Probe(int), 0x6F7C8430)

//  0x6F7B9990 - `retn 0Ch`, thiscall + three stack args.  A three-argument
//  TSExplicitList-shaped splice (default-to-self-terminator on both the
//  source and destination link when the respective pointer argument is
//  null, then either a single-node relink or a multi-node walk depending
//  on the count argument) that does not match any of storm.h's own
//  `__forceinline` Link*() members closely enough to be confident of the
//  source shape in the time this session had; left a redirect rather than
//  guessed at.
ADDR_THUNK(void SListSpliceThunk::Splice(void*, void*, unsigned int),
           0x6F7B9990)

//  0x6F604FC0 - `retn 4`, thiscall + one stack arg.  Shares CLayoutFrame's
//  own +0x54 field (Frame/frameremovepointsrelativeto.cpp's own
//  RemovePointsRelativeTo reads the same field at the same offset) but
//  calls into 0x6F62B910 - itself an event-registry/TLS-slot routine this
//  session did not finish tracing, right below - against a sub-object at
//  +0x44 whose type is not established, so left a redirect rather than
//  guessed at.
ADDR_THUNK(void SFrameEventBindingThunk::SetBinding(void*), 0x6F604FC0)

//  0x6F62B910 - `retn 0`, thiscall.  A TLS-slot fetch (sub_6F4C34D0) feeding
//  a CEventRegistry::AcquireBucket call (sub_6F62AF10, already reconstructed
//  - Frame/eventregistry.cpp) against the module-global registry
//  `stru_6FACEA68`, then on a hit a further sub_6F631350/sub_6F6D8950
//  (ConditionVariable::SignalOne) pair this batch's dump does not resolve
//  a receiver type for.
ADDR_THUNK(void SFrameEventBindingThunk::Unregister(), 0x6F62B910)

#undef ADDR_THUNK

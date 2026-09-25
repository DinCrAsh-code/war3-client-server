//============================================================================
//  CDataAllocator: a fixed-block pool allocator.  Its own default
//  allocation tag is its own source file's name (".\\CDataAllocator.cpp",
//  baked into sub_6F4C1AB0's body as a fallback string constant) - the
//  strongest evidence in this call tree for the class's real name.
//
//  Layout, established by Alloc()/Free() (0x6F4C1AB0/0x6F4C1B50) - the only
//  two methods this call tree reaches:
//    +0x00  m_elementSize   - bytes per element, read but never written here
//    +0x04  m_growCount     - elements to grow by when the free list is
//                             empty; also selects the tag/line defaulting
//                             (see Alloc())
//    +0x08  m_liveCount     - bumped by Alloc(), dropped by Free()
//    +0x0C  m_blockListHead - the head of a singly-linked list of raw
//                             SMemAlloc'd blocks, threaded through each
//                             block's own first dword (freed as a whole,
//                             never read back by this call tree)
//    +0x10  m_freeListHead  - the head of the free-element list, threaded
//                             through each free element's own first dword
//
//  Nothing at or past whatever comes after +0x10 is touched here, so the
//  class ends there; the real allocator almost certainly has more fields
//  (an element destructor pointer, most likely) that this call tree never
//  reads.
//============================================================================
#ifndef DATAALLOCATOR_H
#define DATAALLOCATOR_H

//  widget.h/jassfunc.h/task.h/taskparam.h/taskrotate.h define the real
//  classes; forward-declared here only so
//  ConstructCWidget/ConstructCJassFunc/ConstructCTask/ConstructCTaskParam/
//  ConstructCTaskRotate below can name their own return types.
class CWidget;
class CJassFunc;
class CTask;
class CTaskParam;
class CTaskRotate;
class CTaskPoint;
class CTaskPoint2;
class CTaskPointAndTarget;
class CTaskTarget;
class CTaskTarget2;
class CTaskAction;
class CTaskOrderParam;
class CTaskGhost;

class CDataAllocator
{
public:
    //  0x6F4C1AB0.  Pop the free list, growing it by m_growCount freshly
    //  SMemAlloc'd elements first if it is empty; optionally zero the
    //  popped element before handing it back.  `tag`/`line` are Storm's
    //  allocation-site strings and are only used the one time this call
    //  actually has to grow the pool with a chunk size other than the
    //  class's own default (m_growCount != 1) - see the .cpp for the exact
    //  defaulting shipped code does.
    void* Alloc(int zeroMemory, const char* tag, int line);

    //  0x6F4C1B50.  Push `ptr` back onto the free list and drop the
    //  live-element count.  The two extra stack arguments the shipped
    //  callers push and this function never reads (see observer.cpp/
    //  observereventreg.cpp) stay unread, matching the shipped body.
    void Free(void* ptr, const char* tag, int line);

    //  0x6F2ACAE0 (CWidget__Constructor's own root address, in the dump's
    //  naming) - allocate a CWidget-sized block from *this* and construct
    //  a CWidget into it; see widget.cpp for the body and for why this is
    //  a method of the generic allocator rather than a CWidget-side
    //  factory.  Declared as a member here purely for the calling
    //  convention: the dump's own `ecx`/one-stack-arg/`retn 4` shape is
    //  thiscall's, with the allocator itself as `this`, which this
    //  compiler will not let a free function spell as an explicit
    //  `__thiscall`.  Returns null if Alloc() itself returns null (never
    //  observed in this call tree, but the shipped code checks it).
    CWidget* ConstructCWidget(int zeroMemory);

    //  0x6F4C1A60 (gamemain-batch-3).  Walk m_blockListHead, SMemFree()ing
    //  every raw block, then reset the pool to empty (m_freeListHead=0,
    //  m_liveCount=0).  `tag`/`line` are re-tested every loop iteration
    //  (the shipped body re-checks `tag != 0` on each pass, not just
    //  once) and fall back to this class's own ".\\CDataAllocator.cpp"
    //  line 0x7F when `tag` is null - the same defaulting Alloc() does
    //  with its own line 0x98.  The third stack argument every call site
    //  pushes is never read here, the same unused-tail-argument shape
    //  Free() already documents.
    void ReleaseAllBlocks(const char* tag, int line, int /*unused*/);

    //  0x6F3B9CE0 (src/Jass/jassfunc.cpp) - the JASS VM's own
    //  InstanceGenerator<CJassFunc> factory slot 0 (0x6F3BCFF0) calls
    //  straight into this, `this` being TInstanceAllocator<CJassFunc>'s own
    //  embedded, layout-compatible CDataAllocator sub-object.  Same
    //  Alloc()-then-placement-construct shape as ConstructCWidget above,
    //  but kept a `THUNK` rather than a hand-written body: the shipped
    //  function opens and closes a real `__except_handler4`-shaped SEH
    //  frame (`push -1 / push offset SEH_6F3B9CE0 / mov eax, fs:0 / ...`)
    //  that this repo's fixed `/GS- /EHs-c-` toolchain cannot emit for any
    //  function, so a from-scratch reconstruction could only ever diverge
    //  on the frame itself - the same reason
    //  JassInstance::Construct/Run and their siblings
    //  (docs/targets/JassVM.md) are kept naked redirects with real,
    //  evidence-backed names instead of hand-written bodies.  See
    //  jassfunc.h for what the real body does (construct a CJassFunc via
    //  CAgent::CAgent() + CAgentWar3's inlined field-init + the real
    //  ??_7CJassFunc@@6B@ stamp + one more zeroed field) and jassfunc.cpp
    //  for the thunk declaration.
    CJassFunc* ConstructCJassFunc(int zeroMemory);

    //  0x6F28EC40 (src/Task) - InstanceGenerator<CTask>'s own factory slot
    //  0 (0x6F291B80) calls straight into this, `this` being
    //  TInstanceAllocator<CTask>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCJassFunc above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28EC40 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See task.h for what the real
    //  body does (CAgent::CAgent() + CAgentWar3's inlined m_flags=0 +
    //  CUnitRefList's inlined field24/field28=-1 + the real
    //  ??_7CTask@@6B@ stamp + two more CTask-owned fields zeroed) and
    //  taskinstancegenerator.cpp for the thunk declaration.
    CTask* ConstructCTask(int zeroMemory);

    //  0x6F28ECC0 (src/Task) - InstanceGenerator<CTaskParam>'s own factory
    //  slot 0 (0x6F291BA0) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskParam>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCTask above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28ECC0 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See taskparam.h for what the
    //  real body does (CTask's own inlined field-init, one field further
    //  than CTask's own construction path + the real
    //  ??_7CTaskParam@@6B@ stamp) and taskparaminstancegenerator.cpp for
    //  the thunk declaration.
    CTaskParam* ConstructCTaskParam(int zeroMemory);

    //  0x6F28ED40 (src/Task) - InstanceGenerator<CTaskRotate>'s own factory
    //  slot 0 (0x6F291BC0) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskRotate>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCTask above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28ED40 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See taskrotate.h for what the
    //  real body does (CTask's own inlined field-init + the real
    //  ??_7CTaskRotate@@6B@ stamp + a FloatMini sub-object stamped and
    //  zeroed at +0x34) and taskrotateinstancegenerator.cpp for the thunk
    //  declaration.
    CTaskRotate* ConstructCTaskRotate(int zeroMemory);

    //  0x6F28EED0 (src/Task) - InstanceGenerator<CTaskPoint>'s own factory
    //  slot 0 (0x6F291C20) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskPoint>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCTask above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28EED0 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See taskpoint.h for what the
    //  real body does (CTask's own inlined field-init + the real
    //  ??_7CTaskPoint@@6B@ stamp + three FloatMini sub-objects and the
    //  m_moveRequest pointer zeroed) and taskpointinstancegenerator.cpp for
    //  the thunk declaration.
    CTaskPoint* ConstructCTaskPoint(int zeroMemory);

    //  0x6F28EF40 (src/Task) - InstanceGenerator<CTaskPoint2>'s own factory
    //  slot 0 (0x6F291C40) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskPoint2>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCTask above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28EF40 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See taskpoint2.h for what the
    //  real body does (CTaskPoint's own inlined field-init + the real
    //  ??_7CTaskPoint2@@6B@ stamp + two more FloatMini sub-objects
    //  stamped/zeroed at +0x50/+0x58) and taskpoint2instancegenerator.cpp
    //  for the thunk declaration.
    CTaskPoint2* ConstructCTaskPoint2(int zeroMemory);

    //  0x6F28EFD0 (src/Task) - InstanceGenerator<CTaskPointAndTarget>'s own
    //  factory slot 0 (0x6F291C60) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskPointAndTarget>'s own embedded,
    //  layout-compatible CDataAllocator sub-object.  Same
    //  Alloc()-then-placement-construct shape as ConstructCTask above, and
    //  kept a `THUNK` for the same reason: the shipped function opens and
    //  closes a real `__except_handler4`-shaped SEH frame (`push -1 / push
    //  offset SEH_6F28EFD0 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See ctaskpointandtarget.h for
    //  what the real body does (CTaskPoint's own inlined field-init + the
    //  real ??_7CTaskPointAndTarget@@6B@ stamp + a second SHandleTriple
    //  zeroed/-1'd and the m_target pointer zeroed) and
    //  taskpointandtargetinstancegenerator.cpp for the thunk declaration.
    CTaskPointAndTarget* ConstructCTaskPointAndTarget(int zeroMemory);

    //  0x6F28F050 (src/Task) - InstanceGenerator<CTaskTarget>'s own factory
    //  slot 0 (0x6F291C80) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskTarget>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCTask above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28F050 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See tasktarget.h for what the
    //  real body does (CTask's own inlined field-init + the real
    //  ??_7CTaskTarget@@6B@ stamp + a second SHandleTriple zeroed/-1'd, a
    //  FloatMini sub-object stamped with g_CFloatZero, and the m_target
    //  pointer zeroed) and tasktargetinstancegenerator.cpp for the thunk
    //  declaration.
    CTaskTarget* ConstructCTaskTarget(int zeroMemory);

    //  0x6F28F0F0 (src/Task) - InstanceGenerator<CTaskTarget2>'s own factory
    //  slot 0 (0x6F291CA0) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskTarget2>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCTaskTarget above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28F0F0 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See tasktarget2.h for what the
    //  real body does (CTaskTarget's own inlined field-init + the real
    //  ??_7CTaskTarget2@@6B@ stamp + a third SHandleTriple zeroed/-1'd) and
    //  tasktarget2instancegenerator.cpp for the thunk declaration.
    CTaskTarget2* ConstructCTaskTarget2(int zeroMemory);

    //  0x6F28EDD0 (src/Task) - InstanceGenerator<CTaskAction>'s own factory
    //  slot 0 (0x6F291BE0) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskAction>'s own embedded, layout-compatible
    //  CDataAllocator sub-object.  Same Alloc()-then-placement-construct
    //  shape as ConstructCTask above, and kept a `THUNK` for the same
    //  reason: the shipped function opens and closes a real
    //  `__except_handler4`-shaped SEH frame (`push -1 / push offset
    //  SEH_6F28EDD0 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See taskaction.h for what the
    //  real body does (CTaskParam's own inlined field-init, unchanged, +
    //  the real ??_7CTaskAction@@6B@ stamp instead of CTaskParam's own) and
    //  taskactioninstancegenerator.cpp for the thunk declaration.
    CTaskAction* ConstructCTaskAction(int zeroMemory);

    //  0x6F28EE50 (src/Task) - InstanceGenerator<CTaskOrderParam>'s own
    //  factory slot 0 (0x6F291C00) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskOrderParam>'s own embedded,
    //  layout-compatible CDataAllocator sub-object.  Same
    //  Alloc()-then-placement-construct shape as ConstructCTask above, and
    //  kept a `THUNK` for the same reason: the shipped function opens and
    //  closes a real `__except_handler4`-shaped SEH frame (`push -1 /
    //  push offset SEH_6F28EE50 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See taskorderparam.h for
    //  what the real body does (CTaskParam's own inlined field-init,
    //  unchanged, + the real ??_7CTaskOrderParam@@6B@ stamp instead of
    //  CTaskParam's own) and taskorderparaminstancegenerator.cpp for the
    //  thunk declaration.
    CTaskOrderParam* ConstructCTaskOrderParam(int zeroMemory);

    //  0x6F28F160 (src/Task) - InstanceGenerator<CTaskGhost>'s own
    //  factory slot 0 (0x6F291CC0) calls straight into this, `this` being
    //  TInstanceAllocator<CTaskGhost>'s own embedded,
    //  layout-compatible CDataAllocator sub-object.  Same
    //  Alloc()-then-placement-construct shape as ConstructCTask above, and
    //  kept a `THUNK` for the same reason: the shipped function opens and
    //  closes a real `__except_handler4`-shaped SEH frame (`push -1 /
    //  push offset SEH_6F28F160 / mov eax, fs:0 / ...`) this repo's fixed
    //  `/GS- /EHs-c-` toolchain cannot emit.  See taskghost.h for what the
    //  real body does (CTaskParam's own inlined field-init, unchanged, +
    //  this class's own six new fields + the real ??_7CTaskGhost@@6B@
    //  stamp instead of CTaskParam's own) and taskghostinstancegenerator.cpp
    //  for the thunk declaration.
    CTaskGhost* ConstructCTaskGhost(int zeroMemory);

    unsigned int    m_elementSize;    // +0x00
    unsigned int    m_growCount;      // +0x04
    unsigned int    m_liveCount;      // +0x08
    void*           m_blockListHead;  // +0x0C
    void*           m_freeListHead;   // +0x10
};

#endif

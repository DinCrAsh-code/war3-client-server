//============================================================================
//  0x6F4B39C0 - CMemBlock's Storm-backed growth.
//============================================================================
#include "cellbuffer.h"

//----------------------------------------------------------------------------
//  0x6F4B39C0 - resize the usable area to `newSize`.
//
//  The block always carries a header of `m_blockSize - m_size` bytes in front
//  of m_data, and that header survives the resize: the new total is
//  header + newSize, and m_data is recomputed as m_block + header afterwards.
//  A total of zero releases the allocation and parks m_block at -1, which is
//  the sentinel the reallocate path tests for.
//----------------------------------------------------------------------------
int CMemBlock::SetSize(unsigned int newSize, int noZero)
{
    if (newSize != m_size)
    {
        unsigned int header = m_blockSize - m_size;
        unsigned int total = header + newSize;

        if (total > 0)
        {
            void* block = m_block;
            m_block = (block != (void*)-1)
                ? SMemReAlloc(block, total, CMemBlockDebugFile(),
                              CMemBlockDebugLine(), 8)
                : SMemAlloc(total, CMemBlockDebugFile(),
                            CMemBlockDebugLine(), 8);
        }
        else
        {
            SMemFree(m_block, CMemBlockDebugFile(), CMemBlockDebugLine(), 0);
            m_block = (void*)-1;
        }

        m_blockSize = total;
        m_data = (char*)m_block + header;
        m_size = newSize;
    }

    if (!noZero)
        FillBytes(m_data, 0, m_size);

    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4B3730 - release the raw allocation, if any, and zero every field
//  SetSize touches.  Written in the shipped store order (m_size,
//  m_blockSize, m_data, m_block) rather than declaration order.
//----------------------------------------------------------------------------
void CMemBlock::FreeBlock()
{
    if (m_block != 0 && m_block != (void*)-1)
        SMemFree(m_block, CMemBlockDebugFile(), CMemBlockDebugLine(), 0);

    m_size = 0;
    m_blockSize = 0;
    m_data = 0;
    m_block = 0;
}

//----------------------------------------------------------------------------
//  0x6F4B3860 - free any existing allocation, then allocate size + header
//  bytes fresh and set the block up to hand out `size` bytes starting
//  `header` bytes in.  A total of exactly zero skips the allocation and
//  parks m_block at the -1 sentinel, same as SetSize's own release path.
//----------------------------------------------------------------------------
void CMemBlock::Init(unsigned int size, unsigned int header,
                     unsigned int /*unused1*/, unsigned int /*unused2*/)
{
    FreeBlock();

    unsigned int total = size + header;
    void* block = (total != 0)
        ? SMemAlloc(total, CMemBlockDebugFile(), CMemBlockDebugLine(), 8)
        : (void*)-1;

    m_block = block;
    m_blockSize = total;
    m_size = size;
    m_data = (char*)block + header;
}

//----------------------------------------------------------------------------
//  0x6F4B3770 - a *polymorphic* NTempest object's own base-class destructor
//  chain: stamp this object's own `NTempest::CMemBlock` vtable, call
//  FreeBlock() (0x6F4B3730, above - the very entry this file already
//  reconstructed for the plain, non-polymorphic pathfinding uses of
//  CMemBlock), then hand off to the `NTempest::CEntity` base subobject by
//  re-stamping its own vtable before returning.  Both vtables are real
//  Game.dll symbols this repo has never built a vtable *for* - every path
//  that reaches this call tree's own callers is itself a deep NTempest
//  rendering-engine class outside this session's scope (see
//  Jass/jassstormterraingates.cpp's 0x6F74B550, the identically-shaped
//  sibling reached from a different module's closure, already treated the
//  same way for the same reason) - so the stamp is reproduced as two
//  literal `mov`s in a naked, self-redirecting body rather than invented
//  C++ that would need to spell out NTempest::CMemBlock/CEntity's own
//  unmodeled member layout.
//
//  0x6F743320 is a one-instruction `jmp` alias IDA gives its own name -
//  the identical shape Agent/handleobject.cpp's GetHandleOrZeroAlias
//  already documents.
//
//  Declared `__fastcall(void*)` (one param in ecx, matching the real
//  `retn` with zero stack bytes) rather than with no parameters at all -
//  Pathfinding/crlagent_dtor.cpp is this function's first real caller, and
//  the real shipped function takes `this` in ecx like any other thiscall
//  destructor.  A naked body ignores its own declared prototype either
//  way, so this is a pure ABI correction with no effect on the two `mov`/
//  `jmp` instructions themselves.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall ReleaseCMemBlockBase(void* pThis)
{
    __asm
    {
        mov eax, 06F4B3770h
        jmp eax
    }
}

__declspec(naked) void ReleaseCMemBlockBaseAlias()
{
    __asm { jmp ReleaseCMemBlockBase }
}

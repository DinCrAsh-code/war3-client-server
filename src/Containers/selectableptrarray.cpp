//============================================================================
//  TSGrowableArray<SCheckedSelectableSlot> - CAgentPtr<CSelectable>, tagged
//  ".?AV?$CAgentPtr@VCSelectable@@@@" (aAvCagentptrVcs).  Its SetCount
//  (0x6F37CBD0) is already reconstructed - Containers/selectableagentptrcount
//  .cpp - and used to declare ComputeChunk/SetAlloc against a redirect
//  (Containers/agentptrarray_thunks.cpp); both are real now.
//
//  ComputeChunk (0x6F378530) needs nothing element-specific - a plain
//  explicit instantiation of the generic body, the same as
//  Containers/unitagentptrarray.cpp's own.
//
//  SetAlloc (0x6F3796A0) is written out for the reason that file gives:
//  declaring a copy constructor on the element collides with the converting
//  one's own mangled name (SCheckedSelectableSlot::SCheckedSelectableSlot,
//  0x6F2C9670).  Its Release() is spelled `m_refcount += -1` / a separate
//  `== 0` test (agenttypedslots.h), so the move loop's addref matches that
//  shape and is written `+= 1` rather than `++`.
//
//  ~TSGrowableArray (0x6F37B520) walks every live element the same way
//  SetAlloc's shrink path does and frees the block.  New() (0x6F37CB20) is
//  in its own translation unit (selectableptrarray_new.cpp): it converting-
//  constructs the new slot through SCheckedSelectableSlot's own constructor,
//  which opens a __try/__finally frame of its own and is why that one body
//  carries an SEH prologue this array's other members do not.
//============================================================================
#include "agenttypedslots.h"
#include "tsarray.inl"

template unsigned int TSGrowableArray<SCheckedSelectableSlot>::ComputeChunk(unsigned int);

template <>
void TSGrowableArray<SCheckedSelectableSlot>::SetAlloc(unsigned int alloc)
{
    SCheckedSelectableSlot* olddata = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
            olddata[i].Release();
    }

    m_alloc = alloc;
    m_data = (SCheckedSelectableSlot*)SMemReAlloc(olddata,
        alloc * sizeof(SCheckedSelectableSlot), s_typeName, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (SCheckedSelectableSlot*)SMemAlloc(
            alloc * sizeof(SCheckedSelectableSlot), s_typeName, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
            {
                SCheckedSelectableSlot* slot = &m_data[i];
                if (slot != 0)
                {
                    CAgent* held = olddata[i].m_value;
                    slot->m_value = held;
                    if (held)
                        held->m_refcount += 1;
                }

                olddata[i].Release();
            }
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

template <>
TSGrowableArray<SCheckedSelectableSlot>::~TSGrowableArray()
{
    for (unsigned int i = 0; i < m_count; i++)
    {
        SCheckedSelectableSlot* element = &m_data[i];
        element->Release();
    }
    if (m_data)
        SMemFree(m_data, s_typeName, -2, 0);
}

//  0x6F37BF60 - a second, byte-identical instantiation of the destructor
//  above the linker folded to a plain jmp here rather than a second copy -
//  the same pointer-template folding docs/msvc-vc8-idioms.md documents,
//  generalised past raw pointers to any T whose Release() has this exact
//  shape (four-byte value, `+= -1` / separate `== 0` test, devirtualised
//  slot-0 call): the body never touches T beyond that, so two distinct
//  instantiations can and do compile identically.  Its own callers
//  (CSelectionManager::CSelectionManager, CWorldFrameWar3::CWorldFrameWar3
//  and two more, per `xrefs --to`) are all destructor/cleanup call sites
//  reached from other TUs' own EH unwind paths, not from this batch.
__declspec(naked) void SecondSelectableSlotArrayDtorAlias()
{
    __asm
    {
        mov     eax, 06F37B520h
        jmp     eax
    }
}

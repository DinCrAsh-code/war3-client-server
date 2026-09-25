//============================================================================
//  0x6F2B6FF0 - CItem's vtable slot 98 (+0x188), where CSelectable answers
//  a flat 0 (0x6F266CD0, selectable_vtable.cpp).
//
//  "Am I the thing the local player currently has selected": walk the world
//  object's own local-player index to a player record, ask that record for
//  its selected agent, take a counted reference to the answer, compare it
//  with `this`, and drop the reference again.
//
//  The reference really is counted - the shipped tail is the same
//  decrement-and-release-if-last three instructions CAgentPtr::Release
//  (agent.h) has - which is what makes the local a smart pointer rather
//  than a bare copy.  Its constructor is at its own address (0x6F038890),
//  a different one from CAgentPtr's own (0x6F02F780, agent.cpp), so this is
//  a second smart-pointer instantiation and not the same class.
//
//  Own translation unit: three real calls plus the constructor, and the
//  release at the end is SCheckedItemSlot's own destructor inlined -
//  itemtypedassign.h already documents that every caller in this codebase
//  has it inlined rather than calling it.
//============================================================================
#include "item.h"
#include "itemtypedassign.h"   // SCheckedItemSlot - 0x6F038890 already lives here
#include "playercolor.h"       // g_unk6FAB65F4 - the world-object singleton

//  0x6F3A1650 - already reconstructed as IndexedArrayHolder_6F3A1650::At
//  (misc_field_getters.cpp): the world object's own 4-byte array at +0x58,
//  indexed here by the local-player index it keeps at +0x28.  Declared here
//  rather than re-thunked - a second declaration at the same address is
//  exactly the two-symbols-for-one-function defect
//  docs/notes/verifier-gate-link-failures.md warns about, and it silently
//  cost that function its own EXACT score the first time this file was
//  written.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  What the array element points at - only the one field this function
//  reaches through.
struct SPlayerRecord
{
    char  m_reserved00[0x34];
    void* m_selection;    // +0x34
};

//  0x6F41FA10 - the agent that record currently has selected.  A thiscall
//  member of whatever +0x34 names, with no arguments.  Reconstructed in its
//  own translation unit (selectionholder.cpp) - declared, not defined, here
//  so this call site reaches it with a real `call` instead of inlining the
//  field read.
struct SSelectionRecord
{
    CAgent* GetSelectedAgent();
};

int CItem::Method_0x188()
{
    SPlayerRecord* player = (SPlayerRecord*)
        ((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(
            *(const unsigned short*)((const char*)g_unk6FAB65F4 + 0x28));
    CAgent* selected =
        ((SSelectionRecord*)player->m_selection)->GetSelectedAgent();

    //  The same type-checked slot JASS item-handle resolution uses
    //  (itemtypedassign.h): its constructor is at 0x6F038890, which is what
    //  the shipped `lea ecx,[esp+var_4]` / `push eax` / `call` builds.
    SCheckedItemSlot held(selected);

    //  The comparison is made *before* the release, and its answer survives
    //  in the register the release then reuses - which is why the shipped
    //  code computes `setz` up front rather than after the drop.
    int same = (held.m_value == (CAgent*)this);
    return same;
}

//============================================================================
//  citem-batch-L: two more of CUnit's own selection-visual lifecycle
//  members, off CUnit::Reposition's own call tree
//  (asm/sub_6F2A5D50_0x6F2A5D50_calltree_asm.md) - the real bodies for the
//  two ADDR_THUNK_UNIT redirects unreconstructed_thunks.cpp used to carry
//  for these two addresses; both are deleted from that file now that their
//  real bodies land here (a redirect left behind after its body lands is a
//  second C++ name for the same shipped address - CLAUDE.md's own rule).
//============================================================================
#include "unit.h"

//  Config/configgates.cpp's own declarations, re-declared here exactly as
//  selectable_vtable.cpp already does (mangled name has to agree with the
//  real definition, not just compile).
float __fastcall GetConfigFloatHW(const char* section, const char* key, int index);
void  __fastcall AttachmentSetScale(unsigned int slot, const float* scaleXY);

//  The player-slot table singleton's own "acting (local) player" slot,
//  re-declared locally the way every other CUnit/CGameUI reader of it does
//  (unit_selectionhighlight.cpp, gameuicursorcolor.cpp) - except this one
//  read genuinely takes only the *low byte* of the field
//  (`mov cl,[ecx+28h]`, not the `movzx ... word ptr` every other reader
//  uses), which is what a `1 << slot` shift count needs and none of the
//  others do.
extern void* g_unk6FAB65F4;
struct SActingPlayerSlotByte
{
    char          m_reserved00[0x28];
    unsigned char m_actingSlot;   // +0x28 - low byte of the acting slot id
};

//----------------------------------------------------------------------------
//  0x6F010100 / 0x6F0100C0 - the terrain-side selection-circle primitive's
//  two release paths (both call into Storm's singleton lookup and a further
//  NTempest render-object chain this batch's own closure sweep has already
//  found to be terrain-rendering internals, not CItem/CUnit logic - see
//  citemclosure_batchL_thunks.cpp).  `retn 0`, `ecx` the primitive handle.
//----------------------------------------------------------------------------
void __fastcall ReleaseTerrainSelectionPrimitive(int handle);
void __fastcall ReleaseTerrainSelectionPrimitiveAndReset(int handle);

//  0x6F2917B0 - rebuild this unit's animation index table for the model
//  its current index now names.  Declared as a CUnit member in unit.h
//  beside TearDownSelectionVisual/NotifyModelChanged; still a thunk (see
//  citemclosure_batchL_thunks.cpp) - a real SEH-framed dynamic-array
//  rebuild this batch's own closure sweep found to be terrain-side model
//  bookkeeping, not CItem/CUnit placement logic.

//----------------------------------------------------------------------------
//  0x6F2859F0 - slot 97 (ReleaseSelectionVisual, selectable_vtable.cpp's
//  own tail dispatch) runs this first.  Drop the terrain-side selection
//  circle primitive at +0x2D0: if the local player's own visibility bit is
//  clear in m_playerMaskA (widget.h), release it outright and return early
//  (its own separate epilogue in the shipped stream); otherwise fall
//  through to the shared "already none" store both this path and the
//  `handle < 0` skip land on.
//----------------------------------------------------------------------------
void CUnit::TearDownSelectionVisual()
{
    int handle = (int)m_int2D0;
    if (handle >= 0)
    {
        unsigned int localSlot = ((SActingPlayerSlotByte*)g_unk6FAB65F4)->m_actingSlot;
        unsigned short localBit = (unsigned short)(1u << localSlot);
        if (!(m_playerMaskA & localBit))
        {
            ReleaseTerrainSelectionPrimitive(handle);
            m_int2D0 = (unsigned int)-1;
            return;
        }
        ReleaseTerrainSelectionPrimitiveAndReset(handle);
    }
    m_int2D0 = (unsigned int)-1;
}

//----------------------------------------------------------------------------
//  0x6F296C40 - slot 34.  Re-scale every visual attachment this unit is
//  carrying after a model change: read the new base scale off slot 113
//  (Method_0x1C4), push it (doubled into a two-float pair, the same
//  `{v,v}` shape CSelectable::SetAttachmentScale already carries) onto
//  m_attachmentSlot1/m_attachmentSlot2, then push
//  `virtualScale + AllyOffset*ScaleFactor` onto the selection-circle
//  attachment at m_handle278, and finally rebuild the animation index
//  table for the new model.
//----------------------------------------------------------------------------
//  the "none" sentinel every attachment-slot field is compared against -
//  unit_playertable.cpp's own extern declaration, re-declared here per this
//  repo's usual per-caller convention.
extern const unsigned int g_unk6F932954;

void CUnit::NotifyModelChanged(int index)
{
    float virtualScale = Method_0x1C4(index);

    float scaleFactor = GetConfigFloatHW("SelectionCircle", "ScaleFactor", 0);
    float allyOffset   = GetConfigFloatHW("SelectionCircle", "AllyOffset", 0);
    float sum = virtualScale + allyOffset * scaleFactor;

    if (m_attachmentSlot1 != g_unk6F932954)
    {
        float pair[2] = { virtualScale, virtualScale };
        AttachmentSetScale(m_attachmentSlot1, pair);
    }
    if (m_attachmentSlot2 != g_unk6F932954)
    {
        float pair[2] = { virtualScale, virtualScale };
        AttachmentSetScale(m_attachmentSlot2, pair);
    }
    if (m_handle278 != g_unk6F932954)
    {
        float pair[2] = { sum, sum };
        AttachmentSetScale(m_handle278, pair);
    }

    RebuildAnimationIndexTable(index);
}

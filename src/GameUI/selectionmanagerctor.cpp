//============================================================================
//  0x6F382D40 - CSelectionManager::CSelectionManager.
//
//  A CObserver, one owner pointer, a '+w3u' checked slot, sixty-odd cleared
//  words, a TEXTUREIMAGE, the eight-segment default geometry count, and the
//  [SelectionCircle] ColorFriend colour - then the circle geometry is built
//  once.
//
//  Two things the shipped body does that the source has to say out loud:
//
//    * the checked slot at +0x14 is constructed from the body and not from
//      an initialiser list, because MSVC runs member constructors before the
//      first statement and the shipped code runs this one after the vtable
//      stamp and the two integer stores;
//    * +0xFC is stored zero and then immediately overwritten by the colour
//      read.  Both stores are in the shipped stream (`mov [esi+0FCh], edi`
//      sits inside the call's own argument set-up), so both are here; the
//      zero is dead and the shipped compiler did not remove it either.
//
//  10.0f at +0xE8 and 0x1F4 at +0x10 are the shipped constants; the first is
//  the float at 0x6F94092C, which the instruction score cannot see either way.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with a base and seven sub-objects to unwind - the trampoline
//  alone destroys +0x14, +0x24, +0x34, +0x44, +0x60, +0x6C and +0xEC - none
//  of which this repo's fixed /GS- /EHs-c- emits.
//
//  Own translation unit: four real calls out of it.
//============================================================================
#include "selectionmanager.h"
#include "agenttypedslots.h"
#include "renderdevice.h"
#include "storm.h"

//  0x6F00A8B0 - Widget/configcolor.cpp, under the name that file gives it.
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);

CSelectionManager::CSelectionManager(void* owner)
{
    m_pOwner  = owner;
    m_field10 = 0x1F4;

    new (m_unit) SCheckedUnitSlot(0);

    m_field18 = 0;
    m_field1C = 0;
    m_scaledGeometry = 0;
    //  Written out rather than looped: a loop unrolls into stores off a
    //  *different* zero register than the ones around it, and the shipped
    //  stream is one run of `mov [esi+N], edi`.
    m_reserved24[0] = 0;
    m_reserved24[1] = 0;
    m_reserved24[2] = 0;
    m_reserved24[3] = 0;
    m_reserved24[4] = 0;
    m_reserved24[5] = 0;
    m_reserved24[6] = 0;
    m_reserved24[7] = 0;
    m_reserved24[8] = 0;
    m_reserved24[9] = 0;
    m_reserved24[10] = 0;
    m_reserved24[11] = 0;

    m_float54 = 0.0f;
    m_float58 = 0.0f;

    m_reserved60[0] = 0;
    m_reserved60[1] = 0;
    m_reserved60[2] = 0;
    m_reserved60[3] = 0;
    m_reserved60[4] = 0;
    m_reserved60[5] = 0;

    new (m_image) TEXTUREIMAGE;

    m_float90 = 0.0f;
    m_float94 = 0.0f;
    m_float98 = 0.0f;
    m_field9C = 0;
    m_fieldA0 = 0;

    //  Sixteen separate stores and not a loop: the shipped stream keeps one
    //  `fldz` live and writes the run with `fst`, and a loop compiles to
    //  integer zero stores instead.
    m_geometry[0]  = 0.0f;
    m_geometry[1]  = 0.0f;
    m_geometry[2]  = 0.0f;
    m_geometry[3]  = 0.0f;
    m_geometry[4]  = 0.0f;
    m_geometry[5]  = 0.0f;
    m_geometry[6]  = 0.0f;
    m_geometry[7]  = 0.0f;
    m_geometry[8]  = 0.0f;
    m_geometry[9]  = 0.0f;
    m_geometry[10] = 0.0f;
    m_geometry[11] = 0.0f;
    m_geometry[12] = 0.0f;
    m_geometry[13] = 0.0f;
    m_geometry[14] = 0.0f;
    m_geometry[15] = 0.0f;

    m_segmentCount = 4;
    m_floatE8 = 10.0f;

    m_fieldEC = 0;
    m_fieldF0 = 0;
    m_fieldF4 = 0;
    m_colorFriend = 0;

    unsigned int colour;
    GetConfigColor(&colour, "SelectionCircle", "ColorFriend", 0);
    m_colorFriend = colour;

    RebuildCircleGeometry();
}

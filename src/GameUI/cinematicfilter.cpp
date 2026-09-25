//============================================================================
//  0x6F368400 - CCinematicFilter::CCinematicFilter.
//
//  The world frame embeds one at +0x254 (0x6F39AB01).  The class name is the
//  shipped one, off the ??_7CCinematicFilter@@6B@ it stamps at +0x00 - and it
//  stamps it with *no* base constructor before it, so this class has no base
//  at all.
//
//  That vtable pointer is written as its **address** (0x6F93D834, read with
//  `ida_query xrefs` off the stamp itself) and not built from a `virtual`
//  declaration, for the reason Frame/clayer.h's LAYERNODE comment already
//  gives: declaring a virtual makes this build emit a vtable of its own -
//  here, one slot long over a shipped class whose length nothing in this tree
//  establishes - and the shipped constructor's own stamp is what would then
//  install it.  The cost is one instruction (`mov [esi], <imm>` against
//  `mov [esi], SYM`); the alternative is a wrong-length vtable no instruction
//  score can see.
//
//  Seven growable arrays, each resized to four, and the shapes really are
//  three different ones - which is why they are three different types here:
//
//    * +0x40 has an out-of-line SetCount (0x6F3079D0) and is one call;
//    * +0x4C is a word array whose SetCount is "if the count is wrong,
//      reallocate and take it" with no element construction at all;
//    * +0x58, +0x64 and +0x70 are arrays of float pairs and construct every
//      new element (which MSVC unrolls four ways with one `fldz` kept live
//      across the whole run);
//    * +0x80, +0x8C and +0x98 are int arrays and construct with a single
//      zero store, which MSVC does not unroll.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with two sub-objects and seven arrays to unwind.
//
//  Own translation unit: eight real calls out of it.
//============================================================================
#include "cinematicfilter.h"


//  Everything down to the TEXTUREIMAGE is a member initialiser, in
//  declaration order: that is the only spelling that runs the sub-object
//  constructor at +0x10, then the store at +0x1C, then the one at +0x20, with
//  no placement-new null test in front of either.  The one position it costs
//  is the vtable stamp, which the shipped stream schedules between the store
//  of +0x08 and the store of +0x0C.
CCinematicFilter::CCinematicFilter()
    : m_vtable((void*)kCinematicFilterVftable),
      m_float04(0.0f),
      m_float08(0.0f),
      m_field0C(0),
      m_params(0),
      m_field1C(0),
      m_image()
{
    m_field38 = 0;
    m_flags3C = (m_flags3C & ~0x3Fu) | 1;

    m_slots.m_alloc  = 0;
    m_slots.m_count  = 0;
    m_slots.m_data   = 0;
    m_order.m_alloc  = 0;
    m_order.m_count  = 0;
    m_order.m_data   = 0;
    m_pairsA.m_alloc = 0;
    m_pairsA.m_count = 0;
    m_pairsA.m_data  = 0;
    m_pairsB.m_alloc = 0;
    m_pairsB.m_count = 0;
    m_pairsB.m_data  = 0;
    m_pairsC.m_alloc = 0;
    m_pairsC.m_count = 0;
    m_pairsC.m_data  = 0;
    m_field7C        = 2;
    m_intsA.m_alloc  = 0;
    m_intsA.m_count  = 0;
    m_intsA.m_data   = 0;
    m_intsB.m_alloc  = 0;
    m_intsB.m_count  = 0;
    m_intsB.m_data   = 0;
    m_intsC.m_alloc  = 0;
    m_intsC.m_count  = 0;
    m_intsC.m_data   = 0;

    m_slots.SetCount(4);

    m_order.SetCount(4);
    m_order.m_data[0] = 0;
    m_order.m_data[1] = 1;
    m_order.m_data[2] = 2;
    m_order.m_data[3] = 3;

    m_pairsA.SetCount(4);
    m_pairsB.SetCount(4);
    m_pairsC.SetCount(4);

    m_intsA.SetCount(4);
    m_intsB.SetCount(4);
    m_intsC.SetCount(4);
}

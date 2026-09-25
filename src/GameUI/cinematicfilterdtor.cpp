//============================================================================
//  0x6F34F070 - CCinematicFilter::~CCinematicFilter.  `retn 0`.
//
//  Releases +0x1C (ReleaseAndClearField1C, Misc/releaseandclearfield1c.cpp -
//  a TRefCnt-derived handle this class's own constructor never sets, so
//  cinematicfilter.h's own field comment undersold it as a plain `int`),
//  frees each of the seven arrays' own storage directly through Storm
//  rather than any array type's own (thunked) SetAlloc/destructor, and
//  finally tears down +0x10 - the `SUnitCreateParamsSub` sub-object,
//  which Unit/unitcreateparams.h's own header comment already identifies
//  as the same twelve-byte object as `RCString` (same constructor address,
//  0x6F011300): the shipped stream calls `RCString::~RCString`
//  (0x6F4C48F0, Misc/rcstring.h) on it directly rather than through any
//  intermediate, so this does too, through a reinterpret rather than by
//  giving SUnitCreateParamsSub a destructor of its own that every other
//  host of that sub-object would then also pick up.
//
//  Each free's own logfilename argument is the freed array's real element
//  type's RTTI descriptor name (CImVector for the three plain-int arrays,
//  C2Vector for the three float-pair arrays, C3Vector for the twelve-byte
//  slot array, and the two-character ".G" for the word array) - not the
//  header's own simplified `int`/`SFilterPair` spellings, which is why
//  each literal is written out here instead of going through a
//  `TSGrowableArray<T>`-style shared destructor.
//============================================================================
#include "cinematicfilter.h"
#include "rcstring.h"

//  Declared locally to match Misc/releaseandclearfield1c.cpp's own mangled
//  name exactly, the same pattern GameUI/gameuinotifyleaf.cpp already uses
//  for a callee reconstructed in its own separate TU.
struct SField1CRefCntHost;
void __fastcall ReleaseAndClearField1C(SField1CRefCntHost* self);

CCinematicFilter::~CCinematicFilter()
{
    m_vtable = (void*)kCinematicFilterVftable;

    ReleaseAndClearField1C((SField1CRefCntHost*)this);

    if (m_intsC.m_data)
        SMemFree(m_intsC.m_data, ".?AVCImVector@NTempest@@", -2, 0);
    if (m_intsB.m_data)
        SMemFree(m_intsB.m_data, ".?AVCImVector@NTempest@@", -2, 0);
    if (m_intsA.m_data)
        SMemFree(m_intsA.m_data, ".?AVCImVector@NTempest@@", -2, 0);
    if (m_pairsC.m_data)
        SMemFree(m_pairsC.m_data, ".?AVC2Vector@NTempest@@", -2, 0);
    if (m_pairsB.m_data)
        SMemFree(m_pairsB.m_data, ".?AVC2Vector@NTempest@@", -2, 0);
    if (m_pairsA.m_data)
        SMemFree(m_pairsA.m_data, ".?AVC2Vector@NTempest@@", -2, 0);
    if (m_order.m_data)
        SMemFree(m_order.m_data, ".G", -2, 0);
    if (m_slots.m_data)
        SMemFree(m_slots.m_data, ".?AVC3Vector@NTempest@@", -2, 0);

    ((RCString*)&m_params)->~RCString();
}

//============================================================================
//  CItem's own vtable slots - the leaf half.  See item.h for the
//  declarations and docs/targets/CWidget_CSelectable_CItem__vtables.md for
//  the survey.  All of these sit in the 0x6F2B5xxx-0x6F2B7xxx cluster with
//  CItem::CItem (0x6F2B7520), and none calls another directly, so one
//  translation unit costs nothing the shipped code does not already have.
//============================================================================
#include "item.h"

//  0x6F009E30 - see configgates.cpp.
float __fastcall GetConfigFloatHW(const char* section, const char* key, int index);

//  flt_6F932004 - a plain IEEE single in .rdata; slot 47 `fld`s it straight
//  into st0.  Its value is not recoverable from the disassembly.
extern const float g_flt6F932004;

//----------------------------------------------------------------------------
//  0x6F2B5AC0 - slot 22.
//----------------------------------------------------------------------------
const char* CItem::GetClassName()
{
    return "CItem";
}

//----------------------------------------------------------------------------
//  0x6F2B7660 - slot 23.  A bare `jmp sub_6F472990`: CItem's vtable names
//  its own address for this slot, but the body is the base class's, reached
//  by an explicit qualified call so the compiler emits the tail jump instead
//  of recursing through the vtable.
//----------------------------------------------------------------------------
void CItem::Method_0x5C()
{
    CAgent::Method_0x5C();
}

//----------------------------------------------------------------------------
//  0x6F2B7770 - slot 25.
//----------------------------------------------------------------------------
unsigned int CItem::Method_0x64()
{
    return m_position.m_reserved74;
}

//----------------------------------------------------------------------------
//  0x6F2B5DA0 - slot 47.
//----------------------------------------------------------------------------
float CItem::Method_0xBC()
{
    return g_flt6F932004;
}

//----------------------------------------------------------------------------
//  0x6F2B76A0 - slot 60.  Its own two instructions at its own address, not
//  the inherited CWidget::QueryVisible (0x6F266A50) - the same body twice
//  over, which this image's missing /OPT:ICF (agentdefaults.cpp) leaves
//  standing as two functions.
//----------------------------------------------------------------------------
int CItem::QueryVisible(int)
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F2B6100 - slot 66.  CWidget's own slot 66
//  (widget_selectioncircle.cpp) is this times two.
//----------------------------------------------------------------------------
float CItem::Method_0x108()
{
    float scale = GetFootprintScaleFactor(m_footprintType);
    return GetConfigFloatHW("SelectionCircle", "ScaleFactor", 0) * scale;
}

//----------------------------------------------------------------------------
//  0x6F2B5C50 - slot 69.
//----------------------------------------------------------------------------
int CItem::Method_0x114()
{
    return 0x20;
}

//----------------------------------------------------------------------------
//  0x6F2B76E0 / 0x6F2B76F0 / 0x6F2B7700 - slots 75, 76 and 77.  The three
//  fields FloatMini/FloatMiniB carry, in the order the slots ask for them:
//  m_value (+0x58 absolute), m_max (+0x60) and m_min (+0x5C).
//
//  These three are what settled m_min/B2's type: floatmini.h used
//  to call them plain dwords because nothing read them back, and these
//  slots read them back through a CFloat out-buffer.
//----------------------------------------------------------------------------
CFloat* CItem::GetLife(CFloat* out)
{
    *out = m_floatB.m_value;
    return out;
}

CFloat* CItem::GetMaxLife(CFloat* out)
{
    *out = m_floatB.m_max;
    return out;
}

CFloat* CItem::GetMinLife(CFloat* out)
{
    *out = m_floatB.m_min;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F2B7710 / 0x6F2B7740 - slots 80 and 81.  Each writes *one* of
//  FloatMiniB's two fields and then re-writes the other with the value it
//  already had, before pushing the pair through FloatMini's own vtable slot
//  0.  That redundant second store is the tell: this is one two-argument
//  setter on FloatMiniB called with one fresh value and one read-back one,
//  not two independent single-field setters (which would each emit a single
//  store).  See FloatMiniB::SetPair in floatmini.h.
//----------------------------------------------------------------------------
void CItem::SetMinLife(const CFloat* value)
{
    m_floatB.SetPair(*value, m_floatB.m_max);
}

void CItem::SetMaxLife(const CFloat* value)
{
    m_floatB.SetPair(m_floatB.m_min, *value);
}

//----------------------------------------------------------------------------
//  0x6F2B7690 - slot 86.  A tail jump into this object's own slot 89.
//----------------------------------------------------------------------------
typedef int (__thiscall *Slot0x164Fn)(void* self);

int CItem::GetPlacementMask()
{
    return ((Slot0x164Fn)(*(void***)this)[0x164 / 4])(this);
}

//----------------------------------------------------------------------------
//  0x6F2B7670 / 0x6F2B7680 - slots 88 and 89.
//----------------------------------------------------------------------------
int CItem::Method_0x160()
{
    return 0x18;
}

int CItem::Method_0x164()
{
    return 0x10;
}

//----------------------------------------------------------------------------
//  0x6F2B76C0 / 0x6F2B76D0 - slots 92 and 93.  m_position's own
//  m_notifyHandle (position.h), read and written raw - no -1 check either
//  way, so the sentinel is the caller's business at this level.
//----------------------------------------------------------------------------
int CItem::GetPositionNotifyHandle()
{
    return (int)m_position.m_notifyHandle;
}

void CItem::SetPositionNotifyHandle(int value)
{
    m_position.m_notifyHandle = (unsigned int)value;
}

//============================================================================
//  0x6F2B7780 - CItem::~CItem().  See item.h.
//
//  Own translation unit, and it has to be: CItem::DeleteSelf
//  (agentwar3_dtor.cpp, vtable slot 1) reaches it with a real `call` in the
//  shipped code, and same-TU MSVC would inline this whole body into it.
//
//  What that split costs is one instruction here.  The shipped tail is
//  `mov [esi], offset ??_7CAgentWar3@@6B@` / `call sub_6F0017F0` -
//  ~CAgentWar3 inlined - and ~CAgentWar3's definition lives in
//  agentwar3_dtor.cpp, so from here the base destructor is a real
//  `call ??1CAgentWar3@@QAE@XZ` instead.  Same operations in the same
//  order, one call deeper.  There is no spelling that gets both: the two
//  functions cannot be in the same translation unit as each other *and* as
//  ~CAgentWar3 without DeleteSelf losing its own call.
//
//  Everything else is implicit member destruction in reverse declaration
//  order, which is exactly what the dump shows: m_timer2 (+0xE4) and
//  m_timer1 (+0xD0) through real calls to ~CAgentTimer, then Position's own
//  vtable reset at +0x64, then the base chain.  m_floatB's own reset at
//  +0x54 is a dead store the shipped build elides.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  multi-member destructor in this family (docs/msvc-vc8-idioms.md): two
//  member destructors the compiler cannot prove will not throw is enough on
//  its own, with no explicit __try/__finally in the source.
//============================================================================
#include "item.h"

CItem::~CItem()
{
}

//============================================================================
//  CTaskRotate's four serialisation-family vtable overrides -
//  GetRecordVersion, SaveFlags, LoadFlags, DumpState - all clustered at
//  0x6F287Exx, one module, one TU.  Save/Load (slots 14/15) are not here,
//  for the same reason task_save.cpp's own header comment gives one class
//  up: nothing in this call tree shows them re-overridden below CTask.
//============================================================================
#include "taskrotate.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Storm/textsink.h"
#include "../Math/CFloat.h"
#include "../Math/floatmini.h"

//  Forward-declared locally, same convention every other save/load TU in
//  this repo uses (see e.g. Item/item_save.cpp, Unit/unit_load.cpp).
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//----------------------------------------------------------------------------
//  0x6F287E00 - slot 10, GetRecordVersion.  `retn 4`.  CAgent's own answer
//  again - the shipped body calls straight into sub_6F471C10
//  (CAgent::GetRecordVersion) rather than through CTask::GetRecordVersion
//  (which would itself add 3), plus 4 this time.
//----------------------------------------------------------------------------
int CTaskRotate::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 4;
}

//----------------------------------------------------------------------------
//  0x6F287E70 - slot 17, DumpState.  `retn 4`.  Same three fields
//  CTask::DumpState (task_save.cpp) already dumps - m_flags (+0x20),
//  m_field24/m_field28 (+0x24, as a "ref" pair) and m_field30 (+0x30) -
//  plus this class's own new field: the FloatMini-shaped block at +0x34,
//  dumped through its own (non-virtual) DumpTo, the same direct-call shape
//  CItem::DumpState already established for a FloatMini/FloatMiniB member
//  (item_dump.cpp) - reinterpret_cast to `FloatMini*` at the point of use,
//  same convention as the plain-dword fields just above (taskrotate.h's own
//  header comment explains why this field is untyped storage).
//----------------------------------------------------------------------------
void CTaskRotate::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    reinterpret_cast<FloatMini*>(&m_field34)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F287E10 - slot 18, SaveFlags.  `retn 4`.  CTask::SaveFlags's own five
//  writes (task_save.cpp) unchanged, plus one more: the raw CFloat bits at
//  +0x38 (m_field38) written by address through WriteCFloat, bypassing the
//  FloatMini-shaped block's own vtable entirely - the shipped call site
//  never loads +0x34 at all, only +0x38.  29/32 (0.935): this compiler
//  stages the WriteCFloat argument through a stack temp (`mov edx,[esi+56]
//  / mov [esp+N],edx / lea edx,[esp+N]`) where the shipped call site takes
//  the field's own address directly (`lea edx,[esi+56]`) - tried both
//  `&m_field38` inline and a named `const CFloat*` local first (the same
//  shape item_save.cpp's own `pair` local uses for its own WriteCFloat
//  calls), identical generated code either way, so this is the compiler's
//  own choice at this call site, not a source spelling this repo's fixed
//  toolchain has a lever for.
//----------------------------------------------------------------------------
void CTaskRotate::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
    store->WriteDword(m_field24);
    store->WriteDword(m_field28);
    store->WriteDwordAlt2(m_field2C);
    store->WriteDword(m_field30);
    WriteCFloat(store, &m_field38);
}

//----------------------------------------------------------------------------
//  0x6F287EC0 - slot 19, LoadFlags.  `retn 4`.  CTask::LoadFlags's own two
//  reads (task_save.cpp) unchanged - m_flags as a plain dword, then
//  m_field24/m_field28/m_field2C in one ReadHandleTriple - plus, in place
//  of CTask's trailing plain-dword read of m_field30, a *third* value:
//  m_field30 is still read the same way, then the raw CFloat at +0x38 is
//  read into the *same* reused local the shipped code reuses for all three
//  raw reads (its own stack-argument slot, read three times over -
//  task_save.cpp's own CTask::LoadFlags already establishes the "one local,
//  reused" idiom for the first two; this file extends it to the CFloat read
//  too by reinterpreting that same local's address rather than declaring a
//  second one) and then *published* through the block's own vtable slot 0
//  - the same notify floatmini.h's own header comment documents and
//  Unit/unit_load.cpp's `PublishFloatMini` helper already uses for exactly
//  this shape, not a second WriteCFloat-style raw write.  29/33 (0.892):
//  the shipped body permanently advances its own `this` register
//  (`add esi, 52`) once it no longer needs the original base, and reaches
//  both the vtable fetch and the notify call's own `this` through that same
//  advanced esi; this build instead computes `&m_field34` as a fresh `lea`
//  off the unmoved `this` (ecx here, not esi) and keeps it in its own
//  register for the notify call - same address, same two calls in the same
//  order, one different way of holding it. Register allocation/instruction
//  selection only, pending angr IDENTICAL verification.
//----------------------------------------------------------------------------
typedef void (__thiscall *FloatMiniNotifyFn)(void*, CFloat*, int);

void CTaskRotate::LoadFlags(CDataStore* store)
{
    unsigned int value;

    store->ReadDword(&value);
    m_flags = value;

    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field24));

    store->ReadDword(&value);
    m_field30 = value;

    ReadCFloat(store, reinterpret_cast<CFloat*>(&value));
    ((FloatMiniNotifyFn)(*(void***)&m_field34)[0])(&m_field34,
                                                     reinterpret_cast<CFloat*>(&value),
                                                     0);
}

//============================================================================
//  CTaskGhost's four serialisation-family vtable overrides -
//  GetRecordVersion, SaveFlags, LoadFlags, DumpState - all clustered at
//  0x6F288Axx-0x6F288Cxx, one module, one TU.  Save/Load (slots 14/15) are
//  not here, for the same reason task_save.cpp's own header comment gives
//  three classes up: nothing in this call tree shows them re-overridden
//  below CTask.
//============================================================================
#include "taskghost.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Storm/textsink.h"
#include "../Math/CFloat.h"
#include "../Math/floatmini.h"

//  Forward-declared locally, same convention every other save/load TU in
//  this repo uses (see e.g. Item/item_save.cpp, Task/taskrotate_save.cpp).
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//----------------------------------------------------------------------------
//  0x6F288AD0 - slot 10, GetRecordVersion.  `retn 4`.  CAgent's own answer
//  again - the shipped body calls straight into sub_6F471C10
//  (CAgent::GetRecordVersion) rather than through any base class's own
//  GetRecordVersion, plus 0xA (10) this time.
//----------------------------------------------------------------------------
int CTaskGhost::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 0xA;
}

//----------------------------------------------------------------------------
//  0x6F288B50 - slot 17, DumpState.  `retn 4`.  The same four fields
//  CTaskParam::DumpState (taskparam_save.cpp) already dumps - m_flags
//  (+0x20), m_field24/m_field28 (+0x24, as a "ref" pair), m_field30
//  (+0x30) and m_field34 (+0x34), all four as SIntMiniValue/SRefMiniValue
//  the same way - plus this class's own four new plain dwords (+0x38
//  through +0x44, each SIntMiniValue::DumpTo again) and its own two
//  FloatMini-shaped blocks (+0x48 and +0x50, each dumped through its own
//  non-virtual FloatMini::DumpTo) - reinterpret_cast to the right pointer
//  type at the point of use, same convention as the plain-dword fields
//  (taskghost.h's own header comment explains why the FloatMini blocks are
//  untyped storage).  Inlined here rather than calling
//  CTaskParam::DumpState/CTaskOrderParam::DumpState: the shipped body makes
//  ten direct `call`s off its own `this`, not one call into a base
//  DumpState followed by four more - same "each class re-emits its own
//  ancestors' dumps inline" shape task_save.cpp/taskparam_save.cpp/
//  taskrotate_save.cpp already establish for DumpState specifically
//  (unlike SaveFlags/LoadFlags below, which do call their base).
//----------------------------------------------------------------------------
void CTaskGhost::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field34)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field38)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field3C)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field40)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field44)->DumpTo(sink);
    reinterpret_cast<FloatMini*>(&m_field48)->DumpTo(sink);
    reinterpret_cast<FloatMini*>(&m_field50)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F288AE0 - slot 18, SaveFlags.  `retn 4`.  A real call into
//  CTaskParam::SaveFlags (0x6F287CE0, its own six writes for +0x20 through
//  +0x34 unchanged), then this class's own four new dwords written via
//  WriteDword (not WriteDwordAlt2 - only +0x2C, inside the base call,
//  gets the "alt" writer), then the raw CFloat bits at +0x4C/+0x54, each
//  copied into one reused stack-local dword first and *that* local's
//  address passed to WriteCFloat (the shipped body's own
//  `mov ecx,[esi+4Ch] / mov [esp+N],ecx / lea edx,[esp+N]` shape, not a
//  direct `lea edx,[esi+4Ch]` - unlike CTaskRotate's own single-field
//  SaveFlags, which the compiler happened to generate identically either
//  way, this two-field call site only matches with an explicit local copy)
//  - one `unsigned int temp`, reused for both calls exactly the way
//  CTaskParam::LoadFlags's own single `value` local is reused across
//  unrelated reads (task_save.cpp), per this wave's own "three FloatMini-
//  style fields read together often share one reused stack local" note.
//  A plain `unsigned int` rather than a `CFloat` local: a `CFloat temp;`
//  compiles a real default-construction store (`mov [esp+N], 0`) ahead of
//  the first assignment that the shipped body never makes, costing five
//  instructions (30/38, 0.822) - the raw dword, bit-copied through
//  reinterpret_cast and handed to WriteCFloat as a `CFloat*`, matches
//  exactly (35/35, 1.000).  Bypasses each FloatMini block's own vtable
//  entirely, same as CTaskRotate's own SaveFlags (taskrotate_save.cpp).
//----------------------------------------------------------------------------
void CTaskGhost::SaveFlags(CDataStoreScratch* store)
{
    unsigned int temp;

    CTaskParam::SaveFlags(store);
    store->WriteDword(m_field38);
    store->WriteDword(m_field3C);
    store->WriteDword(m_field40);
    store->WriteDword(m_field44);

    temp = *reinterpret_cast<const unsigned int*>(&m_field4C);
    WriteCFloat(store, reinterpret_cast<CFloat*>(&temp));

    temp = *reinterpret_cast<const unsigned int*>(&m_field54);
    WriteCFloat(store, reinterpret_cast<CFloat*>(&temp));
}

//----------------------------------------------------------------------------
//  0x6F288BD0 - slot 19, LoadFlags.  `retn 4`.  1.000 EXACT (57/57).
//  Mirrors SaveFlags: a real call into CTaskParam::LoadFlags (0x6F287D70,
//  its own reads for +0x20 through +0x34 unchanged), then this class's own
//  four new dwords each read into the shipped call site's own reused
//  stack local (`value`) and stored, then each FloatMini block's raw
//  CFloat read back into that same reused local and *published* through
//  the block's own vtable slot 0 - the same notify shape CTaskRotate's own
//  LoadFlags (taskrotate_save.cpp) establishes, applied twice over (once
//  per block; the shipped body holds each block's own address in a
//  register once - `ebx` for the first, `esi` advanced by +0x50 for the
//  second - and reuses it for both the vtable fetch and the notify call's
//  own `this`).  A named `void* first`/`void* second` local for each
//  block's address, rather than re-spelling `&m_field48`/`&m_field50`
//  inline at both use sites, is load-bearing here, not just style: inline
//  the compiler recomputes the address twice (one `lea` for the vtable
//  load, a second for the `this` argument - 26/57, 0.473); with a named
//  local it computes it once and keeps it live across both, matching the
//  shipped register reuse exactly.
//----------------------------------------------------------------------------
typedef void (__thiscall *FloatMiniNotifyFn)(void*, CFloat*, int);

void CTaskGhost::LoadFlags(CDataStore* store)
{
    unsigned int value;

    CTaskParam::LoadFlags(store);

    store->ReadDword(&value);
    m_field38 = value;

    store->ReadDword(&value);
    m_field3C = value;

    store->ReadDword(&value);
    m_field40 = value;

    store->ReadDword(&value);
    m_field44 = value;

    void* first = &m_field48;
    ReadCFloat(store, reinterpret_cast<CFloat*>(&value));
    ((FloatMiniNotifyFn)(*(void***)first)[0])(first,
                                                reinterpret_cast<CFloat*>(&value),
                                                0);

    void* second = &m_field50;
    ReadCFloat(store, reinterpret_cast<CFloat*>(&value));
    ((FloatMiniNotifyFn)(*(void***)second)[0])(second,
                                                 reinterpret_cast<CFloat*>(&value),
                                                 0);
}

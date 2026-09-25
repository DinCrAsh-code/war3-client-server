//============================================================================
//  0x6F34FC90 - CEscMenu::~CEscMenu().  See escmenu.h.
//
//  Own translation unit for the same reason agent_deleteself.cpp's own
//  header comment gives for CAgent: CEscMenu::DeleteSelf
//  (escmenu_deleteself.cpp, vtable slot 1) reaches this destructor with a
//  real `call sub_6F34FC90` in the shipped code; compiled beside that
//  override, this build inlines the whole release chain into it instead.
//
//  The `__except_handler4`-shaped SEH frame this destructor carries in the
//  dump (push -1 / push offset SEH_6F34FC90 / mov large fs:0 / the
//  cookie-XORed, EBP-omitted shape, and the appended scope-table
//  trampoline) is the same one docs/msvc-vc8-idioms.md documents as
//  unreproducible at this toolchain's fixed /GS- /EHs-c- - see "An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce", and
//  CObserver::~CObserver (observer.cpp) for the same finding on the base
//  class this destructor itself calls into automatically after its own
//  body runs. Nothing here writes an explicit `__try`/`__finally`; MSVC
//  puts this frame around any destructor whose base class has one to run
//  behind a call the compiler cannot prove will not unwind, exactly as
//  observer.cpp's own note explains.
//============================================================================
#include "escmenu.h"

//----------------------------------------------------------------------------
//  The compiler's own "drop one reference, self-destruct once the count
//  hits zero" shape, inlined identically at all five fields below: `test`,
//  `add [obj+4], -1`, and - only on the transition to zero - `call
//  [[obj]]`, i.e. slot 0 (TRefCnt::ReleaseSelf) reached through the raw
//  vtable pointer rather than by name. That is *not* TRefCnt::Release()
//  (0x6F4C40A0, refcnt.h) inlined - Release() tail-calls slot 1
//  (DeleteSelf) with its own documented zero-argument quirk, and this
//  shape reads slot 0 - so it is written here exactly as the dump shows it
//  rather than routed through Release() by name.
//
//  A `void*` parameter, not `TRefCnt*`: three of the five fields below
//  point at classes (CEscMenuMainPanel/OptionsPanel/SaveGamePanel) this
//  session leaves forward-declared only, and an implicit upcast to a real
//  base needs a complete type C++ does not have here. All five objects in
//  this chain put their vtable and refcount at offset 0 regardless (single
//  first-base inheritance throughout - CFrame's own TRefCnt subobject is
//  at 0x00, same as CObserver's), so the raw offsets below are exactly
//  what a real `(TRefCnt*)` cast would reach if the types were complete.
//----------------------------------------------------------------------------
typedef void (__thiscall *ReleaseSelfFn)(void*);

static void ReleaseRefSlot(void* obj)
{
    if (obj && --*(int*)((char*)obj + 4) == 0)
        ((ReleaseSelfFn)(*(void***)obj)[0])(obj);
}

CEscMenu::~CEscMenu()
{
    ReleaseRefSlot(m_pSaveGamePanel);
    ReleaseRefSlot(m_pOptionsPanel);
    ReleaseRefSlot(m_pMainPanel);
    ReleaseRefSlot(m_pActivePanel);
    ReleaseRefSlot(m_pContainer);
    //  CObserver::~CObserver() (observer.cpp) runs automatically from here.
}

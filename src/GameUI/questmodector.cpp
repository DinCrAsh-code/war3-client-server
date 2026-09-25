//============================================================================
//  0x6F33B0D0 - CQuestMode::CQuestMode.
//
//  The class name is the shipped one: the body stamps
//  ??_7CQuestMode@@6B@ over the CObserver base's vtable pointer.  CGameUI's
//  constructor makes it with a 0x10-byte allocation tagged ".\\CGameUI.cpp"
//  line 0x20A and parks it at +0x224 (0x6F2FFF19).
//
//  0x10 bytes is CObserver's twelve plus one word, and that word is all this
//  constructor writes.  The `CGameUI*` it is handed is never read - the
//  shipped body pushes nothing before `call sub_6F629A70`, whose own `retn`
//  is 0, and never touches [esp+8] - but it *is* a parameter: the function
//  is `retn 4`.  Left in and left unused, with this note, because dropping
//  it would change the calling convention the caller was compiled against.
//============================================================================
#include "observer.h"

class CGameUI;

class CQuestMode : public CObserver
{
public:
    CQuestMode(CGameUI* ui);

    int m_field0C;      // +0x0C
};

CQuestMode::CQuestMode(CGameUI*)
{
    m_field0C = 0;
}

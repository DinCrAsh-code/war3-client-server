//============================================================================
//  0x6F34A940 - CScriptDialogMode::CScriptDialogMode.
//
//  CQuestMode's twin, sixteen kilobytes further on and one module of its
//  own: the same CObserver base, the same 0x10-byte record, the same single
//  cleared word at +0x0C, and the same ".\\CGameUI.cpp" allocation (line
//  0x20B, parked at CGameUI +0x240, 0x6F2FFF77).  The class name is the
//  ??_7CScriptDialogMode@@6B@ the body stamps.
//
//  Unlike CQuestMode's this one takes nothing - `retn 0`, and the call site
//  pushes nothing.
//============================================================================
#include "observer.h"

class CScriptDialogMode : public CObserver
{
public:
    CScriptDialogMode();

    int m_field0C;      // +0x0C
};

CScriptDialogMode::CScriptDialogMode()
{
    m_field0C = 0;
}

//============================================================================
//  0x6F4D30C0 - a one-instruction `jmp sub_6F4C3E90` (CStatus::~CStatus,
//  Render/statusdtor.cpp), reached (as a tail jmp) from inside the still-
//  self-redirected SetSpriteModel (0x6F4D8D60, Widget's own out-of-scope
//  188-instruction body - see funcmap.py's own note on that address) and
//  from one more unnamed leaf (sub_6F4D95C0).  Neither caller is CStatus-
//  shaped, so this is very likely the same identical-code-folding shape
//  Pathfinding/cellbuffer.cpp's ReleaseCMemBlockBaseAlias already documents
//  for a different pair of addresses: two logically distinct destructors
//  whose compiled bodies happened to be byte-identical, folded to one body
//  with every other call site redirected onto it.  Reproduced the same way -
//  a naked tail jump onto the name this repo already owns for that body -
//  rather than inventing a second, unreachable copy of CStatus::~CStatus.
//============================================================================
#include "cstatus.h"

__declspec(naked) void Thunk_0x6F4D30C0()
{
    __asm { jmp CStatus::~CStatus }
}

//============================================================================
//  CFrame's own slots 6/7/8 (+0x18/+0x1C/+0x20).  The real handoff bodies
//  (SetOwner66C/674/678) are deliberately in a *different* translation unit
//  (frameownertoggle.cpp) - see that file's own header comment for why.
//============================================================================
#include "frame.h"

//  frameownertoggle.cpp.
CLayer* __fastcall SetOwner66C(CLayer* candidate);
CLayer* __fastcall SetOwner674(CLayer* candidate);
CLayer* __fastcall SetOwner678(CLayer* candidate);

extern CLayer* g_frameTokenOwner66C;   // dword_6FACE66C
extern CLayer* g_frameTokenOwner674;   // dword_6FACE674
extern CLayer* g_frameTokenOwner678;   // dword_6FACE678

namespace {

//  `(current == self) ? 0 : current` - give up the token only while I still
//  hold it, otherwise pass the current holder along unchanged (a no-op
//  re-assignment the callee's own `candidate == current` check absorbs) -
//  the shipped `sub/neg/sbb/and` idiom for exactly that select.
inline CLayer* KeepUnlessMine(CLayer* self, CLayer* current)
{
    return (current == self) ? 0 : current;
}

}  // namespace

CLayer* CFrame::Method_0x18(int take)
{
    if (take)
        return SetOwner66C(this);
    return SetOwner66C(KeepUnlessMine(this, g_frameTokenOwner66C));
}

CLayer* CFrame::Method_0x1C(int take)
{
    if (take)
        return SetOwner674(this);
    return SetOwner674(KeepUnlessMine(this, g_frameTokenOwner674));
}

CLayer* CFrame::Method_0x20(int take)
{
    if (take)
        return SetOwner678(this);
    return SetOwner678(KeepUnlessMine(this, g_frameTokenOwner678));
}

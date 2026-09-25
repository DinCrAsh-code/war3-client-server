//============================================================================
//  Jass2/Nodes.h - `DeleteSelf` (Node's own vtable slot 0, the "scalar
//  deleting destructor" MSVC would otherwise synthesize invisibly - see
//  jassnodes.h's own note on why Node declares it explicitly instead).
//  jassvm-sweep-K5's own closure: nineteen classes across
//  jassnodectors.cpp (thirteen), jassnodes.h (three:
//  NodeDebug/NodeFunction/NodeArgList) and jassnodedtors.cpp (three:
//  NodeIf/NodeCode/NodeCall).
//
//  Every one of these has the identical shape - `this->NodeXxx::~NodeXxx();
//  if ((flags & 1) && this != 0) SMemFree(this, "delete", -1, 0); return
//  this;` - already established in this repo for `CEscMenu::DeleteSelf`
//  (GameUI/escmenu_deleteself.cpp) and `PositionTrack::DeleteSelf`
//  (Pathfinding/positiontrack.h).  Defined in its own translation unit,
//  separate from every one of the nineteen `~NodeXxx()` bodies above, for
//  the same reason escmenu_deleteself.cpp's own header comment gives: a
//  same-TU destructor call here is exactly the shape /Ob2 folds into its
//  caller, and the shipped body is a real out-of-line `call` at every one
//  of these addresses.
//============================================================================
#include "jassnodes.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//  0x6F460B30
Node* NodeGoal::DeleteSelf(int flags)
{
    this->NodeGoal::~NodeGoal();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F460C70
Node* NodeFile::DeleteSelf(int flags)
{
    this->NodeFile::~NodeFile();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F460DF0
Node* NodeDecl::DeleteSelf(int flags)
{
    this->NodeDecl::~NodeDecl();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F460F40
Node* NodeNative::DeleteSelf(int flags)
{
    this->NodeNative::~NodeNative();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F461060
Node* NodeStartFunc::DeleteSelf(int flags)
{
    this->NodeStartFunc::~NodeStartFunc();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F4612B0
Node* NodeFunction::DeleteSelf(int flags)
{
    this->NodeFunction::~NodeFunction();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F461C00
Node* NodeIf::DeleteSelf(int flags)
{
    this->NodeIf::~NodeIf();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F461D00
Node* NodeLoop::DeleteSelf(int flags)
{
    this->NodeLoop::~NodeLoop();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F461790
Node* NodeCode::DeleteSelf(int flags)
{
    this->NodeCode::~NodeCode();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F461E00
Node* NodeExitWhen::DeleteSelf(int flags)
{
    this->NodeExitWhen::~NodeExitWhen();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F461F10
Node* NodeReturn::DeleteSelf(int flags)
{
    this->NodeReturn::~NodeReturn();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F4621B0
Node* NodeParams::DeleteSelf(int flags)
{
    this->NodeParams::~NodeParams();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F462360
Node* NodeArgList::DeleteSelf(int flags)
{
    this->NodeArgList::~NodeArgList();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F4630E0
Node* NodeLocals::DeleteSelf(int flags)
{
    this->NodeLocals::~NodeLocals();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F4631D0
Node* NodeGlobals::DeleteSelf(int flags)
{
    this->NodeGlobals::~NodeGlobals();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F463350
Node* NodeGlobalList::DeleteSelf(int flags)
{
    this->NodeGlobalList::~NodeGlobalList();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F463600
Node* NodeParam::DeleteSelf(int flags)
{
    this->NodeParam::~NodeParam();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F463790
Node* NodeCall::DeleteSelf(int flags)
{
    this->NodeCall::~NodeCall();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F4609E0
Node* NodeDebug::DeleteSelf(int flags)
{
    this->NodeDebug::~NodeDebug();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//============================================================================
//  0x6F2AE850 - CWidget vtable[0xD0] (index 52).
//
//  widget.cpp's CWidget::MoveForward calls this slot before its search and
//  again after, and its own comment guesses at "a vtable[0xD0] lock/unlock
//  pair".  That guess is now testable, and it is close but not what the
//  body does: there is no lock anywhere in it.  It resolves the widget's
//  own CAgent handle, checks the object behind it carries one specific type
//  tag and is not already busy, and then **adjusts a reference count** -
//  +1 when the flag is set, -1 when it is clear - on the path data hanging
//  off whatever the widget's handle sub-object points at, and on every
//  entry of a second array at CWidget+0x34 if it has one.
//
//  So the pair really is balanced, and really does bracket the search, but
//  what it brackets is a pathing-data pin, not mutual exclusion.  The
//  comment in widget.cpp is corrected to say so.
//
//  The type tag is the literal dword 0x2B61676C.  Read as bytes in memory
//  that is `l g a +`, the same "+"-suffixed four-character agent tag family
//  the rest of this binary uses; it is compared as an integer, never as
//  text, so it is written as the integer here.
//============================================================================
#include "widget.h"
#include "game.h"
#include "widgetpathrefadjust.h"

typedef SPathRefHandle* (__thiscall *GetHandleObjectFn)(void*);

//  The object LookupHandle hands back for a CAgent handle.  Only the two
//  fields this function tests are named.
struct SAgentHandleObject
{
    char         m_reserved00[0x0C];
    unsigned int m_typeTag;      // +0x0C, must equal kAgentTagPathable
    char         m_reserved10[0x10];
    unsigned int m_busy;         // +0x20, must be zero
};

//  '+agl' MSB first - the agile type id every handle-registered object
//  carries, one byte off CAgent's own '+aga'.  Same constant as
//  kAgileHandleTag (agentregistry.h), which carries the full note.
const unsigned int kAgentTagPathable = 0x2B61676C;

void CWidget::AdjustWidgetPathRefs(int add)
{
    void* vself = this;

    //  CAgent's own handle/type pair at +0x0C/+0x10 - not the widget's
    //  handle sub-object, which is reached through vtable[0xB8] below.
    SAgentHandleObject* agent = (SAgentHandleObject*)
        LookupHandle(*(unsigned int*)((char*)this + 0x0C),
                     *(int*)((char*)this + 0x10));
    if (agent == 0)
        return;

    //  The tag test and the null test share one branch in the shipped code:
    //  `setnz`/`sub 1`/`and` turns "tag matched" into an all-ones mask,
    //  ANDs the pointer with it, and falls through to a single `jz`.  That
    //  is what a *checked cast written as a conditional expression* lowers
    //  to - the pointer is either itself or null - and not what two
    //  independent `if`s give, which is two compares and two branches.
    SAgentHandleObject* pathable =
        (agent->m_typeTag == kAgentTagPathable) ? agent : 0;
    if (pathable == 0)
        return;
    if (pathable->m_busy != 0)
        return;

    ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself)
        ->AdjustPathRefCount(add);

    SPathRefArray* extra = (SPathRefArray*)m_pathGridRegistration;
    if (extra != 0)
        extra->AdjustPathRefArray(add);
}

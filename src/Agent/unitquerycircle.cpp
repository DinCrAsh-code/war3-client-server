//============================================================================
//  0x6F476DC0 / 0x6F476670 - the circle form of a unit enumeration: where
//  the circle is centred, and how big the centre object itself is.
//
//  **The receiver is a CPathRef** (game.h).  0x6F476DC0 calls
//  CPathRef::ToWorldVec3 on the very `this` it was given, and 0x6F476670
//  resolves the {handle, typeTag} pair at +0x08/+0x0C that CPathRef's own
//  layout has - so `SUnitQueryCircleCentre` and `CPathRef` are one object
//  seen from two directions.  The cast below is that identification, not a
//  reinterpretation of unrelated bytes; the second name survives only
//  because BuildAndRegisterEventBinding's own call site is already
//  compiled against these parameter types.
//
//  `altForm` decides the distance rule the same way it does in
//  EnumerateUnitsInRect (agentqueryrect.cpp): set means a candidate counts
//  when its own collision circle *touches* the query circle, and then the
//  query radius is first grown by the centre object's own radius; clear
//  means the candidate's centre has to be inside, and the caller's radius
//  travels down untouched.  That is why only the touching arm computes a
//  sum, and why the two arms hand the enumerator a different pointer - one
//  the local sum's, one the caller's own.
//
//  Both of the shipped function's own temporaries live in the *parameter*
//  homes of `origin` and `altForm`: its frame reserves twelve bytes,
//  exactly the three words of the position, and MSVC puts the sum and
//  GetWorldRadius's scratch in the two incoming slots it has finished
//  with - the same reuse FloatListener::Init's own spilled `resolved`
//  shows.  This build reuses only *one* of the two homes and takes a
//  sixteenth byte of frame for the other, so every `[esp+N]` after the
//  prologue reads four higher.  The instruction stream is otherwise
//  identical, instruction for instruction, in both arms; three spellings
//  of the scratch were measured (an `int` local inside the branch, a
//  CFloat local inside it, and a CFloat hoisted above the branch) and all
//  three give the same frame.
//
//  Own translation unit: EnumerateUnits calls GetWorldRadius for real.
//  They are one shipped module (0x6F476xxx) and neither may be inlined
//  into the other, so the pair is split across this file and
//  unitquerycircleradius.cpp.
//============================================================================
#include "agenteventbinding.h"
#include "agentquery.h"
#include "game.h"

void SUnitQueryCircleCentre::EnumerateUnits(const int* origin, int altForm,
                                            unsigned int mask, int a, int b,
                                            UnitQueryVisitFn fn, void* ctx)
{
    CWorldVec3 centre;
    ((CPathRef*)this)->ToWorldVec3(centre);

    if (altForm != 0)
    {
        //  The scratch GetWorldRadius writes through, and the sum.  Bound
        //  through the pointer GetWorldRadius hands back rather than
        //  through a second read of `own`, which is what puts the shipped
        //  `push eax` straight into operator+'s argument list.
        int own;
        CFloat total = *(const CFloat*)origin +
                       *(const CFloat*)GetWorldRadius(&own);

        EnumerateAgentsTouchingCircle((const int*)&centre.m_x,
                                      (const int*)&centre.m_y,
                                      (const int*)&total,
                                      mask, (unsigned int)a, b, fn, ctx);
    }
    else
    {
        EnumerateAgentsInCircle((const int*)&centre.m_x,
                                (const int*)&centre.m_y,
                                origin,
                                mask, (unsigned int)a, b, fn, ctx);
    }
}

//============================================================================
//  0x6F4C5D50, 0x6F4C5DD0 and 0x6F4C5E70 - RCString's two append operations
//  and the concatenation helper they share.
//
//  The helper is a *member* whose body never reads `this` (`retn 0Ch` with a
//  receiver in ecx and three pushes, and nothing in the body touches ecx) -
//  which is what a static member function of the class looks like once the
//  compiler has emitted the call site.  It builds its answer in a 0x1001-byte
//  stack buffer, which is where the append's own length limit comes from.
//
//  Both appends have the same three-way shape: an empty string takes the
//  other one whole, a null argument does nothing at all, and anything else
//  goes through a temporary that is assigned and then destroyed.  The
//  temporary is what makes `s.Append(s)` safe.
//============================================================================
#include "rcstring.h"
#include "jassnatives.h"    // STStringField
#include "os.h"             // Storm_501, Storm_503


//----------------------------------------------------------------------------
//  0x6F4C5D50 - build `out` as `a` followed by `b`.
//----------------------------------------------------------------------------
RCString* RCString::Concat(RCString* out, const char* a, const char* b)
{
    char text[0x1001];

    text[0] = 0;
    Storm_501(text, a, 0x1001);
    Storm_503(text, b, 0x7FFFFFFF);

    out->Construct(text);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F4C5DD0 - append a plain string.
//----------------------------------------------------------------------------
RCString* RCString::AppendCStr(const char* text)
{
    const char* mine = (const char*)GetHandleOrZero((const SHandleHolder*)this);

    if (mine == 0)
    {
        ((STStringField*)this)->Assign(text);
        return this;
    }

    if (text != 0)
    {
        //  A local with a destructor: Concat is what constructs it (it
        //  calls the counted string's own constructor on the object it is
        //  handed), and the scope exit is what destroys it.
        RCString joined;
        Concat(&joined, mine, text);
        Assign(&joined);
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C5E70 - append another RCString.
//----------------------------------------------------------------------------
RCString* RCString::AppendRCString(const RCString* other)
{
    const char* mine = (const char*)GetHandleOrZero((const SHandleHolder*)this);
    const char* theirs = (const char*)GetHandleOrZero((const SHandleHolder*)other);

    if (mine == 0)
    {
        ((STStringField*)this)->Assign(theirs);
        return this;
    }

    if (theirs != 0)
    {
        RCString joined;
        Concat(&joined, mine, theirs);
        Assign(&joined);
    }

    return this;
}

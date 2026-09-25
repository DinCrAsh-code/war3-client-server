//============================================================================
//  0x6F3409B0 - CSaveGame::MapPath.  The path the save was made from, or -
//  when the header never read - the empty string a freshly built
//  STStringField answers.
//
//  The empty case really does construct and destroy a whole temporary string
//  rather than returning a literal, which is what the `bl & 1` guard in the
//  stream is: a "was the temporary built" flag the destructor is gated on.
//============================================================================
#include "savegame.h"
#include "handleobject.h"

#include "rcstring.h"


const char* __fastcall SaveGameMapPath(CSaveGame* self)
{
    char temporary[0x0C];
    const void* source;
    int built = 0;

    if (self->m_valid == 0)
    {
        ((RCString*)temporary)->Construct(0);
        built = 1;
        source = temporary;
    }
    else
    {
        source = &self->m_string418;
    }

    const char* answer =
        (const char*)GetHandleOrZeroAlias((const SHandleHolder*)source);

    if (built & 1)
        ((RCString*)temporary)->~RCString();

    return answer;
}

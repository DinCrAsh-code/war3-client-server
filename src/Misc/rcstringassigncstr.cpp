//============================================================================
//  0x6F4C5CF0 - assign a C string into the counted string at `this`.
//
//  Kept under the name Jass/jassnatives.h already binds to this address -
//  STStringField::Assign - because four translation units already call it
//  under that name, and a second C++ name for one shipped address costs
//  every one of those call sites its match.  It replaces the naked redirect
//  jassnatives_thunks.cpp used to carry for it.
//
//  A null assignment does not go near the manager: it bumps the shared empty
//  rep's own refcount directly (the `dword_6FA85E1C` in the stream is
//  g_emptyRCStringData.m_refcount) and adopts it.
//============================================================================
#include "cstringrep.h"
#include "rcstring.h"
#include "jassnatives.h"

void STStringField::Assign(const char* value)
{
    RCString* self = (RCString*)this;

    if (value == 0)
    {
        g_emptyRCStringData.m_refcount++;

        CStringRep* old = (CStringRep*)self->m_data;
        if (old != 0)
        {
            typedef void (__thiscall *ReleaseFn)(void*);
            ((ReleaseFn)((void**)old->m_vtable)[2])(old);
        }

        self->m_data = (RCSTRINGDATA*)&g_emptyRCStringData;
        return;
    }

    CStringRep* rep = GetStringManager()->Intern(value);
    if (rep != 0)
        rep->m_refcount++;

    CStringRep* old = (CStringRep*)self->m_data;
    if (old != 0)
    {
        typedef void (__thiscall *ReleaseFn)(void*);
        ((ReleaseFn)((void**)old->m_vtable)[2])(old);
    }

    self->m_data = (RCSTRINGDATA*)rep;
}

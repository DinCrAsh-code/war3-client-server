//============================================================================
//  0x6F476670 - SUnitQueryCircleCentre::GetWorldRadius: resolve the traced
//  object this ref names and hand back its collision radius in world units.
//
//  The conversion is game.h's own GridToWorld - add five exponent steps,
//  leaving an encoded zero alone - which is what the shipped
//  `and`/`neg`/`sbb`/`and 0x2800000`/`add` is, and 32 world units to the
//  pathing cell is why five.  Writing it as a literal
//  `bits + (bits & 0x7F800000 ? 0x2800000 : 0)` would say the same thing
//  in a way the next reader has to re-derive; the helper already carries
//  the note.
//
//  The out buffer is returned, which is what puts it in eax at the tail -
//  the shipped `mov eax,[esp+arg_0]` after the store, not before it.
//
//  Own translation unit: its one caller in this tree reaches it with a
//  real call - see unitquerycircle.cpp.
//============================================================================
#include "agenteventbinding.h"
#include "game.h"

int* SUnitQueryCircleCentre::GetWorldRadius(int* out)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(((CPathRef*)this)->m_handle,
                                                  ((CPathRef*)this)->m_typeTag);

    *(CFloat*)out = GridToWorld(trace->m_radius);
    return out;
}

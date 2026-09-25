//============================================================================
//  0x6F01FCB0 - SPathField::Set.
//
//  A four-instruction forwarder: hand the string to the counted-string
//  field at +0x00 and answer `this`.  The counted string's own assign is
//  0x6F4C5CF0, which Jass/jassnatives.h already declares as
//  STStringField::Assign; this is the same field type at the same offset,
//  reached from the cursor's texture path (gameuicursorstate.cpp).
//
//  Own translation unit: that assign is a real call.
//============================================================================
#include "gameuithunks.h"
#include "jassnatives.h"        // STStringField::Assign, 0x6F4C5CF0

SPathField* SPathField::Set(const char* path)
{
    ((STStringField*)this)->Assign(path);

    return this;
}

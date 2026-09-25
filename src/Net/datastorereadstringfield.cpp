//============================================================================
//  0x6F309A00 - read a NUL-terminated string out of a CDataStore straight
//  into a counted string field.
//
//  The 0x800-byte scratch buffer is the shipped one, and so is the empty-
//  string check: a string that came back empty leaves the field alone
//  rather than assigning "".
//============================================================================
#include "cdatastore.h"
#include "jassnatives.h"

CDataStore* __fastcall DataStoreReadStringField(CDataStore* store,
                                                STStringField* field)
{
    char text[0x800];

    store->ReadString(text, sizeof(text));

    if (text[0] != 0)
        field->Assign(text);

    return store;
}

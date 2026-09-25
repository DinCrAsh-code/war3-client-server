//============================================================================
//  0x6F534A40 - CDataStore::GetVersion: the save-stream format version.
//
//  A plain field read at +0x238 of whatever the load path is actually
//  handed - an object rather larger than the 0x18 bytes cdatastore.h names,
//  and the only member of it this codebase reaches.  Declared through a
//  local overlay rather than by padding CDataStore out with 0x220 bytes of
//  reserved block, per CLAUDE.md's "do not invent fields the call tree does
//  not read": nothing here knows what is between 0x18 and 0x238.
//
//  Own translation unit: its two callers (LoadVersionedObject,
//  versionedload.cpp, and CItem::Load, item_load.cpp) reach it with real
//  calls, four of them in total, and two instructions is well inside what
//  /Ob2 would inline given the chance.
//============================================================================
#include "cdatastore.h"

struct SVersionedStream
{
    char m_reserved00[0x238];
    int  m_version;     // +0x238
};

int CDataStore::GetVersion()
{
    return ((SVersionedStream*)this)->m_version;
}

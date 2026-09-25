//============================================================================
//  SHandleTriple - the three-dword record the item load path reads as a
//  unit (0x6F02F110, item_loadhandletriple.cpp) and the element type of
//  Position's own +0x9C array (position.h), whose four entries the
//  constructor initialises to (-1, -1, 0) apiece.
//
//  Its own header rather than a local declaration in each file, because two
//  translation units pass it across: a `void*` on one side and an
//  `SHandleTriple*` on the other are two different mangled names for one
//  function - the LNK2019 class docs/notes/verifier-gate-link-failures.md
//  describes.
//============================================================================
#ifndef HANDLETRIPLE_H
#define HANDLETRIPLE_H

struct SHandleTriple
{
    unsigned int m_handle;      // +0x00 - -1 = unset
    unsigned int m_typeTag;     // +0x04 - -1 = unset
    unsigned int m_extra;       // +0x08 - 0
};

class CDataStore;

//  0x6F02F110 - item_loadhandletriple.cpp.
CDataStore* __fastcall ReadHandleTriple(CDataStore* store, SHandleTriple* out);

#endif

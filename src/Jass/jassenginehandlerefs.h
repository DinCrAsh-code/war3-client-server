//============================================================================
//  See jassenginehandlerefs.cpp - 0x6F459660, JassEngineHandleOwner::
//  AddHandleRef.  Declared here (rather than only in that .cpp) so
//  jassnativecalldispatch.cpp's own DispatchWithArg can call it under the
//  same C++ name instead of a second mangled symbol for the same address.
//============================================================================
#ifndef JASSENGINEHANDLEREFS_H
#define JASSENGINEHANDLEREFS_H

//  JassHandleTableHeader (count at +4, entries at +8, each entry 0x10
//  bytes with its own refcount at +0xC) is declared once, in
//  jassinstance.h - JassInstance's own +0x2874 is this same table
//  (ReleaseCompiledFunc reads the identical shape) - and reused here so a
//  translation unit that includes both headers together does not see two
//  conflicting definitions of the same name.
#include "jassinstance.h"

struct JassEngineHandleOwner
{
    char                    m_reserved0[0x2874];
    JassHandleTableHeader*  m_handleTable;   // +0x2874

    //  0x6F459660 - declared, not defined, here: out-of-line in
    //  jassenginehandlerefs.cpp so every caller (including
    //  jassnativecalldispatch.cpp's own DispatchWithArg) emits a real call
    //  rather than /Ob2 inlining a fresh copy of this short a body at each
    //  site - a real difference the shipped DispatchWithArg's own dump
    //  shows (`call JassEngineHandleOwner::AddHandleRef`), not a cosmetic
    //  one.
    void AddHandleRef(int id);
};

#endif

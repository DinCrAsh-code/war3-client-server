//============================================================================
//  The two vtable symbols .\RCString.cpp's constructors stamp, and the one
//  string constant its heap copies tag themselves with.  See cstringrep.h
//  for why these are local definitions rather than funcmap.DATA rows.
//============================================================================
#include "cstringrep.h"

void* g_vftableCStringManager[1] = { 0 };
void* g_vftableCStringRepTable[1] = { 0 };

extern "C" const char aEDrive1TempBui[] =
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\Hash.h";

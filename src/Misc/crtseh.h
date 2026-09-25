//============================================================================
//  The two CRT array-iteration helpers src/Misc/crtseh4.cpp reconstructs,
//  declared for the call sites that reach them.  MSVC generates these calls
//  itself under /EHsc; this build's fixed /EHs-c- never does, so a
//  reconstruction that has to reproduce one calls it by name.
//============================================================================
#ifndef CRTSEH_H
#define CRTSEH_H

void __stdcall EhVectorConstructorIterator(
    void* array, unsigned int elementSize, int count,
    void (__thiscall* ctor)(void*), void (__thiscall* dtor)(void*));

void __stdcall EhVectorDestructorIterator(
    void* array, unsigned int elementSize, unsigned int count,
    void (__thiscall* dtor)(void*));

#endif

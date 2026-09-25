//============================================================================
//  Thiscall adjuster thunks reaching SOptionalHandleRefResolver::
//  ResolveChained (handlereref_resolve.cpp) on `this` biased by a fixed
//  amount - the same resolve reused against several embedded instances at
//  different offsets in different (not otherwise identified) owners.
//============================================================================
#include "itemhandleresolve.h"
//  Re-declared (not redefined) to match handlereref_resolve.cpp's own
//  class exactly, so the compiler mangles this declaration's name the same
//  way and `jmp ...::ResolveChained` resolves to the real target symbol
//  instead of a hand-written (and easy to get subtly wrong) decorated name.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

__declspec(naked) void Adjuster_6F286010()
{
    __asm
    {
        add ecx, 20h
        jmp SOptionalHandleRefResolver::ResolveChained
    }
}

__declspec(naked) void Adjuster_6F286070()
{
    __asm
    {
        add ecx, 38h
        jmp SOptionalHandleRefResolver::ResolveChained
    }
}

__declspec(naked) void Adjuster_6F416D30()
{
    __asm
    {
        add ecx, 2Ch
        jmp SOptionalHandleRefResolver::ResolveChained
    }
}

__declspec(naked) void Adjuster_6F416E70()
{
    __asm
    {
        add ecx, 20h
        jmp SOptionalHandleRefResolver::ResolveChained
    }
}

//  Three more of the same shape, found by the batch-08 sweep: two more
//  offsets into SOptionalHandleRefResolver::ResolveChained and one into
//  HandleRefFieldOwner_6F473170::QueryField78.
__declspec(naked) void Adjuster_6F286050()
{
    __asm
    {
        add ecx, 2Ch
        jmp SOptionalHandleRefResolver::ResolveChained
    }
}

__declspec(naked) void Adjuster_6F416EB0()
{
    __asm
    {
        add ecx, 44h
        jmp SOptionalHandleRefResolver::ResolveChained
    }
}

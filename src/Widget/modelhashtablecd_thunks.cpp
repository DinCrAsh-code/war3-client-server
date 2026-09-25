//============================================================================
//  0x6F4E9B70 - a TSHashTable<T,HASHKEY_STRI>-shaped NewNode reached from
//  CWar3Image::SetModelByName/SetModelByHandle/CreateSpriteFromDescriptor's
//  closure.  Not a reconstruction: it dispatches through its table's
//  AllocNode virtual, whose real body is below this dump's depth cut - the
//  same reason Widget/stringref_thunks.cpp's own NewNode stays a redirect.
//  `retn 0Ch`.  The concrete owning table is not established by this call
//  tree.
//============================================================================

struct SModelHashTableC
{
    void* NewNode(unsigned int hashval, void* arg1, void* arg2);
};

__declspec(naked) void* SModelHashTableC::NewNode(unsigned int, void*, void*)
{
    __asm { mov eax, 06F4E9B70h }
    __asm { jmp eax }
}

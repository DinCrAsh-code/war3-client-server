//============================================================================
//  0x6F2DC040 - `retn 0`.  Publishes `((void**)obj)[0]` into a single global
//  cache slot.  Nothing in this closure reaches the global's other readers or
//  the object's own type, so both stay untyped rather than guessed at.
//============================================================================

extern "C" void* g_unk6FAB4E70;

//  0x6F2DC040 (__fastcall, obj arrives in ecx).
void __fastcall CacheFirstFieldGlobal(void** obj)
{
    g_unk6FAB4E70 = obj[0];
}

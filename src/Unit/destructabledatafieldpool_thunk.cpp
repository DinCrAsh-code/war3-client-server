//============================================================================
//  0x6F71C770 - get-or-create the lazily-created global field-registration
//  pool CDestructableDatabase::SetRecordSet (Unit/destructabledatabase.h)
//  reads once and reuses for every field.  The same sibling-pool-helper
//  cluster AcquireUpgradeDataFieldPool (Game/upgradedatafieldpool_thunk.cpp)
//  documents - see that file's own header note.  No arguments; the pool
//  object itself comes back in `eax`.
//============================================================================

void* __cdecl AcquireDestructableDataFieldPool();

__declspec(naked) void* __cdecl AcquireDestructableDataFieldPool()
{
    __asm
    {
        mov eax, 06F71C770h
        jmp eax
    }
}

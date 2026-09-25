//============================================================================
//  0x6F71C9B0 - get-or-create the lazily-created global field-registration
//  pool CUpgradeDatabase::SetRecordSet (Game/upgradedatabase.h) reads once
//  and reuses for every field.  One of CCustomData::Construct's own "six
//  sibling lazy-singleton-pool helpers" (Item/abilitycustomdata.h's own
//  header note - 0x6F71C6E0/70/800/890/920/9B0), each its own SEH-carrying
//  module entirely unrelated to the checksum-provider registry this batch
//  is about.  No arguments; the pool object itself comes back in `eax`.
//============================================================================

void* __cdecl AcquireUpgradeDataFieldPool();

__declspec(naked) void* __cdecl AcquireUpgradeDataFieldPool()
{
    __asm
    {
        mov eax, 06F71C9B0h
        jmp eax
    }
}

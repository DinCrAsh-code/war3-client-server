//============================================================================
//  NTempest::CSyncData's own three out-of-line members.  See syncdata.h for
//  the class and docs/notes/checksum-provider-registry.md for the whole
//  mechanism this is one leg of.
//============================================================================
#include "syncdata.h"
#include "textsink.h"

unsigned int __fastcall CSyncData_GetChecksum(CSyncData* self)
{
    return self->m_checksum;
}

void __fastcall CSyncData_Reset(CSyncData* self)
{
    self->m_checksum = 0;
    self->m_count = 0;
}

void __fastcall CSyncData_DebugDumpTo(CSyncData* self, void* /*unused_edx*/,
                                       void* sink, unsigned int /*unused_arg*/)
{
    //  "" - the fmt argument is `push offset ValueName` (0x6F87529C, the
    //  same shared all-zero-bytes global
    //  NTempest::CPresenceTagged::DumpDebugState's own header already reads
    //  this way (Agent/presencetagged_dumpdebugstate.cpp).
    LogFormatted(sink, "");
    LogFormatted(sink, "[CSyncData] (%s)", self->m_name);

    //  Read fresh off the vtable rather than calling CSyncData_GetChecksum
    //  directly - the shipped body really does a virtual dispatch here
    //  (`mov edx,[esi] ; mov eax,[edx+4] ; call eax`), not a direct call to
    //  its own slot 1's known address, matching CLAUDE.md's own raw
    //  vtable-slot-dispatch idiom.
    typedef unsigned int(__fastcall * GetChecksumFn)(CSyncData*);
    GetChecksumFn* vtable = (GetChecksumFn*)self->m_vtable;
    unsigned int checksum = vtable[1](self);

    LogFormatted(sink, "\tcsum[%08X], count[%u]", checksum, self->m_count);
}

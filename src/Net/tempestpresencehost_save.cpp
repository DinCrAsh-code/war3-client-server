//============================================================================
//  0x6F485530 / 0x6F485650 - two `Save`-shaped methods against the tempest
//  presence host (`g_unk6FAB778C`), each writing a fixed sequence of
//  length-prefixed sub-records through a `CDataStoreScratch` writer: write
//  a 0 placeholder dword (`WriteDword(0)`), call the real content writer,
//  then back-patch the placeholder with the number of bytes the content
//  writer actually appended (`WriteDwordAt(placeholderPos, writer->m_field10
//  - posAfterPlaceholder)`) - the same length-prefixed-block idiom
//  Agent/agent_save.cpp and Player/selectionwar3_save.cpp already use for a
//  record *count*, applied here to a raw byte length instead.
//
//  Investigated as part of "does any of the 8 unreconstructed
//  g_unk6FAB778C-touching functions perform the missing
//  STwoIndexedFreeLists::AllocateAt call": no - both only ever *read*
//  already-live fields off the host (and its own `+0x20` negative pool) and
//  hand them to a writer; neither stores anything into the host at all.
//
//  0x6F485530 writes three blocks - two calls to `sub_6F484A60`/`sub_6F484DB0`
//  against the host itself and its negative pool, then two calls to
//  `sub_6F484E50` that additionally pass the host's own `+0x40`/`+0x44`
//  scalar fields (`STwoIndexedFreeLists::m_positiveHead`/`m_negativeHead`,
//  Math/indexedfreelist.h - the free-list head indices) as a third
//  argument.  0x6F485650 writes six blocks against a *different* writer
//  method set, one of which (`sub_6F49B0E0`) is called on the writer object
//  itself rather than through `this`/the host - see its own comment below.
//
//  Both are called from one wrapper, `sub_6F46DB30` (not reconstructed this
//  session - "SaveAgileSubsystem" by inference from its own two calls),
//  which this session did not trace further; nothing establishes whether
//  that wrapper runs on ordinary save-game, replay-write, or some other
//  trigger.
//
//  Every one of the individual field writers below (`sub_6F484A60` etc.) is
//  a genuinely new leaf this session did not reconstruct - each stays a
//  naked redirect, since none is one of this session's eight addresses.
//============================================================================
#include "tempestpresencehost.h"
#include "cdatastorescratch.h"

//  0x6F484A60 - write one field of the host itself into `writer`.  `retn 8`.
ADDR_THUNK(void __fastcall WriteHostFieldA(void* host, CDataStoreScratch* writer, void* extra), 0x6F484A60)
//  0x6F484DB0 - write one field of whichever pool object (`host` or
//  `host+0x20`) is handed to it.  `retn 8`.
ADDR_THUNK(void __fastcall WritePoolFieldA(void* host, CDataStoreScratch* writer, void* pool), 0x6F484DB0)
//  0x6F484E50 - write one field of a pool object *and* an extra scalar (the
//  pool's own free-list head index).  `retn 0Ch`.
ADDR_THUNK(void __fastcall WritePoolHeadField(void* host, CDataStoreScratch* writer, void* pool,
                                              int headValue), 0x6F484E50)

//  0x6F481280 - write one field of `this` (the Agile subsystem manager, not
//  the host) into `writer`.  `retn 4`.
ADDR_THUNK(void __fastcall WriteAgileField(void* self, CDataStoreScratch* writer), 0x6F481280)
//  0x6F49B0E0 - a method the writer calls *on itself*, no extra argument -
//  `mov ecx, esi` (esi = the writer) right before this call, not `this`.
//  `retn` (no stack args).
ADDR_THUNK(void __fastcall WriterOwnMarker(CDataStoreScratch* writer), 0x6F49B0E0)
//  0x6F484E90 / 0x6F484FB0 - two more "write one field of the given pool
//  object" writers, distinct from `WritePoolFieldA` above (a different
//  field each, same call shape).  Both `retn 8`.
ADDR_THUNK(void __fastcall WritePoolFieldB(void* self, CDataStoreScratch* writer, void* pool), 0x6F484E90)
ADDR_THUNK(void __fastcall WritePoolFieldC(void* self, CDataStoreScratch* writer, void* pool), 0x6F484FB0)

//  This build's toolchain (VS2005) has no lambdas, so each length-prefixed
//  block below is spelled out by hand rather than through one generic
//  helper: `pos0 = writer->m_field10`, `writer->WriteDword(0)`, `pos1 =
//  writer->m_field10`, run the real content writer, then
//  `writer->WriteDwordAt(pos0, writer->m_field10 - pos1)`.

//  0x6F485530.  `self` (the Agile subsystem manager) is unused beyond being
//  handed through to `WriteHostFieldA`/`WritePoolHeadField` unchanged.
void SaveTempestPresenceHostFieldsA(void* self, CDataStoreScratch* writer)
{
    void* host = g_unk6FAB778C;
    void* negativePool = (char*)host + 0x20;
    unsigned int pos0, pos1;

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WriteHostFieldA(self, writer, host);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolFieldA(self, writer, host);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolFieldA(self, writer, negativePool);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolHeadField(self, writer, host, *(int*)((char*)host + 0x40));
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolHeadField(self, writer, negativePool, *(int*)((char*)host + 0x44));
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);
}

//  0x6F485650.
void SaveTempestPresenceHostFieldsB(void* self, CDataStoreScratch* writer)
{
    void* host = g_unk6FAB778C;
    void* negativePool = (char*)host + 0x20;
    unsigned int pos0, pos1;

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WriteAgileField(self, writer);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WriterOwnMarker(writer);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolFieldB(self, writer, host);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolFieldB(self, writer, negativePool);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolFieldC(self, writer, host);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);

    pos0 = writer->m_field10;
    writer->WriteDword(0);
    pos1 = writer->m_field10;
    WritePoolFieldC(self, writer, negativePool);
    writer->WriteDwordAt(pos0, writer->m_field10 - pos1);
}

//============================================================================
//  0x6F485790 / 0x6F488F60 - the `Load`/verify counterparts of
//  tempestpresencehost_save.cpp's own `SaveTempestPresenceHostFieldsB`/
//  `SaveTempestPresenceHostFieldsA`: each reads a length-prefixed dword off
//  a `CDataStore` reader (`CDataStore::ReadDword`), calls the matching
//  content reader, and fails (returns 0/false) unless the number of bytes
//  the content reader actually consumed
//  (`reader->m_readPos - posAfterLengthDword`) equals the declared length -
//  the read-side mirror of the write-side's own back-patched length prefix.
//
//  Investigated as part of "does any of the 8 unreconstructed
//  g_unk6FAB778C-touching functions perform the missing
//  STwoIndexedFreeLists::AllocateAt call": no.  0x6F488F60 does WRITE
//  directly into the host (`g_unk6FAB778C->m_field40`/`m_field44`, the same
//  `STwoIndexedFreeLists::m_positiveHead`/`m_negativeHead` free-list head
//  indices `SaveTempestPresenceHostFieldsA` reads out), but that is a
//  scalar field restore, not a call to `AllocateAt`/`Push` and not an
//  entry stored into the free-list's own record array.
//
//  0x6F485790's own six blocks line up exactly with
//  `SaveTempestPresenceHostFieldsB`'s own six writes (`WriteAgileField` ->
//  `ReadAgileField`, `WriterOwnMarker` -> `ReaderOwnMarker`,
//  `WritePoolFieldB`/`C` -> `ReadPoolFieldB`/`C`), with one extra step this
//  session's Save side has no counterpart for: between the second
//  `ReadPoolFieldB` call (the negative pool) and the first `ReadPoolFieldC`
//  call, it resets both pools via `sub_6F485040` (no length check around
//  either call - a plain, unconditional reset, presumably clearing the
//  free-list bookkeeping before the following two blocks repopulate it).
//
//  0x6F488F60 does NOT line up its own middle two blocks 1:1 with
//  `SaveTempestPresenceHostFieldsA`'s corresponding writers by argument
//  count (`sub_6F483080` takes only `(this, reader)`, no host/pool
//  argument at all, where `WritePoolFieldA` takes `(this, writer, pool)`) -
//  noted rather than forced into a false symmetry; the two functions most
//  likely each resolve `g_unk6FAB778C` internally instead of taking it as a
//  parameter.  Its own prelude (before any of the four blocks) walks a
//  singly-linked list at `this+0x38C` (unrelated to the presence host)
//  calling each node's own `sub_6F48E4C0`, most likely flushing a pending-
//  request queue before the load proper begins.
//============================================================================
#include "tempestpresencehost.h"
#include "cdatastore.h"

//  0x6F481340 - read one field of `this` (the Agile subsystem manager, not
//  the host) off `reader`.  `retn 4`.
ADDR_THUNK(void __fastcall ReadAgileField(void* self, CDataStore* reader), 0x6F481340)
//  0x6F49B1E0 - the reader's own counterpart to `WriterOwnMarker` - called
//  on `reader` itself, no extra argument.  `retn`.
ADDR_THUNK(void __fastcall ReaderOwnMarker(CDataStore* reader), 0x6F49B1E0)
//  0x6F484F20 - read one field of the given pool object (`host` or
//  `host+0x20`).  `retn 8`.
ADDR_THUNK(void __fastcall ReadPoolFieldB(void* self, CDataStore* reader, void* pool), 0x6F484F20)
//  0x6F485040 - reset one pool object's own bookkeeping, no length prefix
//  around it on the shipped side either.  `retn 4`.
ADDR_THUNK(void __fastcall ResetPool(void* self, void* pool), 0x6F485040)
//  0x6F485080 - read one more field of the given pool object, called after
//  the reset above.  `retn 8`.
ADDR_THUNK(void __fastcall ReadPoolFieldC(void* self, CDataStore* reader, void* pool), 0x6F485080)

//  0x6F48E4C0 - visited once per node of `this->m_field38C`'s own linked
//  list (the node's own `+8` field is `next`), `this`-call, no stack
//  argument.  Not characterised beyond that shape.
ADDR_THUNK(void __fastcall FlushPendingListNode(void* node), 0x6F48E4C0)
//  0x6F484C00 - read one field of the host into `reader`.  `retn 8`.
ADDR_THUNK(void __fastcall ReadHostFieldA(void* self, CDataStore* reader, void* host), 0x6F484C00)
//  0x6F483080 - read one more field, taking only `(this, reader)` - see
//  this file's own header note on why it does not mirror
//  `WritePoolFieldA`'s own argument shape.  `retn 4`.  Called twice on the
//  shipped side, back to back.
ADDR_THUNK(void __fastcall ReadUnpooledField(void* self, CDataStore* reader), 0x6F483080)
//  0x6F488E90 - read one scalar out-value (the free-list head index later
//  stored into `host->m_field40`/`m_field44`) for the given pool object.
//  `retn 0Ch`.
ADDR_THUNK(void __fastcall ReadPoolHeadField(void* self, CDataStore* reader, void* pool,
                                             int* outValue), 0x6F488E90)

//  0x6F485790.  Returns 1 on a fully-consistent read, 0 the moment any
//  block's declared length disagrees with what its own content reader
//  consumed - the shipped code abandons the rest of the sequence at the
//  first mismatch rather than reading every block regardless.
int VerifyTempestPresenceHostFieldsB(void* self, CDataStore* reader)
{
    void* host = g_unk6FAB778C;
    void* negativePool = (char*)host + 0x20;
    unsigned int declaredLength;
    unsigned int posBefore;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadAgileField(self, reader);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReaderOwnMarker(reader);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadPoolFieldB(self, reader, host);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadPoolFieldB(self, reader, negativePool);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    ResetPool(self, host);
    ResetPool(self, negativePool);

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadPoolFieldC(self, reader, host);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadPoolFieldC(self, reader, negativePool);
    return (reader->m_readPos - posBefore) == declaredLength;
}

//  0x6F488F60.  `self` doubles as both the Agile subsystem manager (the
//  `+0x38C` pending-list prelude) and the "this" every field reader below
//  is called against, exactly like the save side's `self`.
int LoadTempestPresenceHostFieldsA(void* self, CDataStore* reader)
{
    for (void* node = *(void**)((char*)self + 0x38C); node; )
    {
        void* next = *(void**)((char*)node + 8);
        FlushPendingListNode(node);
        node = next;
    }

    void* host = g_unk6FAB778C;
    void* negativePool = (char*)host + 0x20;
    unsigned int declaredLength;
    unsigned int posBefore;
    int headValue;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadHostFieldA(self, reader, host);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadUnpooledField(self, reader);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    ReadUnpooledField(self, reader);
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    headValue = 0;
    ReadPoolHeadField(self, reader, host, &headValue);
    *(int*)((char*)host + 0x40) = headValue;
    if (reader->m_readPos - posBefore != declaredLength)
        return 0;

    declaredLength = 0;
    reader->ReadDword(&declaredLength);
    posBefore = reader->m_readPos;
    headValue = 0;
    ReadPoolHeadField(self, reader, negativePool, &headValue);
    *(int*)((char*)host + 0x44) = headValue;
    return (reader->m_readPos - posBefore) == declaredLength;
}

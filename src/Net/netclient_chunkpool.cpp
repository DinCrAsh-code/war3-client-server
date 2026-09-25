//============================================================================
//  0x6F67C540 - Net::NetClient's own receive-chunk acquire helper
//  (agent-networking-wave4-2026-08-29). One field of the per-connection
//  record `AppendReceivedData` (netclient_recvappend.cpp) scans for is a
//  `CDataRecycler`-shaped sub-object at +0x1E8 (same vtable, same RTTI
//  stamp - see cdatarecycler.h/cdatarecycler_slots.cpp), and this is the
//  method that object's own class does not itself declare: it is called
//  with `ecx` pointed at that sub-object directly (`lea ecx,[edi+1E8h]`
//  at the one real call site), not through the vtable, so it reads as a
//  free function taking a `CDataRecycler*` rather than a genuine member -
//  modelled that way here to keep it out of cdatarecycler.h, which only
//  carries the class's own committed 5-slot vtable and the members that
//  dispatch through it.
//============================================================================
#include "cdatarecycler.h"
#include "storm.h"

typedef void* (__thiscall* CDataRecyclerReallocSlot)(void*, void*, unsigned int, unsigned int*, const char*, int);

void* __fastcall AcquireChunk(CDataRecycler* pool)
{
    void* ptr = 0;
    unsigned int size = 0;
    pool->TryPopChunk(&ptr, &size, 0, 0);

    void* chunk;
    if (size >= 0x418)
    {
        chunk = ptr;
    }
    else
    {
        CDataRecyclerReallocSlot reallocSlot = (CDataRecyclerReallocSlot)(*(void***)pool)[3];
        chunk = reallocSlot(pool, ptr, 0x418, &size, 0, 0);
    }

    if (chunk)
    {
        *(unsigned int*)chunk = 0;
        *((unsigned int*)chunk + 1) = 0;
    }
    return chunk;
}

//============================================================================
//  0x6F67D630 - AcquireChunk's release-side twin. `this` (the pool) is not
//  touched anywhere in this body - the shipped code relies on the caller's
//  own ecx flowing straight through into ReleaseChunk unmodified, exactly
//  the shape an ordinary thiscall member with a single stack parameter
//  compiles to, so it is written as a real member here rather than a naked
//  passthrough. `node`'s own element type is unrecoverable from this call
//  tree (TSLink<T>::Unlink() never touches T - same reasoning
//  netprovider_freepool_helpers.cpp's own UnlinkAndFreeNode already gives),
//  poison-tagged by this instantiation's own target address the same way.
//============================================================================
struct UnknownChunkNode_6F67D630;
typedef TSLink<UnknownChunkNode_6F67D630> ChunkNodeLink;

//  Truncated the same way aE_1/aEDrive1TempBui25 already are - IDA's own
//  listing comment names it "type descriptor name" but does not spell it
//  out, and the exact bytes make no difference to the score (both sides
//  canonicalise the operand to SYM).
static const char aAuclientturnNe[] = "?AU...";

void CDataRecycler::ReleaseNode(void* node)
{
    ((ChunkNodeLink*)node)->Unlink();
    ReleaseChunk(node, 0x418, aAuclientturnNe, -2);
}

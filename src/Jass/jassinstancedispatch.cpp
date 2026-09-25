//============================================================================
//  0x6F45D720 - JASS VM/compiler core internals (jassvm-core-A), "compiler/
//  AST dispatcher internals" per docs/targets/JassVM.md.  `retn 0`, `this`
//  in ecx (JassInstance*).
//
//  Walks a tagged dword stream through JASSSCOPEREF's own (previously
//  "unread") +0x00 cursor - `*m_field2858` reinterpreted as a
//  JASSSCOPEREF* rather than a raw array base, which is consistent with
//  jassinstance.h's own existing GetGlobalSlot (both read `*m_field2858`;
//  this closure just resolves what the pointed-to object *is*).  Each
//  dword's own top byte is a one-based tag selecting a ten-way jump table:
//
//    1        - terminate the stream and return.
//    3        - declare a ctor/alias (JASSTYPESCOPE::DeclareCtorOrAlias,
//                against the flag byte captured once before the loop from
//                the very first record's own +2), then fall into a nested
//                per-field loop reading (kind byte, value dword) pairs
//                until a kind-4 terminator - kind 0x0C is a "next field"
//                no-op, kind 0x28 stores the current stream position into
//                JassInstance's own +0x2870 array (a
//                TSGrowableArray<SJassPakElem*>*-shaped field this
//                closure's own growth math shares its two callees with -
//                growing it in place to cover `value` when the array is
//                not already that large), any other kind loops for the
//                next field.
//    6, 7      - skip one dword (unread field), no-op otherwise.
//    9         - resolve a declared name (JASSSCOPEREF::GetDeclaredName)
//                and cache it, without resetting the cached-name slot the
//                way a fresh top-of-record entry does.
//    10        - resolve a second declared name and pair it with the one
//                tag 9 cached as a child/parent type declaration
//                (JASSTYPESCOPE::DeclareChildType), then reset.
//    2, 4, 5, 8, anything out of 1..10 - no-op, read the next tag.
//============================================================================
#include "jassinstance.h"
#include "jassthreadstate.h"
#include "cdatastore.h"
#include "storm.h"

//----------------------------------------------------------------------------
//  0x6F45E450 - `retn 4`.  Deserializes a discarded dword and a record
//  count off `store`, then reads that many script-data records - each one
//  its own freshly spawned child instance (CreateChildInstance,
//  0x6F45E300 - jassinstancebigthunks.cpp) fed straight into
//  LoadScriptDataRecord (0x6F45E140, same file).
//----------------------------------------------------------------------------
void JassInstance::LoadScriptDataRecords(CDataStore* store)
{
    unsigned int discard = 0;
    store->ReadDword(&discard);

    unsigned int count = 0;
    store->ReadDword(&count);

    for (unsigned int i = 0; i < count; i++)
    {
        JassInstance* child = CreateChildInstance();
        child->LoadScriptDataRecord(store);
    }
}

//  0x6F44B2C0 - jassdeclare.cpp's own JASSSCOPEREF, redeclared here just
//  for the mangled call signature (its own field layout is that file's).
struct JASSSCOPEREF
{
    const char* GetDeclaredName(unsigned int index);
};

//  0x6F45D010 / 0x6F45D300 - jasstypedecl.cpp's own JASSTYPESCOPE,
//  likewise redeclared for the call signature only.  Its own +0x286C
//  neighbourhood is this same JassInstance object (jasstypedecl.cpp's own
//  "almost certainly JassThreadLocal" guess narrows to JassInstance here -
//  this closure reaches its +0x2858/+0x2870, both documented JassInstance
//  fields, from the identical `this`).
struct JASSTYPESCOPE
{
    void DeclareChildType(const char* childName, const char* parentName);
    void DeclareCtorOrAlias(const char* name, void* value, void* unused);
};

struct SJassPakElem;

void JassInstance::ExecuteDeclStream()
{
    JASSSCOPEREF* scope = (JASSSCOPEREF*)m_field2858;
    unsigned int* stream = *(unsigned int**)m_field2858;
    unsigned char ctorFlag = ((unsigned char*)stream)[2];

    unsigned int pendingName = 0;

top:
    pendingName = 0;

nextTag:
    for (;;)
    {
        unsigned int tagWord = *stream;
        unsigned int tag = (tagWord >> 24) - 1;
        stream += 1;

        if (tag > 9)
            continue;

        switch (tag)
        {
        case 0:   // tag == 1: terminate
            return;

        case 2:   // tag == 3: declare ctor/alias, then walk its field list
        {
            unsigned int index = *stream;
            const char* name = scope->GetDeclaredName(index);
            stream += 1;

            ((JASSTYPESCOPE*)this)->DeclareCtorOrAlias(
                name, stream, (void*)(size_t)ctorFlag);

            for (;;)
            {
                unsigned char kind = ((unsigned char*)stream)[3];
                unsigned int value = stream[1];
                stream += 2;

                if (kind == 0x0C)
                    continue;

                if (kind == 4)
                    goto nextTag;

                if (kind == 0x28)
                {
                    TSGrowableArray<SJassPakElem*>* arr =
                        *(TSGrowableArray<SJassPakElem*>**)((char*)this + 0x2870);

                    if (value >= arr->m_count)
                    {
                        unsigned int newCount = value + 1;
                        if (newCount > arr->m_alloc)
                        {
                            unsigned int chunk = arr->m_chunk;
                            if (chunk == 0)
                                chunk = arr->ComputeChunk(newCount);

                            unsigned int alloc = newCount;
                            unsigned int over = newCount % chunk;
                            if (over != 0)
                                alloc = newCount + (chunk - over);

                            arr->SetAlloc(alloc);
                        }

                        arr->m_count = newCount;
                    }

                    ((void**)arr->m_data)[value] = (void*)stream;
                }
            }
        }

        case 5:   // tag == 6
        case 6:   // tag == 7
            stream += 1;
            continue;

        case 8:   // tag == 9: cache a resolved name
        {
            unsigned int index = *stream;
            pendingName = (unsigned int)scope->GetDeclaredName(index);
            stream += 1;
            continue;
        }

        case 9:   // tag == 10: pair with the cached name, then reset
        {
            unsigned int index = *stream;
            const char* childName = scope->GetDeclaredName(index);
            stream += 1;

            ((JASSTYPESCOPE*)this)->DeclareChildType(
                childName, (const char*)pendingName);
            goto top;
        }

        default:   // tags 2, 4, 5, 8
            continue;
        }
    }
}

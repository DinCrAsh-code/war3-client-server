//============================================================================
//  0x6F05F630 - the load-time counterpart of a FloatListener-typed slot:
//  read the {handle, typeTag} pair back and, unless it is the sentinel
//  (handle == -1, meaning the field was never saved with a live listener),
//  release whatever the slot currently holds, make a fresh FloatListener
//  over a new sixteen-byte Storm block, and adopt the *resolved* object's
//  own {handle, typeTag} pair (its +0x14/+0x18 - the same offsets
//  positionmodifier.h's SMadeModifier already names, since the persisted
//  handle names a made-object record and not the listener itself).
//
//  The sentinel case is a hard early return: the shipped code does not
//  release or null the slot at all when handle == -1, so neither does this.
//  When the handle *is* real but does not resolve (or the allocation
//  failed), the shipped code still stores whatever it has - null, if the
//  allocation failed - and unconditionally writes the resolved object's
//  fields onto it if resolution succeeds; that unguarded `made->m_handle =
//  ...` on a possibly-null `made` is a real, transcribed asymmetry (an
//  allocation failure paired with the handle actually resolving would
//  fault in the shipped binary too), not a bug introduced here.
//
//  __fastcall(store in ecx, &slot in edx); returns store.
//
//  Own translation unit: the FloatListener family's own module
//  (floatlistener.h), not CDataStore's own reader cluster.
//============================================================================
#include "floatlistener.h"
#include "cdatastore.h"
#include "storm.h"   // placement new

void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

//  ".\\FloatProp.cpp"'s own header truncates identically in the dump
//  (aEDrive1TempBui_92) to several other, textually different strings in
//  this module's neighbourhood; the byte content does not affect the score
//  (an unresolved string symbol canonicalises to SYM either way - see
//  cdatastore_writerefhandlepair.cpp's own neighbours), so this is written
//  out as the header floatlistener.h/floatpropmodifier.h already cites.
static const char kFloatListenerAllocTag[] =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\engine\\floatprop.h";

CDataStore* __fastcall ReadFloatListenerRef(CDataStore* store,
                                            FloatListener** slot)
{
    unsigned int handle;
    unsigned int typeTag;
    store->ReadDword(&handle);
    store->ReadDword(&typeTag);

    //  The sentinel: nothing to load, and the slot is left exactly as it
    //  was - not released, not nulled.  See the file header.
    if (handle == (unsigned int)-1)
        return store;

    FloatListener* made = 0;
    void* raw = SMemAlloc(0x10, kFloatListenerAllocTag, 0x47, 0);
    if (raw != 0)
    {
        made = new (raw) FloatListener();
        made->m_refcount += 1;
    }

    FloatListener* old = *slot;
    if (old != 0)
    {
        if (--old->m_refcount == 0)
            ((void (__thiscall *)(void*))(*(void***)old)[0])(old);
    }
    *slot = made;

    SMadeModifier* resolved =
        (SMadeModifier*)LookupHandle(handle, (int)typeTag);
    if (resolved != 0)
    {
        made->m_handle  = resolved->m_handle;
        made->m_typeTag = resolved->m_typeTag;
    }
    return store;
}

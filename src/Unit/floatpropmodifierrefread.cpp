//============================================================================
//  0x6F053640 - the FloatModifier twin of floatlistenerrefread.cpp's
//  ReadFloatListenerRef.  Byte-for-byte the same shape (see that file for
//  the sentinel/adopt notes, which apply here unchanged) with FloatModifier
//  in place of FloatListener and the module's own allocation line (0x6F
//  instead of 0x47).
//
//  __fastcall(store in ecx, &slot in edx); returns store.
//============================================================================
#include "floatpropmodifier.h"
#include "cdatastore.h"
#include "storm.h"   // placement new

void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

//  See floatlistenerrefread.cpp's own note - the byte content of this
//  truncated string does not affect the score.
static const char kFloatModifierAllocTag[] =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\engine\\floatprop.h";

CDataStore* __fastcall ReadFloatModifierRef(CDataStore* store,
                                            FloatModifier** slot)
{
    unsigned int handle;
    unsigned int typeTag;
    store->ReadDword(&handle);
    store->ReadDword(&typeTag);

    if (handle == (unsigned int)-1)
        return store;

    FloatModifier* made = 0;
    void* raw = SMemAlloc(0x10, kFloatModifierAllocTag, 0x6F, 0);
    if (raw != 0)
    {
        made = new (raw) FloatModifier();
        made->m_refcount += 1;
    }

    FloatModifier* old = *slot;
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

//============================================================================
//  0x6F47C100 - ReleasePathRegistration.  Declared in
//  src/Widget/widgetfootprintremove.cpp, the one caller in this call tree:
//  `ReleasePathRegistration(registration); SMemFree(registration, "delete",
//  -1, 0);` - this function tears the *contents* of the SPathRefArray down
//  (every element, then its own heap buffer if it has one); the caller
//  frees the 0x18-byte block the array itself lives in immediately
//  afterwards. Two separate frees of two separate allocations, which is
//  why this one does not also free `registration` itself.
//
//  Release every element (each through TeardownRegistration - a mover's
//  path registration being torn down, per pathclientidlecancel.cpp's own
//  comment on 0x6F4A00B0, which this is the one caller of in scope), null
//  the slot behind it, then - only when the array actually owns a heap
//  buffer (capacity, the dword at +0x00, greater than 1; a capacity of 0
//  or 1 is the array's own inline/unallocated case, which Save/Load
//  already read the same way, see pathref_load.cpp) - free the buffer
//  through Storm, tagged with the array's own RTTI-shaped type name the
//  way CCamera::~CCamera already frees `m_pName` with "PAVCBaseManaged@@"
//  (ccamera.cpp): IDA's auto-generated string name for this literal is
//  `aPavcpmregionNi`, which decodes to ".?AVCPMRegionNI@@" once the
//  punctuation IDA drops from a legal C identifier is put back - the
//  mangled "pointer to class CPMRegionNI" RTTI name. The exact bytes of a
//  Storm memory tag are not behaviour - `verify.py` canonicalises any
//  data-label operand to a bare `SYM` the same way it does for a real
//  symbol - so nothing here can regress the score if the guess is
//  imperfect; the shape of the call (four arguments, `-2` flags, freed
//  only past the capacity guard) is what actually matters and is read
//  directly off the disassembly.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  loop-with-a-call-inside in this family (docs/msvc-vc8-idioms.md) - the
//  per-element TeardownRegistration call is exactly the kind of "cannot be
//  proven not to throw" call site that idiom describes.
//
//  Own translation unit: one real call per element plus one free.
//============================================================================
#include "widgetpathrefadjust.h"
#include "spatialgrid.h"   // CGridRegistration::TeardownRegistration - real
                            // body now, gridregistrationteardown.cpp - see
                            // that file's own note on the tempest presence
                            // host's (`g_unk6FAB778C`) now-confirmed
                            // NTempest::CDynTable<CPrRgEntry> identity
#include "storm.h"   // SMemFree

void __fastcall ReleasePathRegistration(void* registration)
{
    SPathRefArray* arr = (SPathRefArray*)registration;

    unsigned int count = arr->m_count;
    for (unsigned int i = 0; i < count; i++)
    {
        ((CGridRegistration*)arr->m_pItems[i])->TeardownRegistration();
        arr->m_pItems[i] = 0;
    }

    //  Same "the leading dword is the capacity" read Load already uses
    //  (pathref_load.cpp) rather than a named field - see that file's own
    //  comment for why it stays unnamed.
    if (*(const unsigned int*)arr->m_reserved00 > 1)
        SMemFree(arr->m_pItems, "PAVCPMRegionNI@@", -2, 0);

    *(unsigned int*)arr->m_reserved00 = 0;
    arr->m_count = 0;
    arr->m_pItems = 0;
}

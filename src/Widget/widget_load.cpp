//============================================================================
//  0x6F2AC810 - CWidget's vtable slot 15 (+0x3C).  See widget.h.
//
//  CWidget::Save's counterpart (widget_save.cpp), field for field, with two
//  asymmetries worth recording because they are real behaviour and not
//  reconstruction noise:
//
//   * the flag Save writes for +0x38 is read back and **thrown away** - the
//     shipped code zeroes a byte slot, reads into it and never looks at it
//     again, so a loaded widget's +0x38 keeps whatever the constructor's -1
//     put there;
//   * the pathing registration is *rebuilt*, not restored in place: on a
//     set flag the loader allocates a fresh 0x18-byte SPathRefArray through
//     Storm (tagged `.\\CWidget.cpp` line 0x77A - the same filename the
//     earlier CWidget survey read out of two other allocation tags),
//     placement-constructs it against this widget and only then reads into
//     it.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  placement-new-into-a-Storm-allocation in this family
//  (docs/msvc-vc8-idioms.md): the constructor call the compiler cannot
//  prove will not throw is enough on its own.
//
//  Own translation unit: nine real calls.
//============================================================================
#include "widget.h"
#include "widgetpathrefadjust.h"
#include "cdatastore.h"
#include "storm.h"   // placement new

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

//  0x6F47C350 - SPathRefArray's own constructor.  Out of this session's
//  scope; redirected.  Five stack arguments past `this`.
//
//  The parameter list has to be spelled exactly as widget_addfootprint.cpp
//  spells it, because that file re-declares this struct to reach the same
//  constructor and MSVC mangles every parameter type into the name: a local
//  `int b` here against its `const int* masks` there is two symbols for one
//  function and the DLL does not link (docs/notes/verifier-gate-link-
//  failures.md, cause 2).  `const int*` is the true type - the footprint
//  path passes a real three- or four-entry mask array through it; this call
//  site passes none, which is the `push ebx` (0) the dump shows at
//  0x6F2AC8B3 and reads here as a null pointer.
struct SPathRefArrayCtor
{
    void Construct(CWidget* owner, void* pos, int maskCount, const int* masks,
                   int flags);
};

__declspec(naked) void SPathRefArrayCtor::Construct(CWidget*, void*, int,
                                                   const int*, int)
{
    __asm
    {
        mov     eax, 06F47C350h
        jmp     eax
    }
}

static const char kWidgetCpp[] = ".\\CWidget.cpp";

void CWidget::Load(CDataStore* store)
{
    CWar3Image::Load(store);

    unsigned int footprint;
    store->ReadDword(&footprint);
    m_footprintType = footprint;

    store->ReadWord(&m_playerMaskA);
    store->ReadWord(&m_playerMaskB);

    unsigned char hasPathRefs = 0;
    store->ReadByte(&hasPathRefs);
    if (hasPathRefs != 0)
    {
        void* raw = SMemAlloc(0x18, kWidgetCpp, 0x77A, 0);
        SPathRefArray* refs = 0;
        if (raw != 0)
        {
            int scratch = 0;
            ((SPathRefArrayCtor*)raw)->Construct(this, &scratch, 0, 0, 1);
            refs = (SPathRefArray*)raw;
        }
        m_pathGridRegistration = refs;
        refs->Load(store);
    }

    //  Read and discarded - see the file header.
    unsigned char unusedFlag38 = 0;
    store->ReadByte(&unusedFlag38);

    unsigned char flag3C = 0;
    store->ReadByte(&flag3C);
    m_reserved3C = flag3C;

    ReadCFloat(store, (CFloat*)&m_reserved40);
}

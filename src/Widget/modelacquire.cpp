//============================================================================
//  0x6F501AA0 - AcquireModelHandle: the model layer's one entry point for
//  "give me a handle on the model this specification names".
//
//  A null specification is not an error - it means the default one, whose
//  address is a *pointer* global (off_6FA87E30) the shipped code substitutes
//  before the lookup rather than after it.
//
//  Two null checks and no diagnostic on either: an unknown specification and
//  a record with no model object both answer zero quietly.  `xor eax, eax`
//  is hoisted above both of them in the shipped stream, which is what makes
//  the two failures share one exit.
//
//  __stdcall and not __fastcall: the shipped body reads neither ecx nor edx,
//  and neither does the record lookup it forwards to.  Its one call site
//  happens to leave values in both, and they are dead there.
//
//  Own translation unit: two real calls out of it, both into other modules.
//============================================================================

//  0x6F501930 / 0x6F4FF9C0 - Widget/modelinstance_thunks.cpp.
void* __stdcall  FindModelRecord(const void* spec);
void* __fastcall AcquireModelObjectHandle(void* model);

//  What a model record carries, at the one depth this function reaches.
struct SModelRecord
{
    char  m_reserved00[0x18];
    void* m_pModel;         // +0x18
};

//: off_6FA87E30 - the default model specification.  A pointer global and not
//: an object: the shipped code loads its *address* as the substitute value,
//: which is what makes the declaration below an array of one rather than a
//: pointer.
extern const void* g_pDefaultModelSpec;

void* __stdcall AcquireModelHandle(const void* spec)
{
    if (spec == 0)
        spec = &g_pDefaultModelSpec;

    //  Two early returns and not one shared `handle`: the shipped stream
    //  hoists `xor eax, eax` above both null tests and shares one epilogue,
    //  which neither spelling reproduces here - but the single-variable form
    //  was measured and costs four instructions more (generated stream 20
    //  against 16), because MSVC materialises the zero twice.
    SModelRecord* record = (SModelRecord*)FindModelRecord(spec);
    if (record == 0)
        return 0;

    void* model = record->m_pModel;
    if (model == 0)
        return 0;

    return AcquireModelObjectHandle(model);
}

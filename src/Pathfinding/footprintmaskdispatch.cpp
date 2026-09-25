//============================================================================
//  0x6F47FA90 - SelectFootprintMaskHandler.
//
//  Reached from PathGridSubmitRegion (0x6F4693D0, Widget/
//  widgetfootprintregion.cpp - itself below this dump's depth cut and
//  thunked), which hands it a count and a small record; dispatches to one
//  of four handlers depending on the count and, when the count is 0 or 1,
//  on the record's own first two dwords.
//
//  __stdcall(unsigned int count, void* record) - both arguments arrive on
//  the stack and the function cleans them itself (`retn 8`), which is not
//  this build's default __fastcall and has to be spelled out.
//
//  Every one of the four handlers is a real call, none of them is a
//  worklist entry of this batch, and all four share one shape confirmed
//  from their own shipped tails: __stdcall(count, record, 0, 0), retn 10h.
//  Left as naked thunks to the real, unhooked functions.
//
//  Own translation unit: four real calls, chosen but never taken all four
//  together.
//============================================================================

//  0x6F47DB20 / 0x6F47E2B0 / 0x6F47E650 / 0x6F47DF30 - see above.
void __stdcall FootprintMaskHandlerGeneral(unsigned int count, void* record,
                                           int, int);
void __stdcall FootprintMaskHandlerSingleA(unsigned int count, void* record,
                                           int, int);
void __stdcall FootprintMaskHandlerSingleB(unsigned int count, void* record,
                                           int, int);
void __stdcall FootprintMaskHandlerEmpty(unsigned int count, void* record,
                                         int, int);

#define ADDR_THUNK4(name, addr)                                    \
    __declspec(naked) void __stdcall name(unsigned int, void*, int, int) \
    {                                                              \
        __asm { mov eax, addr }                                    \
        __asm { jmp eax }                                          \
    }

ADDR_THUNK4(FootprintMaskHandlerGeneral, 0x6F47DB20)
ADDR_THUNK4(FootprintMaskHandlerSingleA, 0x6F47E2B0)
ADDR_THUNK4(FootprintMaskHandlerSingleB, 0x6F47E650)
ADDR_THUNK4(FootprintMaskHandlerEmpty,   0x6F47DF30)

#undef ADDR_THUNK4

void __stdcall SelectFootprintMaskHandler(unsigned int count, void* record)
{
    if (count > 1)
    {
        FootprintMaskHandlerGeneral(count, record, 0, 0);
        return;
    }

    int first = *(int*)record;
    if (first == 0 || first == -1)
    {
        FootprintMaskHandlerEmpty(count, record, 0, 0);
        return;
    }

    if (*(int*)((char*)record + 4) != 0)
        FootprintMaskHandlerSingleA(count, record, 0, 0);
    else
        FootprintMaskHandlerSingleB(count, record, 0, 0);
}

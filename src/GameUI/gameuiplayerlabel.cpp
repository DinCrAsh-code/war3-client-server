//============================================================================
//  0x6F40AFF0 - SPlayerSlot::EnsureDisplayName.
//
//  Hand back the slot's cached display name, building it first when the
//  caller asks for it and the cache is empty: the localised "PLAYER" game
//  string, then the slot's own one-based number formatted into a second
//  buffer and appended with Storm's bounded append.
//
//  The two buffers are real locals (0x20 and 0x10 bytes), which is what puts
//  a /GS cookie in the shipped prologue.  This repo builds at a fixed /GS-,
//  so the cookie load, the `xor esp` and the check call are the only
//  instructions of this function that cannot come back.
//
//  Own translation unit: five real calls out of it, and it is its own
//  shipped module.
//============================================================================
#include "playercolor.h"        // SPlayerSlot
#include "gameuithunks.h"       // GetGameString, Storm_503
#include "handleobject.h"       // GetHandleOrZero - 0x6F4C4630

//  0x6F4C5CF0 - spelled exactly as Jass/jassnatives.h has it, so both sides
//  reach the one symbol rather than minting a second name for one address.
struct STStringField { void Assign(const char* value); };

//  Storm ordinal 578 - SStrPrintf.  Variadic and __cdecl, which is what the
//  `add esp, 10h` after the call says; the other Storm string entry points
//  in this repo are __stdcall because none of them is.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  unk_6F93E510, the format the slot number is appended with.  Read out of
//  the binary rather than guessed: `ida_query bytes 0x6F93E510 32` shows
//  `20 25 64 00`, i.e. " %d".  A literal and not a DATA name, because it is
//  a shared string constant (the neighbouring bytes are "UPKEEP_HIGH%d" and
//  "%s %s%d/%d%s") and naming one of those can only break matches elsewhere.
static const char kSlotNumberFormat[] = " %d";

const char* SPlayerSlot::EnsureDisplayName(int build)
{
    STStringField* field = (STStringField*)m_displayName;
    const char* cached =
        (const char*)GetHandleOrZero((const SHandleHolder*)field);

    if (build != 0 && (cached == 0 || *cached == 0))
    {
        char name[0x20];
        GetGameString("PLAYER", name, sizeof(name));

        char number[0x10];
        Storm_578(number, sizeof(number), kSlotNumberFormat,
                  (int)m_slotNumber + 1);

        Storm_503(name, number, sizeof(name));

        field->Assign(name);
        cached = (const char*)GetHandleOrZero((const SHandleHolder*)field);
    }

    return cached;
}

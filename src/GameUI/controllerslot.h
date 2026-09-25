//============================================================================
//  The numbered value slots every one of the 0x6F4E7xxx-0x6F4EBxxx handle
//  objects carries - the camera controller, the camera event sink, the fog
//  object and the light object all share this layer, and every one of the
//  slot entry points below is reached on all four.
//
//  A slot is one animatable value: a vftable whose slots 1-3 are the update
//  and apply hooks, a flag byte that says which of them is live, and the
//  three floats the value itself is read out of.  Only what this call tree
//  touches is named.
//============================================================================
#ifndef CONTROLLERSLOT_H
#define CONTROLLERSLOT_H

struct SControllerSlot
{
    void*         m_vtable;             // +0x00
    char          m_reserved04[0x09];   // +0x04
    unsigned char m_flags;              // +0x0D - bit 2 wants an update
                                        //         first, bit 1 picks which
    char          m_reserved0E[0x02];   // +0x0E
    int           m_field10;            // +0x10
    int           m_field14;            // +0x14
    float         m_field18;            // +0x18
    float         m_value[3];           // +0x1C, +0x20, +0x24
};

struct SControllerSlotHost
{
    char              m_reserved00[0x10];
    SControllerSlot** m_slots;          // +0x10
};

//  0x6F4E80D0 - GameUI/controllerslotread.cpp.  **Not hookable**: the
//  shipped body takes its host in eax and its destination in edi, which no
//  MSVC calling convention produces (tools/unhookable.py carries the entry).
void __fastcall ReadControllerSlotValue(SControllerSlotHost* host, int slot,
                                        float* out);
//  0x6F4E8260 - GameUI/controllerslotquery.cpp.
void __fastcall QuerySlotVec3(void* host, int slot, float* out);
//  0x6F4E82C0 - GameUI/controllerslotreset.cpp.
void __fastcall ResetControllerSlot(void* host, int slot, void* arg);
//  0x6F4E8370 - GameUI/controllerslotsetint.cpp.
void __fastcall SetControllerSlotInt(void* host, int slot, int value);
//  0x6F4E8380 - GameUI/controllerslotsetfloat.cpp.
void __fastcall SetControllerSlotFloat(void* host, int slot, float value);
//  0x6F4E82D0 - GameUI/controllerslotblend.cpp.
void __fastcall MergeControllerSlotVec3(void* host, int slot,
                                        const float* value, unsigned int mask);

#endif

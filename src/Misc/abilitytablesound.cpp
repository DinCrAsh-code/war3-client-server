//============================================================================
//  0x6F32F7B0 - look up an ability-table sound cue and forward it, together
//  with an optional position, to the sound-cue filter (0x6F32F700).
//
//  Reached from SWidgetOffsetPrep::Prepare (0x6F2ADA80, below the depth
//  cut in Widget/widgetnegateforward.cpp) - a shipped caller our own
//  reconstruction of Prepare never runs through (Prepare itself is a naked
//  redirect to real code), but this address is still its own hookable key
//  and worklist entry.
//
//  __fastcall(unsigned int category /*ecx*/, unsigned int soundIndex /*edx*/,
//             const SVec3* pos /*stack*/), retn 4.
//
//  g_soundGateFlags (dword_6FA76030) is read once as a *byte*: bit 3 (0x08)
//  gates "master sounds" at all - clear it and every call here logs
//  "master sounds disabled, aborting" through the diagnostic sink at
//  off_6FA76044 and returns without touching the table.  Bit 6 (0x40) gates
//  the per-slot lookup: clear and the whole thing is a silent no-op (the
//  shared `add esp,8Ch` / `retn 4` tail with no logging).  Past both gates,
//  `category`/`soundIndex` are range-checked (< 0x18, < 6) before indexing
//  the table, so an out-of-range request also falls through as a silent
//  no-op rather than reading past dword_6FAB3C08.
//
//  `pos`, when non-null, is copied by value into a zero-initialised local
//  (SZeroInitStruct::ZeroFields, Misc/misc_leaves_batch3.cpp - the same
//  0x54-byte scratch record every other user of it zeroes) with a tag of 2
//  at +0x0 and the three floats at +0xC/+0x10/+0x14; when null the tag stays
//  0 and the floats stay zero.  The final `neg`/`sbb`/`and` against the
//  local's own address is the non-branching form of
//  "pass &local if pos != 0, else NULL" - the callee (0x6F32F700) tells the
//  two cases apart by the pointer it is handed, not by the tag alone.
//
//  0x6F32F700 is a callee, not a worklist entry of this batch; it is left as
//  a naked thunk to the real, unhooked function rather than guessed at here.
//============================================================================
#include "abilitytablesound.h"

//  0x6F252980 - Misc/misc_trivial_getters.cpp.
int __fastcall LookupAbilityTableEntry(int row, int col);

//  0x6F266720 - Misc/misc_leaves_batch3.cpp.
struct SZeroInitStruct { void __thiscall ZeroFields(); };

//  dword_6FA76030 - a byte of gate flags; only bits 3 and 6 are read here.
extern unsigned char g_soundGateFlags;

//  off_6FA76044 - a pointer to a diagnostic-sink object: [obj][+0xC] is a
//  cdecl `void (*)(void* self, int level, const char* msg)` vtable slot.
struct SDiagnosticSink;
extern SDiagnosticSink* g_pSoundDiagnosticSink;
typedef void (__cdecl *DiagLogFn)(void* self, int level, const char* msg);

void __fastcall PlayAbilityTableSound(int category, int soundIndex,
                                      const SVec3* pos)
{
    if ((g_soundGateFlags & 8) == 0)
    {
        void* sink = g_pSoundDiagnosticSink;
        DiagLogFn log = (DiagLogFn)(*(void***)sink)[3];
        log(sink, 0, "master sounds disabled, aborting");
        return;
    }

    if ((g_soundGateFlags & 0x40) == 0)
        return;
    if (category >= 0x18)
        return;
    if (soundIndex >= 6)
        return;

    int entry = LookupAbilityTableEntry(soundIndex, category);

    //  0x8C-byte local - the entire `sub esp, 8Ch` frame is this one
    //  record; only SZeroInitStruct::ZeroFields' own fields and the tag are
    //  ever written, but the frame is not trimmed to just those.
    struct
    {
        int  m_tag;
        char m_reserved04[8];
        int  m_x, m_y, m_z;         // floats, moved as raw dwords - the
        char m_reserved18[0x8C - 0x18]; // shipped copy never touches the FPU.
    } local;

    ((SZeroInitStruct*)&local)->ZeroFields();

    local.m_tag = 0;
    if (pos != 0)
    {
        const int* src = (const int*)pos;
        local.m_tag = 2;
        local.m_x = src[0];
        local.m_y = src[1];
        local.m_z = src[2];
    }

    FilterAbilitySoundCue(entry, pos != 0 ? (void*)&local : 0, 0, 0, 0, 0);
}

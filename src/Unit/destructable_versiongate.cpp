//============================================================================
//  0x6F266D80 / 0x6F266620 - the version-gated float Load's own +0x5C read
//  goes through (destructable_save.cpp).
//
//  Unlike item_load.cpp's own per-site ENSURE_GATE (which hands the
//  pointer straight back to the caller to fill in field by field), this
//  pair is a matched set: 0x6F266D80 owns the one static SVersionGate,
//  configures all three of its fields itself and returns it configured;
//  0x6F266620 is the generic "is the stream's version inside this gate's
//  [since, until] window, and if so read the float `target` points at -
//  otherwise, if the window has already closed, consume and discard the
//  same four bytes so the reader stays aligned" reader that shape drives.
//  A second, unrelated instantiation of `SVersionGate`'s own idea
//  (versiongate.h) - not a generic version-gated *object* loader the way
//  LoadVersionedObject is (item_load.cpp), because this one only ever
//  reads a float.
//============================================================================
#include "destructable.h"
#include "cdatastore.h"
#include "versiongate.h"
#include "storm.h"   // placement new

//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

static const char kGateHeader[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\Engine\\SaveVersion.h";

static SVersionGate* g_float5CGate = 0;   // dword_6FAB661C

//----------------------------------------------------------------------------
//  0x6F266D80.  Ensure the one static gate exists, then (re)configure all
//  three of its fields through the global itself rather than a cached
//  local - the shipped code re-reads dword_6FAB661C before every one of
//  the three stores, which is what writing through the global directly
//  gives and a local copy of the pointer does not (the same reasoning
//  item_load.cpp's own ENSURE_GATE macro documents for its four gates).
//----------------------------------------------------------------------------
SVersionGate* __fastcall ConfigureFloat5CGate(int since, int until, void* target)
{
    if (g_float5CGate == 0)
    {
        void* raw = SMemAlloc(12, kGateHeader, 0x9D, 0);
        if (raw != 0)
            g_float5CGate = new (raw) SVersionGate();
        else
            g_float5CGate = 0;
    }

    g_float5CGate->m_since = since;
    g_float5CGate->m_until = until;
    g_float5CGate->m_target = target;
    return g_float5CGate;
}

//----------------------------------------------------------------------------
//  0x6F266620.  Read the stream's version twice (once per bound, matching
//  the shipped code's own two separate calls rather than one cached
//  local), and:
//
//    * if it is within [since, until] and the gate names a target, read
//      the float there and stop;
//    * otherwise, if it is not yet past `until`, there is nothing to read
//      at all (the field does not exist yet in this stream) - stop
//      without touching the stream;
//    * otherwise (the field's window has already closed, or it was inside
//      the window but the gate names no target) consume the four bytes
//      into a scratch float and discard them, so the reader stays aligned
//      for whatever comes next.
//
//  Written with the shipped code's own two labels rather than refactored
//  into independent booleans, because the "gate names no target" case
//  rejoins the same "not yet past until" label the low-version case uses -
//  a nested if/else loses that shared exit.
//----------------------------------------------------------------------------
CDataStore* __fastcall LoadFieldWithVersionGate(CDataStore* store, SVersionGate* gate)
{
    int afterSince = (store->GetVersion() >= gate->m_since);
    int beforeUntil = (store->GetVersion() <= gate->m_until);

    if (afterSince)
    {
        if (!beforeUntil)
        {
            float scratch;
            store->ReadFloat(&scratch);
            return store;
        }

        float* target = (float*)gate->m_target;
        if (target != 0)
        {
            store->ReadFloat(target);
            return store;
        }
    }

    if (beforeUntil)
        return store;

    {
        float scratch;
        store->ReadFloat(&scratch);
    }
    return store;
}

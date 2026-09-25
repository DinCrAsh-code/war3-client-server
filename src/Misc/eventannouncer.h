#pragma once

//============================================================================
//  CEventAnnouncer - a small embedded "announce something happened" broadcast
//  sub-object this binary embeds at a fixed offset inside several unrelated
//  owner classes (SJassTimer at +0x24, an Agent-family class in
//  agenttypetag.cpp at +0x44 - see each caller's own note on why its offset
//  differs). Named/declared here (not in an anonymous namespace) precisely
//  so every owner can share the one real PostEvent definition instead of
//  each redeclaring its own thunk to the same address.
//============================================================================
struct CEventAnnouncer
{
    typedef void (__thiscall *QueryOverrideFn)(CEventAnnouncer*, int*);

    void QueryOverride(int* out)
    {
        QueryOverrideFn fn = (QueryOverrideFn)(*(void***)this)[0x18 / 4];
        fn(this, out);
    }

    //  0x6F477A80 - the deep __except_handler4-shaped broadcast
    //  src/Jass/jasstimerstartelapsed.cpp's own SEH-frame family
    //  (checkeditemslot.cpp, jassresolvetriggerhandle.cpp's own sibling
    //  constructors) already documents as unreproducible at this build's
    //  fixed /GS- /EHs-c-. `retn 14h` - five stack arguments read off the
    //  compiler's own listing. Declared returning `void*` (not `void`)
    //  because src/Agent/agenttypetag.cpp's own forwarder passes this call's
    //  own `eax` straight through as its own return value with no
    //  intervening store - SJassTimer::Start (the other caller) just
    //  discards it. Thunked rather than guessed at.
    void* PostEvent(const int* subject, unsigned int eventId, void* owner,
                    int extra, int zero);
};

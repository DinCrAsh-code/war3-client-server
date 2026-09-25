//============================================================================
//  0x6F005BB0 - CPreferencesWar3::ApplyStoredPreference: read one
//  preference back out of the store and push it at whoever owns it.
//
//  The kind word in the defaults array picks the path: kind 0 is an integer
//  and goes through GetIntValue into ApplyPreference (0x6F0028C0), kind 1 is
//  a string and goes through GetStringValue into 0x6F002F00, and anything
//  else is refused.  `sub eax,0` / `jz` / `sub eax,1` / `jnz` is what MSVC
//  makes of that two-case switch (docs/msvc-vc8-idioms.md).
//
//  The string buffer really is 0x1000 bytes on the stack, which is what
//  makes the shipped prologue call __alloca_probe; this build emits the
//  same call (Misc/chkstk.cpp supplies the body, since /Zl means no CRT is
//  on the link line).  What it cannot emit is the /GS buffer cookie the
//  shipped frame carries around that array - the load of dword_6FAAE140,
//  the `xor esp`, the check call and the four-byte displacement the cookie
//  slot puts on every [esp+N] below it - because this repo builds at a
//  fixed /GS-.  SysMessagePrintf records the identical loss.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "preferences.h"

//  0x6F002F00 - the string arm's applier.  Misc/sweep_predicates.cpp got to
//  this address first from another call tree and named it
//  SAlwaysFalseOwner::Reject - its body really is a two-argument constant
//  `false` - and one shipped address carries one C++ name, so it is
//  re-declared here rather than given a second.
struct SAlwaysFalseOwner { int __thiscall Reject(void* a, void* b); };

//  SPreferenceKind / g_preferenceKinds - preferences.h.

int CPreferencesWar3::ApplyStoredPreference(int key)
{
    if (key >= 0x4F)
        return 0;

    //  A switch and not two ifs: the shipped code's `sub eax,0` / `jz` /
    //  `sub eax,1` / `jnz` is MSVC's two-case switch, and it is what puts
    //  the string arm in the fallthrough position with the integer arm
    //  branched to below it.
    switch (g_preferenceKinds[key].m_kind)
    {
    case 0:
        {
            int value = 0;
            if (GetIntValue(key, &value))
                return ApplyPreference(key, value);
        }
        break;

    case 1:
        {
            char text[0x1000];
            if (GetStringValue(key, text, sizeof(text)))
                return ((SAlwaysFalseOwner*)this)->Reject((void*)key, text);
        }
        break;
    }

    return 0;
}

//============================================================================
//  0x6F4550C0 / 0x6F455110 - the JASS native-registration primitive.  Every
//  one of the 1167 `RegisterNative(name, entry, "(sig)R")` calls in the
//  table at 0x6F3D4020 (docs/targets/jass-natives-registration-table.md)
//  ends up here: resolve-or-create the native's own NativeFunc record in
//  the running thread's JassThreadLocal, then parse the JASS signature
//  string to fill in its argument count and return-type code.
//============================================================================
#include "jassregisternative.h"
#include "jassthreadstate.h"
#include "gamecontext.h"    // GetThreadLocalSlot - 0x6F4C34D0
#include "storm.h"

//  Storm ordinal 578 - SStrPrintf, variadic and __cdecl
//  (GameUI/gameuiplayerlabel.cpp's own note has the reasoning).  Declared
//  here rather than pulled from a header neither of those two owns.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//----------------------------------------------------------------------------
//  0x6F454E30 - the real body is jassvm-batch-2/3's JASSVARSCOPE::Declare
//  (src/Jass/jassdeclare.cpp): one hash-table "resolve-or-create the node
//  for `name`, then stamp its two trailing fields" primitive that the
//  shipped build reuses for both a JassVarTable node (name/type/slot) and
//  this thread's own NativeFunc table (name/outHandle -> funcAddr/sig
//  land at the identical +0x1C/+0x24 offsets a JassNativeSlot uses -
//  JassThreadLocal's own m_natives sits at +0x18, jasssymtables.h's own
//  JASSVARSCOPE also reads its hash table from +0x18).  One shipped
//  address, two call-site names picked independently by two batches before
//  either had seen the other's dump; JASSVARSCOPE::Declare is the name that
//  landed in funcmap.py, so this call site is fixed up to call it directly
//  instead of keeping its own now-duplicate thunk.
//
//  Forward-declared locally rather than pulled from jassdeclare.cpp's own
//  header, because nothing else needs the JASSVARSCOPE type - only its
//  mangled symbol, which needs the exact same parameter types to bind
//  (link_check.py's own concern: a `void**` here instead of
//  `JassNativeSlot**` would mangle to a different, unbound symbol).
//----------------------------------------------------------------------------
struct JassNativeSlot;
struct JASSVARSCOPE
{
    void Declare(const char* name, JassNativeSlot** outResult, void* type,
                 unsigned int slot);
};

//----------------------------------------------------------------------------
//  0x6F4550C0 - `retn 8`.  ecx = JASS name, edx = &outHandle; two more
//  stack arguments (funcAddr, sig) are just forwarded to 0x6F454E30
//  (JASSVARSCOPE::Declare, see above) against the running thread's own
//  JassThreadLocal - reinterpreted as a JASSVARSCOPE because the callee
//  only ever touches it at +0x18, which is JassThreadLocal::m_natives.
//----------------------------------------------------------------------------
void __fastcall ResolveNativeFuncRecord(const char* name, void** outHandle,
                                         void* funcAddr, const char* sig)
{
    JassThreadLocal* jass = (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);
    ((JASSVARSCOPE*)jass)->Declare(name, (JassNativeSlot**)outHandle, funcAddr,
                                    (unsigned int)sig);
}

//----------------------------------------------------------------------------
//  0x6F455110 - `retn 4`.  The `/GS` stack cookie the shipped body opens
//  with (`mov eax, dword_6FAAE140 / xor eax, esp`) is not reproducible
//  under this build's fixed `/GS-` (docs/msvc-vc8-idioms.md).
//
//  The scan from `sig+1` to the matching `)` counts one stack word per
//  parameter, skipping over an `H<typename>;` handle tag's own name
//  without counting its characters; the character just past `)` is the
//  JASS return-type letter, dispatched through a byte-lookup/jump-table
//  pair the shipped listing shows spanning the 21 characters 'B'..'V' -
//  seven of them real (V=0, C=3, I=4, R=5, S=6, H=7, B=8), the rest and
//  any signature that runs out before its own `)` falling through to
//  Storm's own "function '%s' has bad signature" diagnostic.
//----------------------------------------------------------------------------
void __fastcall RegisterNative(void* funcAddr, const char* name, const char* sig)
{
    void* handle = 0;
    ResolveNativeFuncRecord(name, &handle, funcAddr, sig);

    NativeFunc* nf = (NativeFunc*)handle;
    nf->m_argCount = 0;

    const char* p = sig + 1;
    char c = *p;
    if (c != ')')
    {
        for (;;)
        {
            if (c == 0)
            {
                char buf[0x104];
                Storm_578(buf, 0x104, "function '%s' has bad signature", name);
                return;
            }

            if (c != 'V')
                nf->m_argCount += 1;

            c = *p;
            if (c == 'H')
            {
                while (c != ';')
                {
                    c = p[1];
                    p += 1;
                }
            }

            c = p[1];
            p += 1;
            if (c == ')')
                break;
        }
    }

    switch ((unsigned char)p[1])
    {
    case 'V': nf->m_returnType = 0; break;
    case 'C': nf->m_returnType = 3; break;
    case 'I': nf->m_returnType = 4; break;
    case 'R': nf->m_returnType = 5; break;
    case 'S': nf->m_returnType = 6; break;
    case 'H': nf->m_returnType = 7; break;
    case 'B': nf->m_returnType = 8; break;
    default:
    {
        char buf[0x104];
        Storm_578(buf, 0x104, "function '%s' has bad signature", name);
        break;
    }
    }
}

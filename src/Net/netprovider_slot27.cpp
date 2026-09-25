//============================================================================
//  Net::NetProvider - vtable slot 27, 0x6F65D580.  Tell vtable+0x90 "code
//  2", copy the second stack argument into a 16-byte local buffer, hand a
//  handful of scratch out-params through sub_6F65CD80 (0x6F65CD80, six
//  stack arguments, `retn 18h`), then forward the result plus the first
//  and third stack arguments through the inner +0x2B4 interface's own
//  vtable+0xC slot.
//
//  sub_6F65CD80 walks the same +0x60C-rooted node list slot 6's own
//  ResetAndNotify (netprovider_slot6.cpp) and ClearPostedBits already read,
//  matching a node by either its own +8 key or (falling back) by an
//  SStrnICmp'd name at +0x4C, and copies four of that node's fields out to
//  the caller.  Not this session's own target and self-contained enough to
//  park; thunked to the real, unhooked body rather than reconstructed.
//============================================================================
#include <windows.h>

namespace Net
{

class NetProvider
{
public:
    //  0x6F65D580 - vtable slot 27.
    void ForwardWithLookup(int idOrKey, const char* name, int extra);
};

}  // namespace Net

void __stdcall Storm_501(char* dest, const char* src, unsigned int size);
int  __stdcall SStrnICmp(const char* a, const char* b, unsigned int max);

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  Out-param shapes sub_6F65CD80 (below) actually writes on a match - NOT
//  the single `int`s the caller (ForwardWithLookup) used to declare for
//  its own `localCC`/`localBC` locals before this session traced the
//  callee's own body: the fifth out-param is a 4-dword (16-byte) struct
//  copied from the matched node's own +0x28..+0x34, and the sixth is a
//  46-dword (184-byte, 0x2E dwords - `rep movsd` with `ecx=0x2E`) raw
//  block copied from the node's own +0x4C. The previous, unreconstructed
//  thunk left both as single `int`s, which is a real stack-buffer
//  overflow the moment this path is actually hit (a plain crash, not
//  observable in `verify.py`, which never runs the code) - fixed here
//  alongside the real body.
struct LookupNodeQuad { int d0, d4, d8, dC; };
struct LookupNodeBlock184 { int words[46]; };

//  0x6F65CD80 - walks the +0x60C-rooted node list slot 6's own
//  ResetAndNotify/ClearPostedBits (netprovider_slot6.cpp) already read,
//  under the +0x5F0 lock: matches a node by its own +8 key (when `key` is
//  non-zero) or, failing that, by an `SStrnICmp`'d name at the node's own
//  +0x4C (when `name` is non-null); on a match, copies the node's own
//  +0x38 into `*out8` (zeroed up front, so a caller sees 0 on no match),
//  +0x3C into `*outC` (left untouched by the callee on no match - the
//  shipped body never zeroes it, so neither does this one), the
//  +0x28..+0x34 quad into `outQuad`, and the +0x4C..+0x4C+0xB8 raw block
//  into `outBlock`. Returns the resolved key (the node's own +8 field on
//  a name match, `key` echoed back unchanged on a key match or on no
//  match at all). Six stack arguments, `this` in ecx, `retn 18h` -
//  un-thunked this session (batch-E-net-misc-1).
class NetProviderSlot27Self
{
public:
    int LookupNode(int key, const char* name, int* out8, int* outC,
                   LookupNodeQuad* outQuad, LookupNodeBlock184* outBlock);
};
int NetProviderSlot27Self::LookupNode(int key, const char* name, int* out8, int* outC,
                                       LookupNodeQuad* outQuad,
                                       LookupNodeBlock184* outBlock)
{
    *out8 = 0;

    EnterCritSec((CRITICAL_SECTION*)((char*)this + 0x5F0));

    int  count = *(int*)((char*)this + 0x610);
    int  result = key;
    char* sentinel = (char*)this + 0x60C;
    char* node = (count > 0) ? (char*)(intptr_t)count : (char*)0;

    if (node > (char*)0)
    {
        for (;;)
        {
            if (key != 0)
            {
                if (*(int*)(node + 8) == key)
                    goto found;
            }
            else if (name != 0)
            {
                if (SStrnICmp(node + 0x4C, name, 0x7FFFFFFF) == 0)
                {
                    result = *(int*)(node + 8);
                    goto found;
                }
            }

            if (node == 0)
                node = sentinel;
            node = *(char**)(node + 4);
            if (node <= (char*)0)
                break;
        }
        LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x5F0));
        return result;

    found:
        outQuad->d0 = *(int*)(node + 0x28);
        outQuad->d4 = *(int*)(node + 0x2C);
        outQuad->d8 = *(int*)(node + 0x30);
        outQuad->dC = *(int*)(node + 0x34);
        *out8 = *(int*)(node + 0x38);
        *outC = *(int*)(node + 0x3C);
        {
            const char* src = node + 0x4C;
            char* dst = (char*)outBlock;
            __asm { mov esi, src }
            __asm { mov edi, dst }
            __asm { mov ecx, 0x2E }
            __asm { rep movsd }
        }
    }

    LeaveCritSec((CRITICAL_SECTION*)((char*)this + 0x5F0));
    return result;
}

void Net::NetProvider::ForwardWithLookup(int idOrKey, const char* name, int extra)
{
    typedef void (__thiscall *Method0x90Fn)(void*, int, int, int);
    ((Method0x90Fn)(*(void***)this)[0x90 / 4])(this, 2, 0, 0);

    char nameBuf[16];
    int  terminator = 0;
    int  local8 = 0;
    int  localC;
    LookupNodeQuad      quad = { 0, 0, 0, 0 };
    LookupNodeBlock184  block;

    Storm_501(nameBuf, name, 0x10);
    (void)terminator;

    ((NetProviderSlot27Self*)this)->LookupNode(idOrKey, 0, &local8, &localC, &quad, &block);

    //  vtable+0xC of the inner +0x2B4 interface: thiscall on `inner`, with
    //  nine stack arguments - the caller's own `this` comes along as a
    //  plain data argument alongside it, not as a second receiver.
    unsigned short cachedWord = *(unsigned short*)((char*)this + 0x2C0);
    typedef void (__thiscall *InnerSlot0xCFn)(void*, void*, void*, int, int,
                                              void*, int, const char*,
                                              unsigned short, int);
    void* inner = *(void**)((char*)this + 0x2B4);
    InnerSlot0xCFn fn = (InnerSlot0xCFn)(*(void***)inner)[0x0C / 4];
    fn(inner, this, &quad, local8, localC, &block, extra, name,
       cachedWord, 0);
}

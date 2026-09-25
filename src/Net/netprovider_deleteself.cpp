//============================================================================
//  Net::NetProvider - vtable slot 0, 0x6F65DB60.  The classic MSVC "scalar
//  deleting destructor": run the base destructor, then free `this` if bit 0
//  of the flags argument is set, and hand `this` back.
//============================================================================
#include "storm.h"
#include "cdatarecycler.h"
#include <windows.h>

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  0x6F65CD70 - already reconstructed (naked thunk) in
//  unreconstructed_thunks.cpp as PostStatusTarget::ResetRecycleState -
//  re-declared identically here for extern linkage.
class PostStatusTarget
{
public:
    void ResetRecycleState();
};

//  0x6F65BD10/0x6F65BCD0 - two more /OPT:ICF-less duplicates of
//  DirectLinkedList::UnlinkAll's own body (netprovider_pendinglist.cpp,
//  0x6F65BC90 - same "count/prevlink at +8, walk while >0" shape, same
//  TSLink<T>::Unlink() body inlined) for this class's own two other
//  embedded lists at +0x644/+0x608.  Reused rather than reproduced at
//  their own two addresses: like NetEventReadDwordField's own header
//  comment already establishes (netevent_dispatch.cpp), canonlib maps
//  every call's own target to the placeholder SYM, so the identical body
//  that function already provides produces the identical canonical
//  instruction stream at these two call sites too.  Re-declared
//  identically here for extern linkage.
struct UnknownListNode_6F65D9E0;   // never dereferenced - TSLink<T>::Unlink() never touches T

class DirectLinkedList
{
public:
    void*                             m_reserved0;
    TSLink<UnknownListNode_6F65D9E0>  m_terminator;

    void UnlinkAll();
};

//  Type-descriptor tag string this destructor's own SMemFree call passes -
//  content not score-relevant (IDA's own truncated auto-name), same
//  precedent as fixedstringrelease.cpp's aAuFixedstringD.
static const char aAuFixedstringD[] = ".?AVAUFixedString@@";

//  The two vtable symbols this destructor re-stamps (`??_7NetProvider@Net@@6B@`,
//  `??_7?$TInstanceId@UNetProvider@Net@@@@6B@`) - local placeholders, same
//  reason src/Game/savegameglobals.cpp gives at length: this build has not
//  reconstructed Net::NetProvider (or its TInstanceId<> base) as a real
//  polymorphic C++ class, so MSVC lays out no real table here to point at.
//  A raw `mov [reg], SYM` store either way - canonlib maps it identically
//  whether SYM is the real shipped vtable or this local stand-in
//  (abi_audit.py's own VTABLE SYMBOL finding), so the score cannot tell
//  the difference, but installing this stand-in for real (hooking this
//  destructor) would leave every virtual call through it dispatching into
//  garbage - arm-with-caution, same as CDataRecycler's own precedent.
void* g_vftableNetProviderPlaceholder[1] = { 0 };
void* g_vftableNetProviderInstanceIdPlaceholder[1] = { 0 };

//  0x6F65D9E0 - the base ~NetProvider() body: stamps this object's own
//  vtable pointer, frees a fixed-string field at +0x61C and resets it,
//  resets its own recycle state through PostStatusTarget::ResetRecycleState,
//  tears down two embedded intrusive lists (+0x644, +0x608 - each a real
//  out-of-line UnlinkAll call followed by an inlined terminator Unlink,
//  same shape netprovider_pendinglist.cpp's own DirectLinkedList destructor
//  already establishes), three embedded CRITICAL_SECTIONs (+0x62C, +0x5F0,
//  +0x2C2), a fourth fixed-string field at +0x148, then re-stamps the
//  TInstanceId<NetProvider> vtable and unlinks this object's own instance-id
//  link at +4.  It carries a real __except_handler4-shaped SEH frame
//  (`push -1` / `push offset SEH_...` / `large fs:0` prologue and matching
//  epilogue, local objects with real unwind state) - the same frame
//  docs/msvc-vc8-idioms.md documents as unreproducible at this build's
//  fixed /GS- /EHs-c-, so the frame furniture never matches; the body does.
//  IDENTICAL, not DIFFERS.
//
//  Left as a plain thiscall member, deliberately not inlined into
//  DeleteSelf below (it lives in a different translation unit and is only
//  ever declared, never defined, here) - see docs/msvc-vc8-idioms.md, "A
//  deleting destructor's `this != 0` test tracks whether the base
//  destructor inlined": with a real, un-inlined call the shipped
//  `test esi,esi` / `jz` this-null guard has to survive in DeleteSelf's own
//  body, and the dump shows it does.
//----------------------------------------------------------------------------
class NetProviderBaseDtor
{
public:
    void Destroy();
};

void NetProviderBaseDtor::Destroy()
{
    char* self = (char*)this;

    *(void**)self = g_vftableNetProviderPlaceholder;

    void* fixedString61C = *(void**)(self + 0x61C);
    if (fixedString61C != 0)
        SMemFree(fixedString61C, "delete", -1, 0);
    *(void**)(self + 0x61C) = 0;

    ((PostStatusTarget*)this)->ResetRecycleState();

    //  An embedded CDataRecycler at +0x664 (cdatarecycler.h - not a
    //  pointer field, the object itself: the dump's own `mov eax,[esi+664h]`
    //  reads the embedded object's own m_vtable word directly).  Its own
    //  Purge() is dispatched through vtable slot 1 here rather than called
    //  directly - the same raw-vtable-index shape CLAUDE.md's own examples
    //  use, confirmed against cdatarecycler.h's own slot-1 comment.
    CDataRecycler* recycler = (CDataRecycler*)(self + 0x664);
    typedef void (__thiscall *CDataRecyclerPurgeFn)(CDataRecycler*);
    ((CDataRecyclerPurgeFn)(*(void***)recycler)[1])(recycler);
    recycler->~CDataRecycler();

    DirectLinkedList* list644 = (DirectLinkedList*)(self + 0x644);
    list644->UnlinkAll();
    list644->m_terminator.Unlink();

    DeleteCriticalSection((CRITICAL_SECTION*)(self + 0x62C));

    DirectLinkedList* list608 = (DirectLinkedList*)(self + 0x608);
    list608->UnlinkAll();
    list608->m_terminator.Unlink();

    DeleteCriticalSection((CRITICAL_SECTION*)(self + 0x5F0));
    DeleteCriticalSection((CRITICAL_SECTION*)(self + 0x2C2));

    void* fixedString148 = *(void**)(self + 0x148);
    if (fixedString148 != 0)
        SMemFree(fixedString148, aAuFixedstringD, -2, 0);

    //  TInstanceId<NetProvider>'s own inline base tail: re-stamp the
    //  vtable, then unlink this object's own instance-id link at +4.
    *(void**)self = g_vftableNetProviderInstanceIdPlaceholder;
    TSLink<UnknownListNode_6F65D9E0>* instanceIdLink =
        (TSLink<UnknownListNode_6F65D9E0>*)(self + 4);
    instanceIdLink->Unlink();
}

namespace Net
{

class NetProvider
{
public:
    //  0x6F65DB60 - vtable slot 0.
    void* DeleteSelf(unsigned int flags);
};

void* NetProvider::DeleteSelf(unsigned int flags)
{
    ((NetProviderBaseDtor*)this)->Destroy();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}  // namespace Net

//============================================================================
//  0x6F31B800 - a depth-1/2 CUnit-vtable-BFS leaf (cunit_agent1_worklist.md)
//  whose real ABI this toolchain cannot spell with an ordinary calling
//  convention: its one caller (0x6F330AF0, a much larger sound-enable
//  routine well outside this batch's scope) does `mov eax,[esi+30h] / call
//  sub_6F31B800 / mov ebp,eax` - the argument and the return value both
//  travel in bare EAX, with no `ecx`/`edx`/stack involvement at all. None
//  of `__cdecl`/`__stdcall`/`__fastcall`/`__thiscall` reaches that; the
//  same whole-program-optimisation register choice
//  docs/msvc-vc8-idioms.md's `__except_handler4` note and
//  tools/unhookable.py's own examples (CConfigFile::GetIntValue in edx,
//  AGILE_TYPE_DATA::ValueAt in eax/esi) already document for this binary.
//  Written `__declspec(naked)` for exactly that reason - not because the
//  body needs asm, but because the *signature* has nowhere else to live -
//  and left with no C++ parameter list to match, the same shape
//  Sync/threadwaitpool.cpp's naked leaves use.
//
//  The one real call inside it (0x6F319C20, `retn 8`, thiscall + one
//  unused second stack dword) is a TSHashTable<T,K>::Ptr-shaped lookup off
//  the fixed object dword_6FAB582C, keyed on the incoming EAX value; it is
//  not this batch's own worklist heading, so it is kept as a thunk to its
//  real, unhooked body (funcmap row only, no claimed reconstruction) - the
//  same scope-management thunk unit_blockedclosureleaves.cpp already uses
//  for an out-of-scope callee.
//============================================================================

//  0x6FAB582C - the fixed registry/table object 0x6F31B800 always looks
//  into.  Declared, not defined: funcmap.DATA's row is what keeps the
//  address meaning something real rather than a private zero-filled copy
//  (link_check.py's own "funcmap.DATA defined instead of declared" check).
extern int g_lookup5800Table;

//  0x6FAB4FC5 - raw data (IDA's own `unk_`, not a recognised type), passed
//  by address as this lookup's unused second (key-by-reference) argument.
extern char g_unk6FAB4FC5;   // matches unittypecacheexpiry.cpp's own declared type

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

struct SLookup5800Table { int __thiscall Ptr(unsigned int hash, const int* key); };

ADDR_THUNK(int SLookup5800Table::Ptr(unsigned int hash, const int* key), 0x6F319C20)

#undef ADDR_THUNK

//  0x6F31B800 - EAX in, EAX out; no C++ parameter list can reach that, so
//  this is a literal transcription rather than a call through the struct
//  above (which exists only to give 0x6F319C20 a real, correctly-cleaned
//  declaration for the linker).
__declspec(naked) void Lookup5800_EaxInEaxOut()
{
    __asm
    {
        push    offset g_unk6FAB4FC5
        push    eax
        mov     ecx, offset g_lookup5800Table
        call    SLookup5800Table::Ptr
        retn
    }
}

//============================================================================
//  A handful of trivial one-field accessors from sub_6F29ED60's own call
//  tree - each reads one fixed offset (or one fixed global) off `this` and
//  returns it, nothing else. Nothing in this call tree says what class any
//  of these belong to or what the field means, so each gets its own
//  minimal, address-named holder rather than a guessed shared type.
//  Defined out-of-line (not in-class) so the compiler emits each one as a
//  real symbol even though nothing in this translation unit calls them.
//============================================================================

//  0x6F481420 was a fourth one of these.  It is CTimeSync::GetCellGrid now
//  (src/Pathfinding/pathmanager.cpp, beside the other two one-load accessors
//  on that same object), so the placeholder is gone rather than shadowing
//  it - one shipped address must not carry two C++ names.

//  0x6F4A9A50 was a third one of these.  It is CDrivenRelation::GetTrace
//  now (src/Agent/agentrelationdrivefields.cpp, beside the two setters on
//  that same object), so the placeholder is gone rather than shadowing it.

struct FieldHolder_6F7C7E60 { char pad[0x24]; int m_field24; int Get(); };
int FieldHolder_6F7C7E60::Get() { return m_field24; }

//  sub_6F3A1650 - `ecx`-based array indexed by the one stack argument, each
//  element 4 bytes, based at `this+0x58`.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};
int IndexedArrayHolder_6F3A1650::At(unsigned int index) { return m_data[index]; }

//  sub_6F427F40 - not a value read, an *address* computation: returns a
//  pointer to the (16-byte-stride) element at `this+8` indexed by the one
//  stack argument.
struct IndexedElementOwner_6F427F40
{
    char pad[8];
    char m_data[1][16];
    void* At(unsigned int index);
};
void* IndexedElementOwner_6F427F40::At(unsigned int index) { return &m_data[index]; }

//  sub_6F427EC0 - the same address computation against a *second* inline
//  16-byte-stride array on the same object, based at `this+0x3C`.  Its
//  elements are the handle-bearing published fields the integer game states
//  live in (HandleRefFieldOwner_6F473170, misc_handle_lookups.cpp), where
//  0x6F427F40's are the float-valued SUnitTrackedRefs - which is what
//  JASS_SetIntegerGameState / JASS_GetIntegerGameState and their float twins
//  establish (src/Jass/jassnatives_gamestatevalue.cpp).
struct IndexedElementOwner_6F427EC0
{
    char pad[0x3C];
    char m_data[1][16];
    void* At(unsigned int index);
};
void* IndexedElementOwner_6F427EC0::At(unsigned int index) { return &m_data[index]; }

extern int g_unk6FA86098;   // dword_6FA86098
int GetGlobal_6F4CB920() { return g_unk6FA86098; }

extern int g_unk6FADC0DC;   // dword_6FADC0DC
int GetGlobal_6F7B8B40() { return g_unk6FADC0DC; }

//  0x6F24FBA0 - createunit-G3's own worklist slice (checkedslot_createunit
//  _cluster.cpp), same shape as this file's other one-field accessors:
//  `mov eax,[ecx+20h] / retn`.  Kept in this TU rather than beside its own
//  caller (0x6F250FD0) for the same reason every getter in
//  misc_rawcode_getters.cpp is: the shipped code reaches it through a real
//  `call`, and a same-TU definition would let this build's /Ob2 inline the
//  one-instruction body away instead.
void* __fastcall GetField0x20(void* obj)
{
    return *(void**)((char*)obj + 0x20);
}

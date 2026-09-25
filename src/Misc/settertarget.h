//============================================================================
//  A shared interface reached only through two of its vtable slots
//  (+0x18/index 6, +0x1C/index 7), each a two-argument thiscall setter -
//  eleven addresses in this dump are pure one-line forwarders to one of
//  the two, `this` untouched, baking in a different second argument (an
//  id/field-selector constant) per address. Nothing in this call tree
//  reaches slots 0-5 or gives either real setter a name beyond what it
//  does structurally, so both stay named for their vtable position and the
//  placeholders stay unnamed - the same "only the reached slot gets a real
//  name" shape src/vslot4thunk.h already uses for IVSlot4Target.
//
//  Each forwarder's own `value` argument arrives on the stack
//  (`mov edx, [esp+arg_0]`, `retn 4`) - real `__thiscall`, not `__fastcall`
//  with `value` in `edx`. A live 250-hook run confirmed this the hard way:
//  declared `__fastcall(ISetterTarget*, int)` (settertarget_forwards.cpp's
//  own earlier version), every one of these popped 0 stack bytes on return
//  instead of the shipped `retn 4`, leaving the real caller's own pushed
//  argument on the stack after every call - the same
//  "`__thiscall` on a free function: rejected by this compiler" trap
//  docs/msvc-vc8-idioms.md already documents for `CDataAllocator::
//  ConstructCWidget`, just reached through `__fastcall`'s silent
//  acceptance instead of a `__thiscall` compile error. The instruction
//  differ could not see it (both conventions produce the same `push`/
//  `call` shape from the callee's own body - the mismatch is only visible
//  in the stack cleanup this build made unreachable by choosing the wrong
//  keyword), but ten crashes in one live run could. Fixed by making these
//  real, non-virtual member functions of `ISetterTarget` itself
//  (settertarget_forwards.cpp) rather than free functions - the same fix
//  CWidget__Constructor's own equivalent trap already used.
//============================================================================
#ifndef SETTERTARGET_H
#define SETTERTARGET_H

class ISetterTarget
{
public:
    virtual void m_reservedSlot0() = 0;
    virtual void m_reservedSlot1() = 0;
    virtual void m_reservedSlot2() = 0;
    virtual void m_reservedSlot3() = 0;
    virtual void m_reservedSlot4() = 0;

    //  vtable+0x14/index 5 - the fourth reached slot, found this session
    //  (settertarget_queries.cpp's own sub_6F6F2D10): a two-argument setter
    //  like SetFieldA/B below, but with two more caller-supplied arguments
    //  after it - `value`, then a caller-chosen "mode" (baked to the
    //  literal 1 at 0x6F6F2D10, the only address that reaches this slot),
    //  then two more plain ints this call tree gives no identifying use
    //  for.  Named for the shape, not a recovered purpose.
    virtual void SetFieldRange(int value, int mode, int a, int b) = 0;

    //  vtable+0x18/index 6 and +0x1C/index 7 - two-argument thiscall
    //  setters, `this` untouched, an id/field-selector constant as the
    //  second argument.  SetFieldA is declared `int`-returning rather than
    //  `void`: the eleven SetFieldA_Id* forwarders below all discard it
    //  (`call eax; retn 4`, no read of eax after), but a second family of
    //  callers at this same slot (settertarget_queries.cpp) captures it and
    //  turns it into a canonical 0/1 (`neg eax; sbb eax,eax; neg eax`) -
    //  proof the slot really does return something, not a claim about what.
    //  A discarded return costs nothing to declare either way, so both
    //  families compile correctly against the one true signature.
    virtual int SetFieldA(int value, int id) = 0;    // vtable+0x18
    virtual void SetFieldB(int value, int id) = 0;   // vtable+0x1C

    //  The eleven forwarders (settertarget_forwards.cpp) - real member
    //  functions so `this` lands in `ecx` and `value` on the stack,
    //  matching each address's own `retn 4`.
    void SetFieldB_Id0(int value);
    void SetFieldA_Id2(int value);
    void SetFieldB_Id3(int value);
    void SetFieldA_Id4(int value);
    void SetFieldA_Id8_6F6F2D90(int value);
    void SetFieldA_Id8_6F6F0D90(int value);
    void SetFieldA_Id0x30(int value);
    void SetFieldA_Id0xA(int value);
    void SetFieldA_Id7(int value);
    void SetFieldA_Id9_6F6F1EB0(int value);
    void SetFieldA_Id9_6F6F0DB0(int value);

    //  This session's own batch (settertarget_queries.cpp): the same two
    //  vtable slots reached through a second family of call shapes -
    //  SetFieldA's own return value kept and boolified, a computed id
    //  chosen from a small caller selector, and one four-argument setter
    //  at a slot (0x14) nothing in this call tree had reached before.
    void SetFieldRange_Mode1(int value, int a, int b);   // 0x6F6F2D10
    int QueryFieldA_Id18(int value);                     // 0x6F6F2DB0
    int QueryFieldA_Id19(int value);                     // 0x6F6F2DD0
    int QueryFieldA_Id6(int value);                      // 0x6F6F2DF0
    int QueryFieldA_Id7(int value);                      // 0x6F6F2E10
    int QueryFieldA_Id20(int value);                     // 0x6F6F2E50
    int QueryFieldA_BySelector(int value, int selector);  // 0x6F6F2E90
    int QueryFieldA_Id5Remapped(int value);               // 0x6F6FA050
    void SetFieldRange_Mode7(int value, int a, int b);    // 0x6F6F0D70

    //  0x6F6FDE70 - four SetFieldA queries at consecutive ids packed into
    //  one dword and written through an output pointer; see
    //  settertarget_pack.cpp for the exact byte order and the five
    //  0x6F6FExxx callers that each supply one four-id block.
    void QueryFourFieldsPacked(int* out, int value, int id1, int id2,
                               int id3, int id0OrSentinel);

    //  The five 0x6F6FExxx callers - each queries one four-consecutive-id
    //  block (base, base+1, base+2, base+3) through QueryFourFieldsPacked
    //  and returns `out` unchanged (not `this` - `this` never moves out of
    //  ecx in any of the five, `out` is what comes back in eax).  Named by
    //  each block's base id.
    int* PackFourFields_Base0x1C(int* out, int value);  // 0x6F6FE350
    int* PackFourFields_Base0x20(int* out, int value);  // 0x6F6FE370
    int* PackFourFields_Base0x24(int* out, int value);  // 0x6F6FE390
    int* PackFourFields_Base0x28(int* out, int value);  // 0x6F6FE3B0
    int* PackFourFields_Base0x2C(int* out, int value);  // 0x6F6FE330
};

#endif

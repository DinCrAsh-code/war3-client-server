//============================================================================
//  0x6F6F0Bxx - 0x6F6F3Axx (and 0x6F785220) - twenty-six one-field wrappers
//  over the same two vtable slots.
//
//  Whatever object these belong to exposes a numbered field space through two
//  virtual methods - slot 0x18 reads one field of a target object, slot 0x14
//  writes one - and each function here is one field id baked in.  That is the
//  whole of them: no other work and no state of their own.
//
//  Three shapes, and the only differences between members of each are the
//  field id and the address:
//
//      Query<addr>      slot 0x18, value handed straight back;
//      QueryFlag<addr>  slot 0x18, normalised to 0/1 with the usual
//                       neg / sbb / neg triple;
//      Assign<addr>     slot 0x14, two more arguments passed through.
//
//  The owner is not identified.  These sit in the Storm/Terrain tail of the
//  JASS_SetItemPosition dump that target explicitly scoped out, and nothing
//  in this repo names the class or its vtable - so they carry the shape and
//  the address for a name, the way the rest of the trivial-accessor sweep
//  does.  The two dispatch helpers are defined inside the struct so that
//  MSVC inlines them and emits no out-of-line copy the shipped build has no
//  counterpart for.
//============================================================================
#include "game.h"

struct SFieldDispatch;

//  vtable +0x18 and +0x14.
typedef int (__thiscall *FieldQueryFn)(void* self, void* target, int field);
typedef int (__thiscall *FieldAssignFn)(void* self, void* target, int field,
                                        int a, int b);

struct SFieldDispatch
{
    int Dispatch18(void* target, int field)
    {
        return ((FieldQueryFn)(*(void***)this)[0x18 / 4])(this, target, field);
    }

    int Dispatch14(void* target, int field, int a, int b)
    {
        return ((FieldAssignFn)(*(void***)this)[0x14 / 4])(this, target, field,
                                                           a, b);
    }

    int Query_6F6F0B80(void* target);
    int Query_6F6F0C10(void* target);
    int Query_6F6F0DD0(void* target);
    int Query_6F6F10B0(void* target);
    int Query_6F6F1330(void* target);
    int Query_6F6F15B0(void* target);
    int Query_6F6F1670(void* target);
    int Query_6F6F1CE0(void* target);
    int Query_6F6F23A0(void* target);
    int Query_6F6F2BF0(void* target);
    int Query_6F6F2C30(void* target);
    int Query_6F6F37A0(void* target);
    int QueryFlag_6F6F0F50(void* target);
    int QueryFlag_6F6F1E10(void* target);
    int QueryFlag_6F6F1E30(void* target);
    int QueryFlag_6F6F1E50(void* target);
    int QueryFlag_6F6F1E70(void* target);
    int QueryFlag_6F6F2000(void* target);
    int QueryFlag_6F6F2220(void* target);
    int Assign_6F6F0E10(void* target, int a, int b);
    int Assign_6F6F1170(void* target, int a, int b);
    int Assign_6F6F1290(void* target, int a, int b);
    int Assign_6F6F15D0(void* target, int a, int b);
    int Assign_6F6F1CC0(void* target, int a, int b);
    int Assign_6F6F2240(void* target, int a, int b);
    int Assign_6F785220(void* target, int a, int b);
};

//  0x6F6F0B80 - field 0x0E.
int SFieldDispatch::Query_6F6F0B80(void* target)
{
    return Dispatch18(target, 0x0E);
}

//  0x6F6F0C10 - field 0x03.
int SFieldDispatch::Query_6F6F0C10(void* target)
{
    return Dispatch18(target, 0x03);
}

//  0x6F6F0DD0 - field 0x06.
int SFieldDispatch::Query_6F6F0DD0(void* target)
{
    return Dispatch18(target, 0x06);
}

//  0x6F6F10B0 - field 0x28.
int SFieldDispatch::Query_6F6F10B0(void* target)
{
    return Dispatch18(target, 0x28);
}

//  0x6F6F1330 - field 0x09.
int SFieldDispatch::Query_6F6F1330(void* target)
{
    return Dispatch18(target, 0x09);
}

//  0x6F6F15B0 - field 0x22.
int SFieldDispatch::Query_6F6F15B0(void* target)
{
    return Dispatch18(target, 0x22);
}

//  0x6F6F1670 - field 0x05.
int SFieldDispatch::Query_6F6F1670(void* target)
{
    return Dispatch18(target, 0x05);
}

//  0x6F6F1CE0 - field 0x01.
int SFieldDispatch::Query_6F6F1CE0(void* target)
{
    return Dispatch18(target, 0x01);
}

//  0x6F6F23A0 - field 0x21.
int SFieldDispatch::Query_6F6F23A0(void* target)
{
    return Dispatch18(target, 0x21);
}

//  0x6F6F2BF0 - field 0x02.
int SFieldDispatch::Query_6F6F2BF0(void* target)
{
    return Dispatch18(target, 0x02);
}

//  0x6F6F2C30 - field 0x05.
int SFieldDispatch::Query_6F6F2C30(void* target)
{
    return Dispatch18(target, 0x05);
}

//  0x6F6F37A0 - field 0x02.
int SFieldDispatch::Query_6F6F37A0(void* target)
{
    return Dispatch18(target, 0x02);
}

//  0x6F6F0F50 - field 0x03, as a flag.
int SFieldDispatch::QueryFlag_6F6F0F50(void* target)
{
    return Dispatch18(target, 0x03) != 0;
}

//  0x6F6F1E10 - field 0x03, as a flag.
int SFieldDispatch::QueryFlag_6F6F1E10(void* target)
{
    return Dispatch18(target, 0x03) != 0;
}

//  0x6F6F1E30 - field 0x04, as a flag.
int SFieldDispatch::QueryFlag_6F6F1E30(void* target)
{
    return Dispatch18(target, 0x04) != 0;
}

//  0x6F6F1E50 - field 0x05, as a flag.
int SFieldDispatch::QueryFlag_6F6F1E50(void* target)
{
    return Dispatch18(target, 0x05) != 0;
}

//  0x6F6F1E70 - field 0x06, as a flag.
int SFieldDispatch::QueryFlag_6F6F1E70(void* target)
{
    return Dispatch18(target, 0x06) != 0;
}

//  0x6F6F2000 - field 0x2D, as a flag.
int SFieldDispatch::QueryFlag_6F6F2000(void* target)
{
    return Dispatch18(target, 0x2D) != 0;
}

//  0x6F6F2220 - field 0x04, as a flag.
int SFieldDispatch::QueryFlag_6F6F2220(void* target)
{
    return Dispatch18(target, 0x04) != 0;
}

//  0x6F6F0E10 - field 0x0B.
int SFieldDispatch::Assign_6F6F0E10(void* target, int a, int b)
{
    return Dispatch14(target, 0x0B, a, b);
}

//  0x6F6F1170 - field 0x02.
int SFieldDispatch::Assign_6F6F1170(void* target, int a, int b)
{
    return Dispatch14(target, 0x02, a, b);
}

//  0x6F6F1290 - field 0x03.
int SFieldDispatch::Assign_6F6F1290(void* target, int a, int b)
{
    return Dispatch14(target, 0x03, a, b);
}

//  0x6F6F15D0 - field 0x02.
int SFieldDispatch::Assign_6F6F15D0(void* target, int a, int b)
{
    return Dispatch14(target, 0x02, a, b);
}

//  0x6F6F1CC0 - field 0x00.
int SFieldDispatch::Assign_6F6F1CC0(void* target, int a, int b)
{
    return Dispatch14(target, 0x00, a, b);
}

//  0x6F6F2240 - field 0x20.
int SFieldDispatch::Assign_6F6F2240(void* target, int a, int b)
{
    return Dispatch14(target, 0x20, a, b);
}

//  0x6F785220 - field 0x29.
int SFieldDispatch::Assign_6F785220(void* target, int a, int b)
{
    return Dispatch14(target, 0x29, a, b);
}


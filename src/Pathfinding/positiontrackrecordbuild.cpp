//============================================================================
//  0x6F473980 - PositionTrack::RecordSlot8: build the 0x2C-byte request
//  block CItem's vtable slot 8 (Item/item_vtable2.cpp) hands on, and
//  forward it.
//
//  Was a `mov eax,<address> / jmp eax` redirect called "a 60-instruction
//  record builder... squarely the kind of deep procedure this session
//  stayed out of".  It is thirty-four instructions and it builds a block,
//  it does not walk one: two FourCC tags, three of the four arguments, one
//  global, five zeroed words and two -1 sentinels, then one call.
//
//  It is the same shape BuildAgileTypeObject (agiletypeobjectbuild.cpp)
//  has - a 0x2C-byte block whose first word is a '^'/'`'-suffixed tag - so
//  this is a request block in the same family, not a "record".
//
//  Three things are in the shipped code rather than in the reading of it:
//
//   * `b == 0` short-circuits the whole thing and returns `a` unchanged, so
//     the function has a return value and it is not the forwarded call's on
//     that path;
//   * `this` is never touched - it travels in ecx straight through to the
//     callee, which is why this is a member and not a __fastcall free
//     function (positiontrack.h already records the `retn 10h` evidence
//     for that);
//   * the two -1 sentinels are written from one register (`or eax,-1`
//     after eax has served as the zero for the five zeroed words), and the
//     +0x28 one lands before the +0x24 one.
//
//  Moved here from Widget/positionrecordbuild.cpp once Widget/position.h's
//  own `Position` was confirmed to be this exact class - not a separate,
//  unrelated one - and merged (see positiontrack.h's own header comment).
//  Same address, same body, same verdict.
//
//  Own translation unit: slot 8 reaches this with a real call and it makes
//  one of its own.
//============================================================================
#include "positiontrack.h"
#include "positionrecordblock.h"
#include "positionrecordarray.h"
#include "floatmini.h"   // g_unk6FAAE624 - already declared there,
                        // as `const unsigned int`; a second declaration with a
                        // different type is a different mangled name and a
                        // link failure verify.py cannot see.

int PositionTrack::RecordSlot8(int a, int b, int c, int d)
{
    if (b == 0)
        return a;

    SPositionRecordBlock block;

    //  `c` is read into a local well ahead of its store, and the global
    //  after it: the shipped code keeps both in esi in turn (`mov esi,
    //  [esp+30h+arg_8]` right after the prologue, `mov esi,
    //  dword_6FAAE624` between the two tag stores).  Storing either
    //  straight from its source moves it.
    int argC = c;

    block.m_zero10 = 0;
    block.m_zero14 = 0;
    block.m_zero18 = 0;
    block.m_zero1C = 0;
    block.m_zero20 = 0;

    //  +0x28 before +0x24: the shipped code stores the sentinel into
    //  var_4 and then into var_8.
    block.m_sentinel28 = -1;
    block.m_sentinel24 = -1;

    //  The third argument lands before the two tags, and the global after
    //  them - that is the shipped store order, not a tidy one.
    block.m_argC = argC;
    unsigned int global = g_unk6FAAE624;
    block.m_tag0 = 0x5E70726F;
    block.m_tag1 = 0x60706F73;
    block.m_global = global;

    return ((SPositionRecordSink*)this)->SubmitRecord(a, b, &block, d);
}

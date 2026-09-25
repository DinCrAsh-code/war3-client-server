//============================================================================
//  SUnitTrackedRef - the handle-bearing sub-object CUnit embeds five times
//  (+0x98, +0xB8, +0xFC, +0x118 and +0x214).  See unit.h.
//
//  Layout is TRefCnt's plus a handle pair, and every method here starts the
//  same way: `mov edx, [ecx+0Ch]` / `mov ecx, [ecx+8]` / `call
//  LookupHandle` - the handle-pair calling shape this repo uses everywhere
//  (game.h).  So the object does not *hold* the value it publishes; it
//  holds a handle to whatever does.
//
//  What the resolved object's +0x80 and +0x84 are is settled by the two
//  setters rather than guessed: 0x6F477460 hands 0x6F4A9390 the pair
//  {*value, resolved->+0x84} and 0x6F477400 hands it {resolved->+0x80,
//  *value}.  Each writes one end of a two-ended range and passes the other
//  end back with the value it already had, which makes +0x80 the low end
//  and +0x84 the high end - and the two getters agree, 0x6F477440 reading
//  +0x80 and 0x6F4773E0 reading +0x84.  That is the same "write the field
//  I am not changing straight back" evidence floatmini.h's own SetPair
//  note is built on.
//
//  CUnit's vtable slots 76/77 and 80/81 are the four of them one class up.
//============================================================================
#ifndef UNITTRACKEDREF_H
#define UNITTRACKEDREF_H

#include "CFloat.h"

struct SUnitTrackedRef
{
    //  0x6F4773E0 - the high end of the range, out through the caller's
    //  buffer.
    CFloat* GetHigh(CFloat* out);
    //  0x6F477440 - the low end.
    CFloat* GetLow(CFloat* out);
    //  0x6F477460 - set the low end, republishing the high end unchanged.
    void SetLow(const CFloat* value);
    //  0x6F477400 - set the high end, republishing the low end unchanged.
    void SetHigh(const CFloat* value);
    //  0x6F4773A0 - the current value itself: how far along the range the
    //  resolved object has travelled, evaluated through its own ramp.
    //  Reconstructed with this target - see unittrackedref.cpp.
    CFloat* GetValue(CFloat* out);
    //  0x6F4774A0 - set *both* ends of the range at once, from two
    //  separate CFloat pointers.  The setter the two one-ended ones above
    //  are variants of; reconstructed with this target.
    void SetRange(const CFloat* low, const CFloat* high);
    //  0x6F477350 - publish a new value.
    void SetValue(const CFloat* value);
    //  0x6F47C490 - wrap the value into one step (CFloatModStep,
    //  floatmodstep.cpp) and publish that.  Below this slice; thunked.
    void SetWrappedValue(const CFloat* value);
    //  0x6F47C4F0 - the same, handing `this` back.  See
    //  src/Math/floatmodstepassign.cpp.
    SUnitTrackedRef* AssignWrappedValue(const CFloat* value);
    //  0x6F47C4D0 - the current value wrapped into one step, returned *by
    //  value*: `retn 4` is MSVC's hidden return buffer and nothing else.
    //  Every one of its eight call sites reads an angle out of a camera
    //  property (CCameraWar3::GetProperty's four angle cases and the four
    //  camera writers below them), which is what the "wrapped" is for - one
    //  turn.  See src/Math/floatmodstepvalue.cpp.
    CFloat GetWrappedValue();
    //  0x6F47C4B0 - what that one forwards to, and its only caller: read
    //  the live value, wrap it, construct the answer into the same hidden
    //  buffer.  See src/Math/floatmodstepget.cpp.
    CFloat WrapCurrentValue();
    //  0x6F4774E0 - make a FloatModifier that walks this value to
    //  `value` over `span`, and hand it back as a counted reference.
    //  `retn 0Ch`: two parameters plus MSVC's hidden return buffer.  See
    //  floatpropmodifiermake.cpp.
    struct SFloatModifierRef MakeFloatModifier(const CFloat* value,
                                               const CFloat* span);

    //  0x6F477550 - make a FloatListener that fires when this value
    //  crosses one boundary, and hand it back as a counted reference.
    //  `retn 18h`: five parameters plus MSVC's hidden return buffer.
    //  See floatboundarylistener.cpp.
    struct SFloatListenerRef MakeBoundaryListener(int above,
                                                  const CFloat* boundary,
                                                  unsigned int msgId,
                                                  void* subject, int mode);

    void*        m_vtable;      // +0x00 - ~CUnit stamps ??_7TRefCnt@@6B@ here
    int          m_refcount;    // +0x04
    unsigned int m_handle;      // +0x08
    int          m_typeTag;     // +0x0C
};

#endif

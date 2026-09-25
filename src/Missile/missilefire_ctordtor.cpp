//============================================================================
//  0x6F0F66A0 / 0x6F0F7210 - CMissileFire's own constructor and vtable
//  slot 1 (the scalar-deleting destructor).  No separate non-scalar
//  ~CMissileFire survives anywhere in this dump's own reachable closure:
//  the real destructor body is fully inlined into DeleteSelf, the
//  outermost level nothing further derived inlines it into.
//
//  DeleteSelf's own body, read address-by-address from raw_asm:
//
//      mov [esi+78h], offset ??_7TRefCnt@@6B@   ; restamp m_projectilePosition
//      call sub_6F03E800                         ; CBulletBase::~CBulletBase()
//      test [esp+4+arg_0], 1
//      jz short skip
//      push 0; push -1; push "delete"; push esi; call Storm_403
//    skip:
//      mov eax, esi; pop esi; retn 4
//
//  Two things worth naming explicitly, same as missile_ctordtor.cpp's own
//  note one level down:
//
//    - only m_projectilePosition's own vtable pointer is restamped.  Every
//      other field this class or an ancestor added past CBulletBase's own
//      0x78 bytes is a plain POD with no vtable of its own to restamp
//      (FloatMini has an explicit but trivial virtual destructor, the
//      other fields are bare dwords) - so there is nothing else for the
//      shipped stream to touch, including this class's own m_fieldAC/
//      m_fieldB0/m_floatF/m_floatG.
//    - the call goes straight to CBulletBase::~CBulletBase() (0x6F03E800),
//      *skipping* CBullet's and CMissile's own vtable-slot-1 bodies both -
//      this level inlines the *whole* chain, the same "this level inlines
//      the immediate base's own destructor body rather than calling it"
//      shape every level of this family uses, just collapsed two levels
//      deeper because nothing between here and CBulletBase does anything
//      else worth keeping a separate call for.
//============================================================================
#include "missilefire.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CMissileFire* CMissileFire::DeleteSelf(int flags)
{
    this->CMissileFire::~CMissileFire();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

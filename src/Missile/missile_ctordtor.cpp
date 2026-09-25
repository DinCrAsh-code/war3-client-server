//============================================================================
//  0x6F0A4410 - CMissile's vtable slot 1, the scalar-deleting destructor.
//  No separate non-scalar ~CMissile survives anywhere in this dump's own
//  reachable closure: the real destructor body is fully inlined here, the
//  outermost level nothing further derived inlines it into.
//
//  Its own body, read address-by-address from raw_asm:
//
//      mov [esi+78h], offset ??_7TRefCnt@@6B@   ; restamp m_projectilePosition
//      call sub_6F03E800                         ; CBulletBase::~CBulletBase()
//      test [esp+4+arg_0], 1
//      jz short skip
//      push 0; push -1; push "delete"; push esi; call Storm_403
//    skip:
//      mov eax, esi; pop esi; retn 4
//
//  Two things worth naming explicitly:
//
//    - only m_projectilePosition's own vtable pointer is restamped.  Every
//      other field this class or CBullet added past CBulletBase's own 0x78
//      bytes (m_floatMini at +0x88, m_floatD/m_floatE at +0x90/+0x98,
//      m_fieldA0Handle/m_fieldA4TypeTag/m_reservedA8 at +0xA0/+0xA4/+0xA8)
//      is a plain POD with no vtable of its own to restamp - FloatMini has
//      an explicit but trivial virtual destructor, same as bullet.h's own
//      m_floatMini, and the other three are bare dwords - so there is
//      nothing else for the shipped stream to touch.
//    - the call at 0x6F0A441A goes straight to CBulletBase::~CBulletBase()
//      (0x6F03E800), *skipping* CBullet's own vtable-slot-1 body entirely -
//      the same "this level's DeleteSelf inlines the immediate base's own
//      destructor body rather than calling it" shape CBullet's own
//      DeleteSelf (bullet_ctordtor.cpp) uses one level down for
//      CBulletBase.
//
//  Letting the compiler generate CMissile's own implicit destructor and
//  calling it explicitly reproduces this shape - no hand-written body
//  needed for the *logic*.  One known gap survives regardless (see below).
//============================================================================
#include "missile.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CMissile* CMissile::DeleteSelf(int flags)
{
    this->CMissile::~CMissile();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

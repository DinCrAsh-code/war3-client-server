//============================================================================
//  The `rect` natives at 0x6F3C18E0..0x6F3C1A20 plus the four bound getters,
//  plus `SetRect` (0x6F3C1710) - not itself in that address range, but
//  co-located here because it is body-for-body JASS_SetRectFromLoc below
//  with the pair read straight from four real pointers instead of two
//  location handles, and its own callees (ResolveRectHandle, Resize) are
//  each already reconstructed in their own translation unit, so nothing
//  here risks folding into either.
//  See jassnatives.h.
//============================================================================
#include "jassnatives.h"
#include "gamebounds.h"

//----------------------------------------------------------------------------
//  0x6F3C1710 - `SetRect` "(Hrect;RRRR)V": point an existing rect at the
//  box the two corners (x1,y1) and (x2,y2) span.
//
//  Body for body this is JASS_SetRectFromLoc below with the location reads
//  replaced by direct pointer dereferences - same clamp into the map's
//  playable rectangle, same four-way min/max pick kept as pointers so all
//  four coordinates stay live on the x87 stack across the
//  `fld`/`fld`/`fcom`...`fxch st(3)`/`fcomp st(2)`/`fcompp` chain, same
//  sixteen-byte block built in the outgoing argument slot and the same
//  setter (Resize, not SetBounds - this is an *existing* rect).
//----------------------------------------------------------------------------
void __cdecl JASS_SetRect(int hRect, const int* x1, const int* y1,
                          const int* x2, const int* y2)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return;

    CGridVec2 a(kCFloatNoInit);
    CGridVec2 b(kCFloatNoInit);

    int aX = *x1;
    int aY = *y1;
    a.m_x = *(const CFloat*)&aX;
    a.m_y = *(const CFloat*)&aY;

    int bX = *x2;
    int bY = *y2;
    b.m_x = *(const CFloat*)&bX;
    b.m_y = *(const CFloat*)&bY;

    ClampToGameBounds(&a);
    ClampToGameBounds(&b);

    const CFloat* maxX =
        (*(const float*)&a.m_x > *(const float*)&b.m_x) ? &a.m_x : &b.m_x;
    const CFloat* maxY =
        (*(const float*)&a.m_y > *(const float*)&b.m_y) ? &a.m_y : &b.m_y;
    const CFloat* minX =
        (*(const float*)&a.m_x < *(const float*)&b.m_x) ? &a.m_x : &b.m_x;
    const CFloat* minY =
        (*(const float*)&a.m_y < *(const float*)&b.m_y) ? &a.m_y : &b.m_y;

    rect->Resize(SJassBounds(minY->m_bits, minX->m_bits,
                             maxY->m_bits, maxX->m_bits));
}

//----------------------------------------------------------------------------
//  0x6F3C1940 / 0x6F3C1960 / 0x6F3C1980 / 0x6F3C19A0 - `GetRectMinX`,
//  `GetRectMinY`, `GetRectMaxX`, `GetRectMaxY`, all "(Hrect;)R".  Unlike the
//  location getters these do not fall back to zero: an unresolvable rect
//  answers with the module's own default bound, and the two Max natives read
//  the block in the other order (+0x30 for X, +0x2C for Y), which is the
//  shipped layout and not a transcription slip.
//----------------------------------------------------------------------------
int __cdecl JASS_GetRectMinX(int hRect)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return g_CFloatZero.m_bits;

    return rect->m_minY;
}

int __cdecl JASS_GetRectMinY(int hRect)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return g_CFloatZero.m_bits;

    return rect->m_minX;
}

int __cdecl JASS_GetRectMaxX(int hRect)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return g_CFloatZero.m_bits;

    return rect->m_maxY;
}

int __cdecl JASS_GetRectMaxY(int hRect)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return g_CFloatZero.m_bits;

    return rect->m_maxX;
}

//----------------------------------------------------------------------------
//  0x6F3C18E0 / 0x6F3C1910 - `GetRectCenterX` / `GetRectCenterY`, both
//  "(Hrect;)R".  GetCentre fills a two-dword buffer and each native reads its
//  own half back.  The X native keeps the default-bound fallback the getters
//  above use; the Y native does not - it returns whatever was in eax, which
//  after a failed resolve is zero.  That asymmetry is in the shipped code.
//----------------------------------------------------------------------------
int __cdecl JASS_GetRectCenterX(int hRect)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return g_CFloatZero.m_bits;

    int centre[2];
    ((SJassRectCentre*)((char*)rect + 0x24))->GetCentre(centre);
    return centre[0];
}

int __cdecl JASS_GetRectCenterY(int hRect)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return 0;

    int centre[2];
    ((SJassRectCentre*)((char*)rect + 0x24))->GetCentre(centre);
    return centre[1];
}

//----------------------------------------------------------------------------
//  0x6F3C19C0 - `MoveRectTo` "(Hrect;RR)V".  MoveTo takes its point as two
//  stack dwords rather than by pointer, which is why the shipped code carves
//  eight bytes off the stack by hand and writes the pair into them: that is
//  the compiler pushing two by-value arguments, not a local being built.
//----------------------------------------------------------------------------
void __cdecl JASS_MoveRectTo(int hRect, const int* x, const int* y)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (rect)
    {
        ((SJassRectCentre*)rect)->MoveTo(
            SJassPoint(*(const CFloat*)x, *(const CFloat*)y));
    }
}

//----------------------------------------------------------------------------
//  0x6F3C19F0 - `MoveRectToLoc` "(Hrect;Hlocation;)V", the same call with the
//  point read out of a location instead.
//
//  The pair is read out through two named ints before either half of the
//  outgoing point is written, and the *loads* run in the opposite order to
//  the *stores*: y is named first and x is stored first.  That is not a
//  stylistic choice - handing the constructor the two dereferences directly
//  makes MSVC read +0x24 before +0x28 and costs four instructions of the
//  twenty.  Same rule as CJassGameState::DefineStartLocation
//  (jassgamestate_startlocation.cpp) and JASS_SetRectFromLoc below.
//----------------------------------------------------------------------------
void __cdecl JASS_MoveRectToLoc(int hRect, int hLocation)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (rect)
    {
        SJassLocation* location = ResolveLocationHandle(hLocation);

        if (location)
        {
            int y = location->m_y;
            int x = location->m_x;

            ((SJassRectCentre*)rect)->MoveTo(
                SJassPoint(*(const CFloat*)&x, *(const CFloat*)&y));
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F3C17F0 - `SetRectFromLoc` "(Hrect;Hlocation;Hlocation;)V": point an
//  existing rect at the box two locations span.
//
//  Body for body this is JASS_RectFromLoc (jassnatives_locfactory.cpp) with
//  the factory removed - the same two coordinate pairs copied y-then-x, the
//  same four picks, the same block - and the same one exact, because there is
//  no scoped agent slot here and so no SEH frame.
//
//  The two copies are the shape MoveRectToLoc above records: both reals of a
//  point are named *before* either is stored, y first, and then stored x
//  first.  Assigning the fields straight from the location interleaves each
//  load with its own store (the compiler cannot rule out the second load
//  aliasing the first store), and naming them but keeping the stores in
//  y-then-x order flips both loads to ascending address order; each costs
//  three of the seventy-seven.  Past that copy it is the
//  same two clamps into the map's playable rectangle and the same four picks
//  kept as *pointers* so that all four coordinates stay on the x87 stack
//  across the whole `fld`/`fld`/`fcom` ... `fxch st(3)`/`fcomp st(2)`/
//  `fcompp` sequence, and the same sixteen-byte block built directly in the
//  outgoing argument slot with the Y pair in +0x24/+0x2C and the X pair in
//  +0x28/+0x30.  Each pick names its own answer first (`a > b ? &a : &b` for
//  a max) because that is the one spelling that reduces to `fcom` /
//  `fnstsw` / `test ah,5` / `jnp` with the default address already in a
//  register; see that function's own comment for the three spellings that do
//  not.
//
//  The one thing it does differently is the setter: an existing rect gets
//  SJassRect::Resize (0x6F3DB9D0), not the SetBounds (0x6F3DB8F0) a freshly
//  made one gets.  Two shipped addresses, the same nine instructions; see
//  jassrectbounds.cpp.
//----------------------------------------------------------------------------
void __cdecl JASS_SetRectFromLoc(int hRect, int hLocationA, int hLocationB)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (!rect)
        return;

    SJassLocation* locA = ResolveLocationHandle(hLocationA);
    SJassLocation* locB = ResolveLocationHandle(hLocationB);

    if (!locA || !locB)
        return;

    CGridVec2 a(kCFloatNoInit);
    CGridVec2 b(kCFloatNoInit);

    int aY = locA->m_y;
    int aX = locA->m_x;
    a.m_x = *(const CFloat*)&aX;
    a.m_y = *(const CFloat*)&aY;

    int bY = locB->m_y;
    int bX = locB->m_x;
    b.m_x = *(const CFloat*)&bX;
    b.m_y = *(const CFloat*)&bY;

    ClampToGameBounds(&a);
    ClampToGameBounds(&b);

    const CFloat* maxX =
        (*(const float*)&a.m_x > *(const float*)&b.m_x) ? &a.m_x : &b.m_x;
    const CFloat* maxY =
        (*(const float*)&a.m_y > *(const float*)&b.m_y) ? &a.m_y : &b.m_y;
    const CFloat* minX =
        (*(const float*)&a.m_x < *(const float*)&b.m_x) ? &a.m_x : &b.m_x;
    const CFloat* minY =
        (*(const float*)&a.m_y < *(const float*)&b.m_y) ? &a.m_y : &b.m_y;

    rect->Resize(SJassBounds(minY->m_bits, minX->m_bits,
                             maxY->m_bits, maxX->m_bits));
}

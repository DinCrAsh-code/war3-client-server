//============================================================================
//  0x6F496A40 - CBhPoProjectile::Apply, vtable slot 3.  See bhpoprojectile.h's
//  own header comment.  Own translation unit, address-adjacent to nothing
//  else already reconstructed (0x6F49xxxx, same module as the Save/Load
//  pair in bhpoprojectile_syncio.cpp and NotifyVelocityDelta in
//  bhpoprojectile_notify.cpp, but each kept in its own TU the way this
//  cluster's own siblings already are).
//============================================================================
#include "bhpoprojectile.h"
#include "prbehavior.h"

void CBhPoProjectile::Apply(SHandleRegistrationRequest* request)
{
    //  Non-virtual call into the base's own slot-3 body: this repo has no
    //  real `class CBhPoProjectile : public NIpse::CPrBehavior` anywhere in
    //  this chain (prbehavior.h's own file header explains why), so the
    //  base method is reached by reinterpreting `this` the same way every
    //  other override in this family already does.
    ((NIpse::CPrBehavior*)this)->AttachHandle(request);

    m_startX = 0;
    m_velX = 0;
    m_startY = 0;
    m_velY = 0;
    m_reserved48 = 0;

    //  First of two separate g_CFloatZero loads - kept apart because the
    //  shipped code reloads the global a second time below rather than
    //  keeping one copy live across every store, the same tell
    //  Agent/impactcontext_ctor.cpp's own header already names for two
    //  real copy-initialisations. (The compiler's own register choice for
    //  each individual store in this function - which of eax/ecx/edx a
    //  given zero rides in - does not track source-level variable identity
    //  at this optimisation level; tried splitting the zero-groups into
    //  their own locals, no change to the generated listing, reverted.)
    unsigned int zero1 = g_CFloatZero.m_bits;
    m_velOri = zero1;
    m_velMag = zero1;
    *(unsigned int*)&m_reserved4C = zero1;
    m_velTime = zero1;

    m_velPeriod = 0;
    m_boundTypeTag6C = -1;
    m_boundHandle68 = (unsigned int)-1;

    unsigned int zero2 = g_CFloatZero.m_bits;
    m_reserved78 = zero2;
    m_reserved74 = zero2;

    m_reserved30 |= 0x40000;
}

//============================================================================
//  0x6F502910 - the same swap over the model's particle emitters.  Each is
//  0x164 bytes and takes the pair straight through the engine-side setter at
//  0x6F7E5B00; nothing here looks at an id.
//
//  The shipped build takes the model in ebx - the whole-program convention -
//  so it is `this` here and the caller pays one move.
//============================================================================
#include "material.h"
#include "renderthunks.h"

void CModelInstance::ReplaceParticleTexture(int id, int alphaMode)
{
    for (unsigned int i = 0; i < m_particleCount; i++)
        ((PARTICLEEMITTER*)(m_particles + i * 0x164))->SetTexture(id, alphaMode);
}

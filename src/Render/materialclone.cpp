//============================================================================
//  0x6F505130 - a private copy of a material, out of the
//  TAllocatedHandleObject<CMaterial,512> pool at unk_6FAB8268.
//
//  The empty layer array is built first and then assigned from the source's,
//  which is why the shipped stream has three zero stores and a self-compare
//  the optimiser could not fold away: `m_layers` is default-constructed and
//  then `Assign`ed, and the `if (&m_layers != &src->m_layers)` in front of
//  the assignment is the copy-assignment guard the source spells out.
//============================================================================
#include "material.h"
#include "refcnt.h"

CMaterial* __stdcall CloneMaterial(const CMaterial* src)
{
    CMaterial* material =
        (CMaterial*)g_materialPool.Alloc(0, kHMaterial, -2);
    if (material == 0)
        return 0;

    material->m_refcount = 0;
    material->m_vtable = (void*)&g_vftCMaterial;

    material->m_layers.m_alloc = 0;
    material->m_layers.m_count = 0;
    material->m_layers.m_data = 0;
    material->m_shared = 0;

    material->m_shared =
        (CMaterialShared*)((TRefCnt*)src->m_shared)->AddRefOrNull();

    if (&material->m_layers != &src->m_layers)
        material->m_layers.Assign(src->m_layers.m_count, src->m_layers.m_data);

    return (CMaterial*)((TRefCnt*)material)->AddRef(kHMaterial);
}

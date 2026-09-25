//============================================================================
//  NIpse::CBaseMap<NIpse::CPmCell>'s own module (0x6F49F370-0x6F49F6B2) -
//  vtable slots 0/1/2/3/4/9/10. See basemap.h for the class layout. Same
//  shapes as CBaseMap<CPaCell>'s own module (basemap_cpacell.cpp),
//  independently compiled - the one real difference is InitGrid's own
//  per-cell default value: a single all-`0xFFFFFF` dword here versus
//  CPaCell's own 8-byte all-zero record (see basemap.h's own
//  AllocateCells_6F49EC90 comment).
//============================================================================
#include "basemap.h"
#include "storm.h"   // SMemFree (Storm_403)
#include "agentbasechain_staticpools.h"   // BaseMapCPmCell_GetStaticPool - own TU, see its own comment

namespace NIpse {

//  0x6F49F370 - vtable slot 0.
const char* CBaseMap_CPmCell::GetTypeName()
{
    return "BaseMap";
}

//  0x6F49F3E0 - vtable slot 1.
void CBaseMap_CPmCell::ReturnToStaticPool()
{
    void* pool = BaseMapCPmCell_GetStaticPool();

    char* self = (char*)this;
    void* prev = *(void**)(self + 4);
    void* next = *(void**)(self + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(self + 4) = 0;
    *(void**)(self + 8) = 0;

    struct SPoolHeader { char m_reserved00[0x14]; void* m_freeListHead; unsigned int m_freeCount; };
    SPoolHeader* p = (SPoolHeader*)pool;
    void* node = self - 4;
    *(void**)node = p->m_freeListHead;
    p->m_freeCount -= 1;
    p->m_freeListHead = node;
}

//  0x6F49F460 - vtable slot 2, scalar deleting destructor.
void* CBaseMap_CPmCell::ScalarDeletingDestructor(unsigned int flags)
{
    ((SBaseMapRealDtorTarget*)this)->RealDtor_6F49F190();

    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//  0x6F49F5D0 - vtable slot 3 (+0x0C).
void CBaseMap_CPmCell::InitGrid(SHandleRegistrationRequest* request)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(request);

    unsigned int width  = *(unsigned int*)((char*)request + 0x10);
    m_width = width;
    unsigned int height = *(unsigned int*)((char*)request + 0x14);
    unsigned int area = width * height;
    m_height = height;

    unsigned int sentinelDefault = 0xFFFFFF;
    m_cellCount = area;
    Cells()->AllocateCells_6F49EC90(&sentinelDefault, area);

    CFloat widthF = CFloatFromInt(m_width);
    CFloat heightF = CFloatFromInt(m_height);

    m_originX = g_CFloatZero;
    m_originY = g_CFloatZero;
    m_extentY = heightF;
    m_extentX = widthF;

    unsigned int heightInt = m_height;
    unsigned int widthInt  = m_width;
    m_reserved54 = 0;
    m_reserved58 = 0;
    m_heightInt = heightInt;
    m_widthInt  = widthInt;
    m_scaleY = g_slopeThreshold1;
    m_scaleX = g_slopeThreshold1;
}

//  0x6F49F670 - vtable slot 4 (+0x10).
void CBaseMap_CPmCell::Teardown(int unused)
{
    //  `cells` cached once - see CBaseMap_CPaCell::Teardown's own comment
    //  (basemap_cpacell.cpp) for why.
    SBaseMapCellStorage* cells = Cells();
    unsigned int count = m_liveCellCount38;
    if (count != 0)
        cells->ShiftCells_6F49ED10(0, count);

    m_cellCount = 0;
    cells->ResetCells_6F49EAB0();

    m_height = 0;
    m_width = 0;

    ((SHandleTableEntry*)this)->UnregisterAndNotify(unused);
}

//  0x6F49F380 / 0x6F49F390 - vtable slots 9/10 (+0x24/+0x28), no-ops.
void SBaseMapNoOpArg::NoOp_6F49F380(void*) {}
void SBaseMapNoOpArg::NoOp_6F49F390(void*) {}

}  // namespace NIpse

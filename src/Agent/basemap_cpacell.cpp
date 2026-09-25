//============================================================================
//  NIpse::CBaseMap<NIpse::CPaCell>'s own module (0x6F484770-0x6F487FB2) -
//  vtable slots 0/1/2/3/4/9/10. See basemap.h for the class layout and the
//  inheritance evidence (CAcceleratorMap reuses slots 3/4 unchanged - see
//  acceleratormap.cpp).
//============================================================================
#include "basemap.h"
#include "storm.h"   // SMemFree (Storm_403)
#include "agentbasechain_staticpools.h"   // BaseMapCPaCell_GetStaticPool - own TU, see its own comment

namespace NIpse {

//  0x6F484770 - vtable slot 0.
const char* CBaseMap_CPaCell::GetTypeName()
{
    return "BaseMap";
}

//  0x6F4848F0 - vtable slot 1. Same unlink-then-return-to-pool shape as
//  every other ReturnToStaticPool in this base chain
//  (Agent/presence_returntopool.cpp) - here it pushes onto a null pool,
//  which is dead in practice (see BaseMapCPaCell_GetStaticPool's own
//  comment) but reproduced exactly as the dump has it regardless.
void CBaseMap_CPaCell::ReturnToStaticPool()
{
    void* pool = BaseMapCPaCell_GetStaticPool();

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

//  0x6F487D70 - vtable slot 2, scalar deleting destructor.
void* CBaseMap_CPaCell::ScalarDeletingDestructor(unsigned int flags)
{
    ((SBaseMapRealDtorTarget*)this)->RealDtor_6F486A40();

    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//  0x6F487ED0 - vtable slot 3 (+0x0C). See basemap.h's own declaration
//  comment - shared unchanged by NIpse::CAcceleratorMap.
void CBaseMap_CPaCell::InitGrid(SHandleRegistrationRequest* request)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(request);

    unsigned int width  = *(unsigned int*)((char*)request + 0x10);
    m_width = width;
    unsigned int height = *(unsigned int*)((char*)request + 0x14);
    unsigned int area = width * height;
    m_height = height;

    unsigned int zeroedDefault[2] = { 0, 0 };
    m_cellCount = area;
    Cells()->AllocateCells_6F484810(zeroedDefault, area);

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

//  0x6F487F70 - vtable slot 4 (+0x10). See basemap.h's own declaration
//  comment - shared unchanged by NIpse::CAcceleratorMap.
void CBaseMap_CPaCell::Teardown(int unused)
{
    //  `cells` cached in one local rather than calling Cells() twice: the
    //  dump computes `this+0x1C` exactly once, unconditionally, before the
    //  branch (into a callee-saved register reused for both calls below) -
    //  matching that shape needs one source-level pointer, not two
    //  textually-identical-but-separate `Cells()` calls.
    SBaseMapCellStorage* cells = Cells();
    unsigned int count = m_liveCellCount38;
    if (count != 0)
        cells->ShiftCells_6F484890(0, count);

    m_cellCount = 0;
    cells->ResetCells_6F4847E0();

    m_height = 0;
    m_width = 0;

    ((SHandleTableEntry*)this)->UnregisterAndNotify(unused);
}

//  0x6F484780 / 0x6F484790 - vtable slots 9/10 (+0x24/+0x28), no-ops.
void SBaseMapNoOpArg::NoOp_6F484780(void*) {}
void SBaseMapNoOpArg::NoOp_6F484790(void*) {}

}  // namespace NIpse

//============================================================================
//  0x6F28C830 - MakeUnitQuery: hand out the next query record from the
//  global pool, making one more when the pool has run out.
//
//  g_unitQueryDepth is the same counter BuildAndRegisterEventBinding
//  decrements on its way out (agenteventbinding.cpp) - so it is a *stack*
//  depth, not a high-water mark, and the pool is a stack of re-entrant
//  scratch records: the Nth nested enumeration gets the Nth record and
//  gives it back by decrementing.  Records are never destroyed.
//
//  The __FILE__/__LINE__ pair handed to Storm is
//  "e:\drive1\temp\buildwar3x\war3\source\unit\CUnit_Enum.cpp", 184, read
//  straight out of the image (`ida_query bytes 0x6F931B18`), and the size
//  is 0x94 - which is what fixes SUnitQuery's own length.
//
//  Own translation unit: it calls the record's constructor and the pool
//  array's Grow() for real.
//============================================================================
#include "agenteventbinding.h"
#include "storm.h"

static const char kCUnitEnumCpp[] =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\unit\\CUnit_Enum.cpp";

//  dword_6FAB4270 / dword_6FAB4494 - declared, never defined here: both are
//  real globals in the shipped image and build_mix.py binds them by address
//  (tools/link_check.py's fifth check is exactly this).
extern unsigned int g_unitQueryDepth;                   // dword_6FAB4270
extern TSGrowableArray<SUnitQuery*> g_unitQueryPool;    // dword_6FAB4494

SUnitQuery* MakeUnitQuery()
{
    //  The *slow* path written first, so it is the fall-through: the
    //  shipped code branches forward to the pool hit and puts it at the
    //  tail of the function.  Written the other way round the two blocks
    //  swap and the fast path's own increment changes shape with them
    //  (measured, 22 of 29 against 29).
    unsigned int depth = g_unitQueryDepth;

    //  Tested `>=` and not `<`: the shipped code lays the make-one-more arm
    //  out as the fallthrough and branches over it to the pool hit, which
    //  is the layout this polarity produces and the opposite one does not.
    if (depth >= g_unitQueryPool.m_count)
    {
        //  Placement new over Storm's block, for the reason
        //  src/Unit/floatlistener.h records: it is what produces the
        //  shipped `test eax,eax` / construct / `mov esi,eax` / `jmp` over
        //  an `xor esi,esi` rather than one pointer held in a register
        //  throughout.
        SUnitQuery* made = new (SMemAlloc(sizeof(SUnitQuery), kCUnitEnumCpp,
                                          184, 0)) SUnitQuery();

        *g_unitQueryPool.Grow() = made;
        g_unitQueryDepth++;

        return made;
    }

    g_unitQueryDepth = depth + 1;
    return g_unitQueryPool.m_data[depth];
}

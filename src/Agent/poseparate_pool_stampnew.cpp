//============================================================================
//  0x6F495AD0 - NIpse::SPoSeparatePool::StampNew. See poseparate_pool.h -
//  own translation unit so it stays a real `call` into
//  SPoSeparatePool::Allocate rather than being inlined into it.
//============================================================================
#include "poseparate_pool.h"

namespace NIpse {

CPoSeparate* SPoSeparatePool::StampNew(unsigned int a, unsigned int b)
{
    CPoSeparate* obj = Allocate(0);
    char* base = (char*)obj;
    *(unsigned int*)(base + 0xC) = 0x5E70726F;
    *(unsigned int*)(base + 0x10) = 0x60706373;
    *(unsigned int*)(base + 4) = a;
    *(unsigned int*)(base + 8) = b;
    ++m_liveCount;
    return obj;
}

}  // namespace NIpse

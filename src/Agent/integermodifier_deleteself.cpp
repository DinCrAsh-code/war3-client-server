//============================================================================
//  0x6F472FD0 - IntegerModifier's own slot-1 override, the scalar deleting
//  destructor. ~IntegerModifier() is implicit and trivial, so it collapses
//  into CModifier's own base destructor one level up - same shape as every
//  other level of this chain.
//
//  Own translation unit - same "keep the destructor call a real out-of-line
//  call" reasoning as the rest of this chain.
//============================================================================
#include "integermodifier.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

IntegerModifier* IntegerModifier::DeleteSelf(int flags)
{
    this->IntegerModifier::~IntegerModifier();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

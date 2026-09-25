//============================================================================
//  0x6F314B30 - TSGrowableArray<NTempest::CIMVector>::~TSGrowableArray().
//  IDA's own label for the RTTI tag string this passes to Storm,
//  `aAvcimvectorNte`, is what a 15-character label truncation of
//  ".?AVCIMVector@NTempest@@" looks like - the class-type-descriptor
//  encoding storm.h's own header comment explains for the AGILE_TYPE
//  relationship list.  That names the element type as
//  `NTempest::CIMVector`; nothing in this call tree reads through one, so
//  its members stay an opaque reserved block sized only by a guess.
//============================================================================
#include "tsarray.inl"

namespace NTempest
{
//  Layout unknown - nothing in this call tree touches a CIMVector's own
//  members, only the array that holds them.  Sized arbitrarily; the
//  destructor's instructions do not depend on the size, only on the type
//  being trivially destructible.
struct CIMVector
{
    char m_reservedUnknown[12];
};
}

template TSGrowableArray<NTempest::CIMVector>::~TSGrowableArray();

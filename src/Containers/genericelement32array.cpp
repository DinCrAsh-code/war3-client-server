//============================================================================
//  0x6F6F54D0 - `TSGrowableArray<T>::ComputeChunk` (Containers/tsarray.inl)
//  for a 32-byte element: `256 / sizeof(T) == 8` is exactly this dump's own
//  `cmp eax,8` cap and its `mov dword ptr [ecx+0Ch],8` store into m_chunk.
//  Nothing in this closure identifies T by name or by any field it touches,
//  so it stays an opaque same-size stand-in - only `sizeof` needs to be
//  right for this member's own instruction stream.
//============================================================================
#include "tsarray.inl"

struct SGenericElement32
{
    char m_reserved00[0x20];
};

template unsigned int TSGrowableArray<SGenericElement32>::ComputeChunk(unsigned int);

//============================================================================
//  Naked real-signature thunks for proximitymap.h/proximitymap.cpp's own
//  declared-but-not-defined out-of-scope callees - same seam and same fix
//  as basemap_outofscope_thunks.cpp's own header comment.
//============================================================================
#include "proximitymap.h"

namespace NIpse {

__declspec(naked) void SProximityMapSubContainer::Resize_6F49ED70(const void*, unsigned int)
{
    __asm { mov eax, 06F49ED70h
            jmp eax }
}

__declspec(naked) int SProximityMapSubContainer::Shift_6F49EED0(unsigned int, unsigned int)
{
    __asm { mov eax, 06F49EED0h
            jmp eax }
}

__declspec(naked) int SProximityMapSubContainer::Shift_6F49EDF0(unsigned int, unsigned int)
{
    __asm { mov eax, 06F49EDF0h
            jmp eax }
}

__declspec(naked) void SProximityMapSubContainer::Reset_6F49EBD0()
{
    __asm { mov eax, 06F49EBD0h
            jmp eax }
}

__declspec(naked) void SProximityMapSubContainer::Reset_6F49EB40()
{
    __asm { mov eax, 06F49EB40h
            jmp eax }
}

__declspec(naked) void* SProximityMapRegistry::Register_6F4A0460(void*, void*, void*)
{
    __asm { mov eax, 06F4A0460h
            jmp eax }
}

__declspec(naked) void SProximityMapTeardownPrelude::Flush_6F49F260()
{
    __asm { mov eax, 06F49F260h
            jmp eax }
}

}  // namespace NIpse

//  SProximityMapRealDtorTarget - declared file-scope in proximitymap.cpp
//  itself (not in proximitymap.h), token-identical redeclaration here so
//  this TU can define its one member - see proximitymap.cpp's own comment
//  on why this struct isn't local to a function.
namespace NIpse {
struct SProximityMapRealDtorTarget { void* RealDtor_6F49FC30(); };

__declspec(naked) void* SProximityMapRealDtorTarget::RealDtor_6F49FC30()
{
    __asm { mov eax, 06F49FC30h
            jmp eax }
}

}  // namespace NIpse

//============================================================================
//  Generator: the pure-abstract interface every InstanceGenerator<T> (the
//  factory singleton behind each CTaskXxx/CJassFunc-style pooled class)
//  implements.  `??_7Generator@@6B@` @ 0x6F874F60
//  (agent_worktrees/classes/0x6F874F60.json) - three slots, all still
//  `_purecall` (0x6F7E10C2) in the live database, so there is nothing to
//  reconstruct beyond the shape itself: no class in the currently-reachable
//  data derives from `Generator` and overrides it with `virtual` here (every
//  InstanceGenerator<T> found so far - see ctaskinstancegenerator.h - is an
//  ordinary struct whose three "slots" are plain functions written at the
//  vtable's own addresses, not C++ `virtual` overrides, for this repo's
//  usual "nothing calls through this vtable indirectly" reason), so this
//  header exists purely to document the interface shape and give a reader
//  something to point at.  No .cpp: an abstract class with only pure
//  virtuals has no out-of-line bodies of its own to write.
//
//  Slot signatures inferred from InstanceGenerator<CTask>'s own three real
//  bodies (ctaskinstancegenerator.h/taskinstancegenerator.cpp): slot 0
//  returns a freshly allocated+constructed instance with no arguments
//  (`retn 0`), slot 1 takes the instance to tear down and cleans one stack
//  dword (`retn 4`), slot 2 takes none and cleans none (`retn 0`).
//============================================================================
#ifndef GENERATOR_H
#define GENERATOR_H

class Generator
{
public:
    //  slot 0 / +0x00 - allocate and construct a fresh pooled instance.
    virtual void* CreateInstance() = 0;
    //  slot 1 / +0x04 - tear down and free one previously created instance.
    virtual void DestroyInstance(void* instance) = 0;
    //  slot 2 / +0x08 - release every block the pool has ever grown by,
    //  back to Storm.
    virtual void ReleaseAll() = 0;
};

#endif

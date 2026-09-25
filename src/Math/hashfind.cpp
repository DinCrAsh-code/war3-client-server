//============================================================================
//  0x6F4D05B0, 0x6F4D1D30 - two name lookups, byte-identical bodies at two
//  addresses.
//
//  Scored against the copies of these two functions that appear in
//  `asm/processed/sub_6F020D30_0x6F020D30_calltree_asm.md`, not against the
//  chunk dump the original write-up used (`asm/part_0x6f4d05b0.md`, which was
//  worked in a separate workspace and never merged back here).  57/59 either
//  way; see docs/targets/sub_6F4D05B0.md.
//
//  Deliberately self-contained, i.e. not built on `src/storm.h`.  An earlier
//  merge-back replaced the canonical `src/storm.h` with a carved-down copy of
//  it that renamed `TSHashTable<T, K>` to `TSHashTable<T>`, which broke every
//  other target in the repo.  The container view this subsystem needs
//  therefore lives in `src/config.h` under its own names rather than competing
//  with the real one: nothing else includes it, and the canonical containers
//  in `storm.h` stay the single definition every verified target compiles
//  against.
//
//  Only the members the lookup actually reads are reproduced, at the offsets
//  AgileTypeIsDerivedFrom recovered:
//
//      node    +0x00 hash value, +0x04 bucket link, +0x14 key
//      table   +0x1C bucket array data, +0x24 mask
//
//  and everything between is a reserved block, not an invented layout.
//
//  The two instantiations were called CNameHashTable<HashKeyNodeA> and
//  <HashKeyNodeB> until the sub_6F020D30 target chained them at 0x6F4D1E60
//  and showed what they hold: a file's table of sections, and a section's
//  table of entries.  Nothing else about them changed.
//============================================================================
#include "config.h"

//----------------------------------------------------------------------------
//  The lookup itself.  Hashes the name, then walks the bucket from the back
//  through m_prevlink - hence `(int)node > 0` rather than `node != 0`: Prev()
//  hands back the field as it stands, and the value the walk ends on is the
//  terminator's complemented address, not null.
//
//  The key test is a pointer compare first and a string compare only if that
//  misses, which is how interned names cost one compare in the common case.
//
//  Defined here and nowhere else, and explicitly instantiated at the bottom:
//  every other translation unit sees only the declaration in config.h and so
//  has to emit the call the shipped code has.
//----------------------------------------------------------------------------
template <class T>
T* CNameHashTable<T>::Ptr(const char* name) const
{
    if (m_mask == 0xFFFFFFFF)
        return 0;

    unsigned int hashval = Storm_590(name);

    T* node = m_buckets[hashval & m_mask].Tail();
    while ((int)node > 0)
    {
        if (node->m_hashval == hashval &&
            (node->m_key == name ||
             SStrnICmp(node->m_key, name, 0x7FFFFFFF) == 0))
            return node;
        node = m_buckets[hashval & m_mask].Prev(node);
    }
    return 0;
}

template CConfigSection* CNameHashTable<CConfigSection>::Ptr(const char*) const;
template CConfigEntry*   CNameHashTable<CConfigEntry>::Ptr(const char*) const;

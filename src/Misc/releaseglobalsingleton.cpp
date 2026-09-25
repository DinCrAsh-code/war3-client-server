//============================================================================
//  0x6F62C060 - ReleaseGlobalSingletonB3 (gamemain-batch-3).  The
//  "release-if-last" shape docs/msvc-vc8-idioms.md's `CAgentPtr`/
//  `AssignChecked` note already documents: drop dword_6FACEB48's own
//  refcount at +4, and only when it reaches zero call the object's own
//  vtable slot 0 (the shipped code re-reads the refcount off memory for the
//  zero test rather than trusting the `add`'s flags, which is why this is
//  written as a real `--refcount == 0` rather than something that could
//  fold the subtract and the compare together).  Always clears the global
//  to null afterward, whether or not anything was released.
//
//  The real class is not otherwise reached by this call tree - only its
//  vtable slot 0 and its own refcount field are ever touched - so it is
//  modeled as the minimal two-word shape rather than named.
//============================================================================

struct RefCountedSingletonB3
{
    void* m_vtable;    // +0x00
    int   m_refcount;  // +0x04
};

extern RefCountedSingletonB3* g_globalSingletonB3;   // dword_6FACEB48

void ReleaseGlobalSingletonB3()
{
    RefCountedSingletonB3* obj = g_globalSingletonB3;
    if (obj)
    {
        if (--obj->m_refcount == 0)
        {
            typedef void (__thiscall *ReleaseFn)(RefCountedSingletonB3*);
            (*(ReleaseFn**)obj)[0](obj);
        }
    }

    g_globalSingletonB3 = 0;
}

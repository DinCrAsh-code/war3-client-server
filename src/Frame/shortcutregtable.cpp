//============================================================================
//  0x6F600430 - SHORTCUTREGTABLE::SHORTCUTREGTABLE, the constructor of the
//  keyboard-shortcut registry every CLayer carries at +0x80.
//
//  The shipped class is `TSHashTable<SHORTCUTREGNODE, HASHKEY_NONE>` - this
//  body stamps that exact vtable - and clayer.h says why it is modelled as a
//  plain struct instead of as an instantiation of storm.h's template.
//
//  `m_list.SetLinkOffset(0xC)` is the whole point of the body: the list is
//  default-constructed with Storm's poison offset and this is what replaces
//  it, draining the (empty) list on the way past.
//============================================================================
#include "frame.h"
#include "framethunks.h"

SHORTCUTREGTABLE::SHORTCUTREGTABLE()
    : m_vtable((void*)kShortcutRegTableVftable)
{
    m_buckets.m_alloc = 0;
    m_buckets.m_count = 0;
    m_buckets.m_data = 0;
    m_buckets.m_chunk = 0;
    m_probes = 0;
    m_list.SetLinkOffset(0x0C);
    m_mask = 0xFFFFFFFFu;
}

//  0x6F5FE450 - `retn 4`, thiscall + one stack bool.  Per-bucket
//  TailLink()/Link()/Unlink() walk when freeNodes != 0 (the same shape
//  TSExplicitList<T>::UnlinkAll() itself uses), or a raw call through this
//  table's own vtable slot 0 when freeNodes == 0 - the same "detach vs.
//  destroy" fork Render/facechartable.cpp's own CHARCODEDESC_TABLE::Clear
//  documents in full.
void SHORTCUTREGTABLE::Clear(unsigned int freeNodes)
{
    m_probes = 0;
    m_list.UnlinkAll();

    typedef void (__thiscall *FreeNodeFn)(void*, void*);

    for (unsigned int i = 0; i < m_buckets.m_count; i++)
    {
        TSExplicitList<SHORTCUTREGNODE>& bucket = m_buckets[i];
        for (;;)
        {
            int tail = bucket.TailLink();
            if (tail <= 0)
                break;
            if (freeNodes)
                bucket.Link((SHORTCUTREGNODE*)tail)->Unlink();
            else
                ((FreeNodeFn)(*(void***)this)[0])(this, (void*)tail);
        }
    }
}

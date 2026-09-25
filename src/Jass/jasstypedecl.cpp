//============================================================================
//  `.\Jass.cpp` - the two "declare a type" primitives above JassTypeTable /
//  JassTypeTable2 / JassTypeIdTable.
//============================================================================
#include "jasssymtables.h"
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F45D010 - declare `childName` as a child entry of the type node
//  already declared as `parentName`, threading it onto the head of that
//  parent's own child list.  The child-name lookup's own return value is
//  never read - a real call (Storm_590 runs inside it) with an observably
//  discarded result, on the return side of the same "the call still
//  happens, only the answer goes unused" shape Containers/tshash.inl's
//  PtrHashOnly documents for an unread *argument*.
//
//  `this` is not resolved beyond the one field this closure touches - the
//  JassTypeTable embedded at +0x286C in whatever larger object (almost
//  certainly JassThreadLocal) owns it.
//----------------------------------------------------------------------------
//  Every one of these is a *pointer* field (0x6F45D010/0x6F45D300 both load
//  through it with `mov reg,[this+OFFSET]`, never `lea`), so this scope
//  object owns four separately-allocated tables rather than embedding them.
struct JASSTYPESCOPE
{
    char             m_reserved00[0x286C]; // +0x0000  unread in this closure
    JassTypeTable*   m_types;              // +0x286C
    char             m_reserved2870[0x10]; // +0x2870  unread in this closure
    JASSCTORLIST*    m_ctors;              // +0x2880
    JassTypeTable2*  m_types2;             // +0x2884
    char             m_reserved2888[0x0C]; // +0x2888  unread in this closure
    JassTypeIdTable* m_typeIds;            // +0x2894

    void DeclareChildType(const char* childName, const char* parentName);
    void DeclareCtorOrAlias(const char* name, void* value, void* unused);
};

void JASSTYPESCOPE::DeclareChildType(const char* childName,
                                     const char* parentName)
{
    JASSTYPENODE* parent = m_types->PtrInterned(parentName);

    m_types->PtrInterned(childName);   // return value unread - see note above

    unsigned int hash = Storm_590(childName);
    JASSTYPENODE* child = m_types->NewNode(hash, 0, 0);
    child->m_hashval = hash;
    child->m_key = childName;

    child->m_parent = parent;
    child->m_nextSibling = parent->m_firstChild;
    parent->m_firstChild = child;
}

//----------------------------------------------------------------------------
//  0x6F45D300 - declare `name`.  The special case is JASS's own implicit
//  type constructor, "<init>": that name goes on `m_ctors`' plain list
//  instead of either type table, one raw-allocated node per call with no
//  de-duplication at all (every "<init>" declared links a fresh node onto
//  the same head).  Anything else is declared once into `m_types2` (an
//  interned lookup first, so re-declaring the same name is a no-op) and
//  its type-id reverse mapping is looked up in `m_typeIds` - keyed on the
//  raw id `value` already is a real call whose key argument is provably
//  unread (Containers/allochashtable.h's own PtrHashOnly), so the constant
//  it is called with here is not otherwise meaningful.
//----------------------------------------------------------------------------
void JASSTYPESCOPE::DeclareCtorOrAlias(const char* name, void* value,
                                       void* arg8)
{
    if (Storm_508(name, "<init>", 0x7FFFFFFF) == 0)
    {
        JASSCTORNODE* node =
            (JASSCTORNODE*)SMemAlloc(sizeof(JASSCTORNODE),
                                     "AuInitFuncAddr", -2, 8);
        if (node != 0)
        {
            node->m_link.m_next = 0;
            node->m_link.m_prevlink = 0;
        }

        m_ctors->m_list.LinkToHead(node);
        node->m_value = value;
        return;
    }

    JASSTYPENODE2* node = m_types2->PtrInterned(name);

    if (node == 0)
    {
        unsigned int hash = Storm_590(name);
        node = m_types2->NewNode(hash, 0, 0);
        node->m_hashval = hash;
        node->m_key = name;

        //  A real call whose own key argument is provably unread - see
        //  Containers/allochashtable.h's own PtrHashOnly note.
        m_typeIds->PtrHashOnly((unsigned int)value, 0);

        JASSTYPEIDNODE* idNode = m_typeIds->NewNode((unsigned int)value, 0, 0);
        idNode->m_hashval = (unsigned int)value;
        ((STStringField*)idNode->m_ctorSub)->Assign(name);
    }

    node->m_value = value;
    node->m_extra1C = arg8;
}

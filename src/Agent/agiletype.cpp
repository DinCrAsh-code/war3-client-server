//============================================================================
//  "is this type derived from that one", and the memo table behind it.
//============================================================================
#include "game.h"

typedef AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP REL;

//----------------------------------------------------------------------------
//  0x6F471840 - answer the question for one type, remembering the answer.
//
//  The registry lookup's result is dereferenced without a null check: the
//  shipped code does that too, so a type id that is not registered is a
//  crash, not a `false`.
//----------------------------------------------------------------------------
int __fastcall CGameData::IsTypeDerivedFrom(unsigned int type, unsigned int base)
{
    if (type == 0)
        return 0;

    if (base == kAgileRootTypeId || type == base)
        return 1;

    AGILE_TYPE_ID typekey(type);
    unsigned int typehash = typekey.Hash();
    AGILE_TYPE_DATA* data = m_types.Ptr(typehash, typekey);

    if (data->m_parent == base)
        return 1;

    AGILE_TYPE_ID basekey(base);
    unsigned int hashval = basekey.Hash();

    REL* relationship = data->m_relations.Ptr(hashval, basekey);
    if (relationship == 0)
    {
        relationship = data->m_relations.New(hashval, basekey, 0, 0);
        relationship->m_derived = IsTypeDerivedFrom(data->m_parent, base);
    }

    return relationship->m_derived;
}

//----------------------------------------------------------------------------
//  0x6F471910 - the entry point.  The two cheap answers are given here as
//  well as inside the memoising version, so that asking whether a type is
//  derived from itself never touches the table.
//----------------------------------------------------------------------------
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base)
{
    if (base == kAgileRootTypeId || type == base)
        return 1;

    return g_pGameData->IsTypeDerivedFrom(type, base);
}

//----------------------------------------------------------------------------
//  0x6F471BC0 - a smart-pointer assignment's "what type must this be"
//  answer, reached from sub_6F022AA0's type-checked assignment operator.
//  It is a genuine out-of-line call rather than the constant itself because
//  every other instantiation of the same assignment operator asks a
//  different type-check function here; this is the one instantiated for the
//  base/unconstrained pointer, whose answer is always the root type.
//  (sub_6F022AA0 itself is not reconstructed in this pass, so nothing beyond
//  this leaf is claimed about the smart pointer template it belongs to.)
//----------------------------------------------------------------------------
unsigned int GetAgileRootTypeId()
{
    return kAgileRootTypeId;
}

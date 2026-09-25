//============================================================================
//  0x6F268DF0 - look a destructable type record up by its FourCC id in the
//  shared registry (g_destructableTypeTable, Unit/destructabletypedata.h -
//  the same TSHashTable<CDestructableTypeData, AGILE_TYPE_ID>::Ptr
//  instantiation destructabletype_table.cpp's own lookup family already
//  uses) and hand back the JASS handle embedded at the record's own +0xB4
//  (GetHandleOrZeroAlias, already real - Agent/handleobject.cpp), or 0 when
//  the type is not registered at all.
//
//  Reached from the CSelectable::RefreshSelectionHighlight subtree this
//  batch shares with CItem (SSelectionVisualDetail::ReleasePreviousTarget,
//  0x6F35B970, Widget/selectionvisualreleasesub.cpp) - ordinary
//  rendering/selection infrastructure, not CItem-specific.
//
//  +0xB4 is past every field destructabletypedata.h's own record currently
//  names (its highest, m_reservedA0, ends at +0xA4); nothing else in this
//  call tree reads between there and +0xB4, so that gap stays unnamed here
//  too - only the touched field gets a name, per CLAUDE.md.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "destructabletypedata.h"
#include "handleobject.h"

unsigned int __fastcall GetDestructableTypeHandleFieldB4(unsigned int typeId)
{
    AGILE_TYPE_ID key(typeId);
    CDestructableTypeData* record = g_destructableTypeTable.Ptr(key.Hash(), key);
    if (record != 0)
        return GetHandleOrZeroAlias((const SHandleHolder*)((char*)record + 0xB4));

    return 0;
}

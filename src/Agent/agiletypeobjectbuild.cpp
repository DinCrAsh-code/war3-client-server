//============================================================================
//  0x6F471F40 - build the per-type object a registered agile type parks at
//  +0x6C (QueryAgileTypeObject, agiletypequery.cpp, calls it on first use).
//
//  Was a `mov eax,<address> / jmp eax` redirect; it is twenty instructions
//  that fill in the same 0x2C-byte request block CWidget's slot 68 builds
//  (widgetagentquery.h) and hand it to the same submitter
//  (SubmitWidgetAgentQuery, 0x6F48AB80, widgetagentsubmit.cpp), with both
//  "run the extra slot" flags set.
//
//  Two things differ from slot 68's block and both are in the shipped
//  code:
//
//   * the tag and the type id are the *same* constant, 0x5E616466 - the
//     four bytes 'f' 'd' 'a' '^', the same '^'-suffixed four-character tag
//     family agiletype.h describes, written twice from one register
//     (`mov eax,5E616466h` then two stores);
//   * +0x24 is -2, not the -1 the other builder writes there, and it is
//     stored *after* +0x28 (`mov var_4,-1` then `mov var_8,-2`), which is
//     why the two assignments below are in that order.
//
//  Own translation unit: it reaches the submitter with a real call, and
//  QueryAgileTypeObject reaches this with one.
//============================================================================
#include "widgetagentquery.h"

void* BuildAgileTypeObject()
{
    SWidgetAgentQuery query;

    query.m_tag = 0x5E616466;
    query.m_typeId = 0x5E616466;
    query.m_reserved08 = 0;
    query.m_pTypeObject = 0;
    query.m_pPool = 0;
    query.m_reserved14 = 0;
    query.m_reserved18 = 0;
    query.m_reserved1C = 0;
    query.m_reserved20 = 0;
    query.m_reserved28 = 0xFFFFFFFF;
    query.m_reserved24 = 0xFFFFFFFE;

    return SubmitWidgetAgentQuery(&query, 1, 1);
}

//============================================================================
//  0x6F4F17B0 - the shared reference-counted node for this string, built the
//  first time it is asked for and reference-counted from then on.
//
//  The key assignment is HASHKEY_STR's own - keep the pointer when it is
//  already the identical one, otherwise free what is there and duplicate the
//  new string, both logged against Storm's template header at lines 0xA38
//  and 0xA39.  Frame/framereg.h spells that out once for the three
//  registries that share it; this module has the same block open-coded
//  rather than reaching for that header, because it is a different
//  subsystem and the shared inline would drag the frame registries in.
//
//  The tail loads edx with the "HSTRINGREF" tag before jumping into
//  TRefCnt::AddRef, and that instruction is reachable now: AddRef is a
//  __fastcall member taking the tag as its one argument, so `this` stays in
//  ecx, the tag lands in edx and the member still cleans nothing.  This
//  comment used to say the instruction could not be reproduced, which was
//  true only for as long as refcnt.h declared AddRef with no parameter.
//
//  Own translation unit: four real calls out of it.
//============================================================================
#include "stringref.h"

unsigned int __stdcall Storm_590(const char* s);
char* __stdcall Storm_507(const char* text, const char* logfilename,
                          int logline);

//: The file/line pair HASHKEY_STR's assignment logs under.  Spelled out and
//: not declared as an extern: the shipped operand is a string constant with
//: no address-shaped IDA name to bind, and the canonicaliser compares both
//: spellings as SYM.
static const char kStormHashTemplateFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h";

TRefCnt* __fastcall AcquireStringRef(const char* text)
{
    SStringRef* node = g_stringRefs.Ptr(text);

    if (node == 0)
    {
        unsigned int hash = Storm_590(text);

        node = g_stringRefs.NewNode(hash, 0, 0);
        node->m_hashval = hash;

        if (node->m_key != text)
        {
            if (node->m_key != 0)
                SMemFree((void*)node->m_key, kStormHashTemplateFile, 0xA38, 0);

            node->m_key = Storm_507(text, kStormHashTemplateFile, 0xA39);
        }
    }

    return node->AddRef("HSTRINGREF");
}

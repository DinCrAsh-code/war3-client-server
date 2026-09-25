//============================================================================
//  The three name registries Storm's frame library resolves an FDF name
//  through, and the two node types the create side of it stores.
//
//  All three are the same shipped class - `TSHashTable<T, HASHKEY_STR>` -
//  and the template arguments are not guessed: each table's constructor
//  stamps its own `??_7?$TSHashTable@U<Node>@@VHASHKEY_STR@@@@6B@`, which
//  `ida_query xrefs` reads straight out of the image.
//
//      0x6FACD264   TSHashTable<BASEFRAMEHASHNODE,HASHKEY_STR>
//      0x6FACD28C   TSHashTable<FRAMENAMECREATEHANDLER,HASHKEY_STR>
//      0x6FACD2B4   TSHashTable<SIMPLEFRAMENAMECREATEHANDLER,HASHKEY_STR>
//
//  The first is what a frame *definition* is looked up in - one node per
//  name in the loaded .fdf files, carrying the flags that say which of the
//  two create paths the name belongs to and the handler each path caches.
//  The other two map a name to the create handler for it, and are filled
//  once from a static table of {name, handler, context} triples.
//============================================================================
#ifndef FRAMEREG_H
#define FRAMEREG_H

#include "storm.h"

//  Never defined: HASHKEY_STR is only ever a template argument here, and
//  nothing this closure reaches touches a HASHKEY_STR's own members.  The
//  same treatment Config/gamestrings.h already gives it.
class HASHKEY_STR;

//----------------------------------------------------------------------------
//  One frame definition, by name.
//
//  The node carries a vftable, so TSHashObject's own header starts four
//  bytes in: the hash at +0x04, the bucket link at +0x08, the table-wide
//  link in the eight bytes after it and the key at +0x18 - which is exactly
//  what 0x6F5C7920's walk reads, and what makes HashLinkOffset() 8 here
//  against 4 for the two handler nodes below.
//
//  Only the fields the create paths touch are named.  +0x48 is the flag
//  word the two entry points test before committing to a path (bit 1 for
//  the frame path, bit 0x8000000 for the simple-frame one), +0x1C and +0x28
//  are what the handler lookup is keyed on, and +0x100 / +0x12C are where
//  each path caches the handler it resolved.
//----------------------------------------------------------------------------
struct BASEFRAMEHASHNODE
{
    void*                       m_vtable;           // +0x00
    unsigned int                m_hashval;          // +0x04
    TSLink<BASEFRAMEHASHNODE>   m_hashlink;         // +0x08
    char                        m_reserved10[0x08]; // +0x10
    const char*                 m_key;              // +0x18
    char                        m_typeName[0x0C];   // +0x1C - a string object
    void*                       m_createArg;        // +0x28
    void*                       m_createKind;       // +0x2C
    char                        m_reserved30[0x18]; // +0x30
    unsigned int                m_flags;            // +0x48
    char                        m_reserved4C[0xB4]; // +0x4C
    void*                       m_createHandler;    // +0x100
    char                        m_reserved104[0x28];// +0x104
    void*                       m_simpleCreateHandler;  // +0x12C
};

//----------------------------------------------------------------------------
//  A name -> create handler mapping, one per registered frame type.  A plain
//  TSHashObject: the hash at +0x00, the bucket link at +0x04, the
//  table-wide link after it and the key at +0x14, with the handler and the
//  context the registration table supplied at +0x18 and +0x1C.
//----------------------------------------------------------------------------
struct FRAMENAMECREATEHANDLER
{
    unsigned int                        m_hashval;          // +0x00
    TSLink<FRAMENAMECREATEHANDLER>      m_hashlink;         // +0x04
    char                                m_reserved0C[0x08]; // +0x0C
    const char*                         m_key;              // +0x14
    void*                               m_handler;          // +0x18
    void*                               m_context;          // +0x1C
};

struct SIMPLEFRAMENAMECREATEHANDLER
{
    unsigned int                            m_hashval;          // +0x00
    TSLink<SIMPLEFRAMENAMECREATEHANDLER>    m_hashlink;         // +0x04
    char                                    m_reserved0C[0x08]; // +0x0C
    const char*                             m_key;              // +0x14
    void*                                   m_handler;          // +0x18
    void*                                   m_context;          // +0x1C
};

//----------------------------------------------------------------------------
//  One row of either registration table: the shipped caller walks an array
//  of these three dwords and puts each one in the matching hash table.
//----------------------------------------------------------------------------
struct FRAMECREATEHANDLERENTRY
{
    const char* m_name;     // +0x00
    void*       m_handler;  // +0x04
    void*       m_context;  // +0x08
};

//----------------------------------------------------------------------------
//  The three tables.  Each is spelled as a concrete class deriving from the
//  instantiation rather than as a typedef for it, for the one reason
//  Agent/agiletype.h's AGILE_TYPE_TABLE already carries: this repo models
//  TSHashTable::AllocNode as pure, so the template itself is abstract and
//  an object of it cannot be declared.  Nothing is invented for the derived
//  class - it adds no member and defines neither virtual, both of which are
//  reached only through the vftable the shipped constructor stamps, and
//  that vftable names `TSHashTable<T,HASHKEY_STR>` itself.
//----------------------------------------------------------------------------
class CFrameDefTable : public TSHashTable<BASEFRAMEHASHNODE, HASHKEY_STR>
{
public:
    virtual ~CFrameDefTable();
    virtual BASEFRAMEHASHNODE* AllocNode(
        TSExplicitList<BASEFRAMEHASHNODE>* bucket, void* arg1, void* arg2);
};

class CFrameCreateHandlerTable
    : public TSHashTable<FRAMENAMECREATEHANDLER, HASHKEY_STR>
{
public:
    virtual ~CFrameCreateHandlerTable();
    virtual FRAMENAMECREATEHANDLER* AllocNode(
        TSExplicitList<FRAMENAMECREATEHANDLER>* bucket, void* arg1, void* arg2);
};

class CSimpleFrameCreateHandlerTable
    : public TSHashTable<SIMPLEFRAMENAMECREATEHANDLER, HASHKEY_STR>
{
public:
    virtual ~CSimpleFrameCreateHandlerTable();
    virtual SIMPLEFRAMENAMECREATEHANDLER* AllocNode(
        TSExplicitList<SIMPLEFRAMENAMECREATEHANDLER>* bucket,
        void* arg1, void* arg2);
};

//----------------------------------------------------------------------------
//  The create side, keyed on the definition rather than on the table: once a
//  name has resolved to a BASEFRAMEHASHNODE, each of the two paths goes on
//  through the node.  Both are genuine __fastcall members - the shipped call
//  sites leave the node in ecx *and* the parent in edx and push only what is
//  left - which is also why neither can be spelled __thiscall: that would
//  put the parent on the stack and clean four bytes too many.
//----------------------------------------------------------------------------
class CSimpleFrame;

//: The bit in BASEFRAMEHASHNODE::m_flags that says a definition belongs to
//: the simple-frame create path.
const unsigned int kFrameDefSimpleCreatable = 0x8000000;

//: 0x6F5C94D0 - Frame/framedefcreateinstance.cpp.  `retn 4`.
CSimpleFrame* __fastcall CreateSimpleFrameFromDef(BASEFRAMEHASHNODE* def,
                                                  CSimpleFrame* parent,
                                                  void* context);

//: The create handler a definition resolves to: one register argument (the
//: parent) and nothing pushed.
typedef CSimpleFrame* (__fastcall *SimpleFrameCreateFn)(CSimpleFrame* parent);

//: 0x6F5C93E0 - find the handler for `name` under `kind` and hand back the
//: context to pass it later.  `retn 4`.  Real body in
//: Frame/framecreatehandlerresolve.cpp.
SimpleFrameCreateFn __fastcall ResolveSimpleFrameCreateHandler(
        const char* name, void* kind, void** outContext);

//: 0x6F5C93A0 - ResolveSimpleFrameCreateHandler's own twin for the
//: non-simple-frame create path (CreateFrameFromDef, still a redirect - the
//: only caller in this closure).  Same shape; real body in
//: Frame/framecreatehandlerresolve.cpp.
void* __fastcall ResolveFrameCreateHandler(const char* name, void* kind,
                                           void** outContext);

//: 0x6F5CCC80 - push the definition's own properties into a freshly built
//: frame.  `retn 4`: a __fastcall member, the context in edx and the frame
//: pushed.  Still a redirect.
void __fastcall ApplyDefToSimpleFrame(BASEFRAMEHASHNODE* def, void* context,
                                      CSimpleFrame* frame);

//: unk_6FACD264 / unk_6FACD28C / unk_6FACD2B4 - the tables themselves and
//: not pointers to them: every shipped call site takes their address with
//: `mov ecx, offset ...`.
extern CFrameDefTable                   g_frameDefs;
extern CFrameCreateHandlerTable         g_frameCreateHandlers;
extern CSimpleFrameCreateHandlerTable   g_simpleFrameCreateHandlers;

//----------------------------------------------------------------------------
//  HASHKEY_STR's own assignment, which is what both registration loops and
//  0x6F4F17B0 spell out where they set a node's key: keep the pointer when
//  it is already the identical one, and otherwise free what is there and
//  duplicate the new string.  The file/line pair Storm logs the two
//  allocations under is its own template header - the string at 0x6F875C68
//  is "e:\Drive1\temp\buildwar3x\Storm\H\stpl.h", read with
//  `ida_query bytes` rather than guessed - and the two lines are 0xA38 for
//  the free and 0xA39 for the duplicate.
//----------------------------------------------------------------------------
//  Spelled out and not declared as an extern: the shipped operand is a
//  string constant with no address-shaped IDA name for build_mix.py's data
//  table to bind, and the canonicaliser compares both spellings as SYM.
#define kStormHashTemplateFile "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h"

char* __stdcall Storm_507(const char* text, const char* logfilename,
                          int logline);

inline void AssignHashKeyStr(const char** slot, const char* value)
{
    if (*slot != value)
    {
        if (*slot != 0)
            SMemFree((void*)*slot, kStormHashTemplateFile, 0xA38, 0);
        *slot = Storm_507(value, kStormHashTemplateFile, 0xA39);
    }
}

//  0x6F5CB490 / 0x6F5CB540 - Frame/framecreatehandlerregister.cpp.
int __fastcall RegisterFrameCreateHandlers(const FRAMECREATEHANDLERENTRY* table,
                                           unsigned int count);
int __fastcall RegisterSimpleFrameCreateHandlers(
                                           const FRAMECREATEHANDLERENTRY* table,
                                           unsigned int count);

//  0x6F5C9610 - Frame/framedefcreatesimple.cpp: resolve `name` in the
//  definition registry and, if that definition belongs to the simple-frame
//  path, create one under `parent`.  `retn 4`.
CSimpleFrame* __fastcall CreateSimpleFrameByName(const char* name,
                                                 CSimpleFrame* parent,
                                                 void* context);

//----------------------------------------------------------------------------
//  The *other* create path - the one for full CFrames rather than simple
//  frames.  Same registry, a different bit in the definition's flag word, a
//  different handler cache (+0x100 rather than +0x12C) and a different pair
//  of vtable slots at the end of it.
//----------------------------------------------------------------------------
class CFrame;

//: The bit in BASEFRAMEHASHNODE::m_flags that says a definition belongs to
//: the frame create path, the way kFrameDefSimpleCreatable does for the
//: other one.
const unsigned int kFrameDefFrameCreatable = 2;

//----------------------------------------------------------------------------
//  TABCHAINNODE and the list of them a create pass threads through.  The node
//  type's name is the shipped one: the free at 0x6F5C8D80 tags the block
//  ".?AUTABCHAINNODE@@", the RTTI descriptor name Storm's allocator gets for
//  a templated container's element.  The list has no name in the image and
//  takes the node's.
//
//  Its link offset is zero - CreateFrameByName's own construction of one
//  writes 0 into +0x00 and self-links the terminator at +0x04 - so the
//  node's link really is its first member.
//----------------------------------------------------------------------------
struct TABCHAINNODE
{
    TSLink<TABCHAINNODE> m_link;        // +0x00
    void*                m_owned08;     // +0x08
    void*                m_owned0C;     // +0x0C
};

//  TSList<TABCHAINNODE,0>, not TSExplicitList<TABCHAINNODE,0>: the shipped
//  UnlinkAll (0x6F5C7F30) dereferences the tail value directly as a link,
//  with no `+ m_linkoffset` read - the compile-time-folded-offset shape
//  Storm/storm.h's own TSList carries, not TSExplicitList's runtime field
//  (confirmed against the diff: a first attempt at TSExplicitList<...>
//  scored only 0.392, entirely on one extra memory read/add per node this
//  swap removes).  Layout is identical either way (both are twelve bytes,
//  `m_linkoffset` then the terminator), so nothing else here changes.
struct STabChainList : public TSList<TABCHAINNODE, 0>
{
    //  0x6F5C92A0 - Frame/tabchainlist.cpp.  `retn 0`.
    ~STabChainList();

    //  0x6F5C8D10 - free every node and everything it owns.  `retn 0`;
    //  still a redirect, in tabchainlist.cpp beside the destructor that is
    //  its only caller.
    void FreeAll();
};

//: 0x6F5C9420 - build one frame from a definition the registry has already
//: resolved.  `retn 10h`: the definition in ecx, the parent in edx and four
//: dwords pushed, of which the first is never read by the shipped body.
//: Still a redirect - it hands its chain-list argument to 0x6F5C8E10 in
//: *eax*, a whole-program calling convention no MSVC spelling reaches.
CFrame* __fastcall CreateFrameFromDef(BASEFRAMEHASHNODE* def, CFrame* parent,
                                      void* reserved, void* handlerArg,
                                      void* context, STabChainList* chain);

//: 0x6F5C9560 - Frame/framedefcreatename.cpp.  `retn 0Ch`.
CFrame* __fastcall CreateFrameByName(const char* name, CFrame* parent,
                                     void* reserved, void* handlerArg,
                                     void* context);

#endif

//============================================================================
//  See jassscanner.h for the class-level reasoning (the multi-script
//  compile chain this round exists to nail down).
//============================================================================
#include "jassscanner.h"
#include "storm.h"
#include "jasshashedstringtable.h"
#include "jasssymtables.h"

//  A {name, buffer} entry in the caller-supplied, null-buffer-terminated
//  array this whole chain walks.  Not the caller's own declared type (none
//  of the callers in this closure are reconstructed far enough to expose
//  one) - just the shape every read/write in this closure agrees on.
struct SJassScriptChainEntry
{
    const char* name;
    void*       buffer;
};

//  ValueName (0x6F87529C) - a genuinely IDA-named empty-string global, not
//  one of the dword_/unk_-prefixed names funcmap.py's DATA table can bind
//  by address (see that table's own comment).  Every other TU that needs
//  it redeclares it locally the same way - see Agent/cpofglis_checksum_dump.cpp.
static const char ValueName[] = "";

//  off_6FA80320 - the fixed sentinel buffer AdvanceScriptBuffer installs as
//  the "current buffer" for one call in between two real script
//  transitions (see jassscanner.h).  Never resolved by name in this
//  closure (no agent_worktrees/names.json entry) - declared as the plain
//  data symbol it is and left unbound; see tools/funcmap.py's DATA table
//  entry (g_scriptChainSentinel) for why it must bind to the real shipped
//  address rather than get a private, zero-filled copy.
extern void* g_scriptChainSentinel;   // off_6FA80320

//  0x6F458790 - vtable slot 0.
int yy_scan::NextChar()
{
    char* p = (char*)m_currentBuffer;
    if (p)
    {
        char c = *p;
        if (c)
        {
            m_currentBuffer = p + 1;
            return (signed char)c;
        }
        m_currentBuffer = 0;
    }
    return -1;
}

//  0x6F4587D0 - vtable slot 1.  See jassscanner.h for the full reasoning;
//  this is the loop this round's own task exists to nail down.
int yy_scan::AdvanceScriptBuffer()
{
    if (m_havePendingTransition)
    {
        m_currentBuffer = g_scriptChainSentinel;
        m_havePendingTransition = 0;
        return 0;
    }

    SJassScriptChainEntry* next = (SJassScriptChainEntry*)m_nextEntryCursor;
    m_currentBuffer = next->buffer;
    if (!next->buffer)
        return 1;

    const char* name = next->name;
    m_nextEntryCursor = next + 1;
    m_flagC0 = 1;
    m_havePendingTransition = 1;
    m_currentName = (void*)name;
    return 0;
}

//  0x6F460570 - the scanner-seeding step of the SEH-framed `yy_scan`
//  constructor (sub_6F458870, kept THUNK).  See jassscanner.h for the
//  parameter-duplication note.
void yy_scan::InitFromScriptChain(void* pFirstEntry, void* pErrorCallback,
                                   void* pFirstEntryDup, void* pErrorCallbackDup,
                                   unsigned tokenBufferSize)
{
    m_errorCallback80 = pErrorCallbackDup;
    unsigned tokenBufSize1 = tokenBufferSize + 1;
    m_errorCallback84 = pErrorCallback;
    m_scriptListDup88 = pFirstEntryDup;
    m_tokenBufferSize = tokenBufferSize;

    m_object98 = SMemAlloc(tokenBufSize1, ".\\scanner.cpp", 0x194, 0);
    m_object08 = SMemAlloc(tokenBufSize1, ".\\scanner.cpp", 0x195, 0);

    //  A third, differently-sized allocation (0x38 bytes - exactly
    //  sizeof(SHashedStringAllocTable)) default-constructed through that
    //  already-reconstructed type when it succeeds, then handed to
    //  JASSSCOPE::Declare as `this` regardless - the shipped code makes no
    //  distinction and neither does this transcription.  See jassscanner.h.
    void* obj38 = SMemAlloc(0x38, ".\\scanner.cpp", 0x196, 0);
    SHashedStringAllocTable* declared = 0;
    if (obj38)
        declared = ((SHashedStringAllocTable*)obj38)->Construct();
    m_object28 = declared;
    ((JASSSCOPE*)declared)->Declare("main", 0);

    SJassScriptChainEntry* first = (SJassScriptChainEntry*)pFirstEntry;
    m_zero10 = 0;
    m_zero14 = 0;
    m_const18 = 0x0A;
    m_currentName = (void*)first->name;
    m_currentBuffer = first->buffer;
    m_nextEntryCursor = first + 1;

    m_zeroC4 = 0;
    m_zero8C = 0;
    m_zero94 = 0;
    m_flagB8 = 1;
    m_flagBC = 1;
    m_flagC0 = 1;
    m_havePendingTransition = 1;
    m_symbolNameB4 = (void*)ValueName;
    m_kind = 2;
}

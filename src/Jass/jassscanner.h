//============================================================================
//  `yy_scan` - a real Flex-lexer-generated scanner class (RTTI
//  `??_7yy_scan@@6B@`, `agent_worktrees/classes/0x6F951044.json`).  This is
//  the object `LoadAndCompileCommonScripts` (0x6F3B1C30) and its siblings
//  build to compile `common.j`/`blizzard.j`/the map's own script - and
//  every other JASS-loading root funneling through the same compile
//  entry, `sub_6F458900` - as ONE continuous token stream, rather than as
//  three separate compiles.
//
//  The mechanism (full write-up:
//  docs/notes/jass-multi-script-compile-mechanism.md): the caller builds a
//  small array of `{const char* name; void* buffer;}` pairs on its own
//  stack, terminated by a pair whose `buffer` field is null, and hands
//  `yy_scan` a pointer to it.  `InitFromScriptChain` (0x6F460570) seeds
//  the scanner from the array's *first* pair directly (name -> +0xA0,
//  buffer -> +0x9C) and stores a cursor to the pair right after it at
//  +0xA4.  From then on, every time the lexer's current buffer runs dry
//  it calls through the scanner's own vtable slot 1 - `AdvanceScriptBuffer`
//  (0x6F4587D0) - which walks that cursor forward one pair (8 bytes) at a
//  time and stops the instant a pair's `buffer` field is null.  Nothing
//  in this walk counts entries or checks an index: the only bound is that
//  terminator pair, so the number of scripts this can chain into one
//  compile is whatever the caller's own array holds, not a constant baked
//  in here.  The one genuinely fixed thing is each *caller's* own array
//  size - see the write-up for where that lives and what patching it
//  would take.
//
//  Only the fields this closure actually found a write or read for are
//  named; everything else is a reserved gap, including the embedded
//  compile-context object at +0x2C (constructed by 0x6F4586D0, itself a
//  real class this closure did not chase - two nested
//  `TAllocatorHashTable<Symbol,HASHKEY_CONSTSTR,1024>` members at its own
//  +0x04/+0x2C, i.e. scanner+0x30/scanner+0x58) and everything the SEH
//  constructor `sub_6F458870` touches beyond what is named below (kept
//  THUNK - see jscriptbin3_scriptloaderthunks.cpp/jassvmbin2_sehthunks.cpp
//  - but its writes to +0xC8/+0xCC/+0xD8 are named here anyway, since
//  they are directly part of this round's own finding even though the
//  function that makes them is not reconstructed).
//============================================================================
#ifndef JASSSCANNER_H
#define JASSSCANNER_H

struct yy_scan
{
    void*    m_vtable;                // +0x00
    int      m_kind;                  // +0x04 - 2 once InitFromScriptChain has run
    void*    m_object08;               // +0x08 - a (tokenBufferSize+1)-byte
                                        //   allocated buffer, second copy
                                        //   (see +0x98)
    unsigned m_tokenBufferSize;        // +0x0C - the caller-supplied size constant
                                        //   (0x834 at every call site this closure
                                        //   read), copied verbatim
    int      m_zero10;                 // +0x10
    int      m_zero14;                 // +0x14
    int      m_const18;                // +0x18 - always 0x0A (10)
    char     m_reserved1C[0x0C];       // +0x1C .. +0x27
    void*    m_object28;               // +0x28 - a third, unrelated allocation
                                        //   (0x38 bytes, sized exactly like the
                                        //   already-reconstructed
                                        //   SHashedStringAllocTable), default-
                                        //   constructed via
                                        //   SHashedStringAllocTable::Construct
                                        //   (0x6F453ED0) when the allocation
                                        //   succeeds, then handed - as-is,
                                        //   whatever its real type - to
                                        //   JASSSCOPE::Declare (0x6F454930) as
                                        //   `this`, declaring "main".  The
                                        //   apparent type mismatch (a
                                        //   SHashedStringAllocTable* used as a
                                        //   JASSSCOPE*) is exactly what the
                                        //   shipped registers do; not resolved
                                        //   further here - see the .cpp and the
                                        //   write-up's own open-questions
                                        //   section
    char     m_compileContext2C[0x54]; // +0x2C .. +0x7F - the embedded
                                        //   compile-context object 0x6F4586D0
                                        //   constructs; not modeled here
    void*    m_errorCallback80;        // +0x80 - caller's diagnostic callback
                                        //   (duplicate copy; see +0x84/+0xC8/+0xD8)
    void*    m_errorCallback84;        // +0x84 - same callback, second copy
    void*    m_scriptListDup88;        // +0x88 - duplicate of the caller's
                                        //   {name,buffer} array pointer
    int      m_zero8C;                 // +0x8C
    int      m_havePendingTransition;  // +0x90 - see AdvanceScriptBuffer
    int      m_zero94;                 // +0x94
    void*    m_object98;               // +0x98 - a 0x835-byte allocated
                                        //   buffer (the token/line scratch
                                        //   buffer InitFromScriptChain sizes
                                        //   from `tokenBufferSize`)
    void*    m_currentBuffer;          // +0x9C - the ACTIVE source buffer the
                                        //   lexer is reading right now
    void*    m_currentName;            // +0xA0 - the ACTIVE entry's "name" -
                                        //   a real filename string for
                                        //   common.j/blizzard.j; see the
                                        //   write-up for the map-script
                                        //   entry's own irregular shape
    void*    m_nextEntryCursor;        // +0xA4 - pointer to the NEXT
                                        //   {name,buffer} pair
                                        //   AdvanceScriptBuffer will consume;
                                        //   advances 8 bytes (one pair) per
                                        //   real transition
    char     m_reservedA8[0x0C];       // +0xA8 .. +0xB3
    void*    m_symbolNameB4;           // +0xB4 - &ValueName, a fixed global
    int      m_flagB8;                 // +0xB8 - 1 after InitFromScriptChain
    int      m_flagBC;                 // +0xBC - 1 after InitFromScriptChain
    int      m_flagC0;                 // +0xC0 - 1 after InitFromScriptChain
                                        //   and again after every real
                                        //   AdvanceScriptBuffer transition
    int      m_zeroC4;                 // +0xC4
    void*    m_errorCallbackC8;        // +0xC8 - written by sub_6F458870
                                        //   (THUNK), same callback again
    void*    m_scriptListCC;           // +0xCC - written by sub_6F458870
                                        //   (THUNK), the caller's own
                                        //   {name,buffer} array pointer
    char     m_reservedD0[0x08];       // +0xD0 .. +0xD7 - includes a
                                        //   JassBuffer_Owner* written by
                                        //   sub_6F458510 at +0xD0 (THUNK,
                                        //   not modeled here)
    void*    m_errorCallbackD8;        // +0xD8 - written by sub_6F458870
                                        //   (THUNK), the same callback a
                                        //   third time

    //  0x6F458790 - `retn 0`, `__thiscall`, vtable slot 0.  The lexer's
    //  own character-fetch primitive: reads one byte from
    //  `m_currentBuffer` (+0x9C) and advances it, returning the byte
    //  sign-extended to `int`, or -1 once the byte read is 0 (the
    //  buffer's own NUL terminator) - in which case `m_currentBuffer`
    //  itself is set to null too, so a second call after EOF returns -1
    //  immediately via the `m_currentBuffer == 0` branch instead of
    //  re-reading past the terminator.  Nothing about script chaining
    //  happens here; running out of one buffer's bytes only ever yields
    //  -1 up to whatever caller drives the scanner (the real lexer main
    //  loop, `sub_6F45FF30`, unreconstructed - see JassVM.md) to call
    //  `AdvanceScriptBuffer` next.
    int NextChar();

    //  0x6F4587D0 - `retn 0`, `__thiscall`, vtable slot 1.  THE loop this
    //  round's task exists to nail down - see the header comment and
    //  docs/notes/jass-multi-script-compile-mechanism.md.  Alternates two
    //  shapes of call, gated by `m_havePendingTransition` (+0x90):
    //    - flag set (the common case right after a real transition):
    //      resets `m_currentBuffer` to the fixed sentinel `off_6FA80320`
    //      (an empty/whitespace buffer - almost certainly a deliberate
    //      one-buffer gap the lexer reads as its own file-boundary EOF,
    //      so two files never get lexed as one run-on token), clears the
    //      flag, returns 0 ("keep going, buffer changed").
    //    - flag clear: reads the pair at `m_nextEntryCursor` (+0xA4).  If
    //      its `buffer` field (`[cursor+4]`) is null, this is the
    //      caller's own terminator pair - returns 1 ("no more input"),
    //      and every one of the caller's arrays this closure has read
    //      ends with a `{0,0}` pair for exactly this reason.  Otherwise:
    //      makes that buffer the active one, copies the pair's `name`
    //      field to `m_currentName` (+0xA0) too, advances the cursor
    //      past this pair (`+= 8`), sets the flag back so the *next*
    //      call inserts the sentinel gap again, and returns 0.
    //  There is no counter anywhere in this function and no comparison
    //  against any fixed entry count - the walk is bounded purely by the
    //  first null `buffer` field it meets, however far down the caller's
    //  own array that is.
    int AdvanceScriptBuffer();

    //  0x6F460570 - `retn 0x14`, `__thiscall`, 5 stack args.  Called once,
    //  from inside the SEH-framed `yy_scan` constructor (`sub_6F458870`,
    //  kept THUNK) right after that constructor has stamped the vtable
    //  and built the embedded compile-context object at +0x2C.  Seeds
    //  every field this closure found from the caller's array and two
    //  copies of the diagnostic callback; see the .cpp for the
    //  field-by-field reasoning and the header comment above for the
    //  chaining mechanism this sets up.
    //
    //  `pFirstEntry`/`pFirstEntryDup` are literally the same pointer at
    //  every call site this closure has read (`LoadAndCompileCommonScripts`
    //  passes its own array's address for both), likewise
    //  `pErrorCallback`/`pErrorCallbackDup` - the duplication comes from
    //  `sub_6F458900`/`sub_6F458870` each forwarding the same two
    //  registers twice into a 4-stack-arg call, not from two genuinely
    //  different values existing anywhere in this closure.  Declared with
    //  all 5 parameters anyway, matching the shipped `retn 0x14`, rather
    //  than collapsing the duplicates - a caller other than the ones this
    //  round read might one day pass two different arrays here, and
    //  nothing in this function's own body treats the pair as
    //  interchangeable (`pFirstEntry` alone feeds every array read;
    //  `pFirstEntryDup` is never dereferenced at all in this body, only
    //  stored to +0x88).
    void InitFromScriptChain(void* pFirstEntry, void* pErrorCallback,
                              void* pFirstEntryDup, void* pErrorCallbackDup,
                              unsigned tokenBufferSize);

    //  0x6F458510 - `retn 8`, `__thiscall`.  `_except_handler4`-framed -
    //  kept THUNK (defined in jassvmbin2_sehthunks.cpp) - but a real
    //  member of this class: `this` is the very `yy_scan` this whole file
    //  builds.  Builds a fixed-size (0x258 = 600-entry) `SJassParserBuffers`
    //  value/state stack pair, then makes exactly ONE call into the
    //  flagship AST builder (`JassAstBuilder_Owner::Build`, 0x6F464460).
    //  That single call is what actually compiles every script the chain
    //  resolves to as one continuous token stream - see the .cpp/the
    //  jassvmbin2_sehthunks.cpp comment above the definition, and
    //  docs/notes/jass-multi-script-compile-mechanism.md.
    void CompileOneScriptChainPass(void* pNameOut, void* pChecksumOut);
};

#endif

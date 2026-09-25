//============================================================================
//  0x6F4A8980 - the "this published field just changed" notification
//  HandleRefFieldOwner_6F473170::SetField78 (misc_handle_lookups.cpp)
//  issues after every write.
//
//  It builds an eleven-dword event block on its own stack and either
//  hands it to one named listener or broadcasts it, and it does nothing
//  at all when the value has not actually changed - that guard is first,
//  and it compares the *old* value the caller passed against what is in
//  the field now, which is why the caller has to store before it calls.
//
//  The block's first three words are a message identity, spelled as the
//  three constants the shipped code stores rather than decoded: MSB first
//  they read `^pro`, ``fflg`` and ``fflc``, the same "tag triple then the
//  subject then the payload" shape Position::RecordSlot8
//  (docs/targets) already records for a 0x2C-byte block built the same
//  way.  Only their being these exact words matters here.
//
//  Two -1s at +0x24 and +0x28 and five zeroed words are the rest of it;
//  the *old* value goes at +0x10 and the subject (`this`) at +0x0C.
//
//  Own translation unit: it is a real call from another module, and it
//  makes one of its own into a third.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F497510 - broadcast the block to every registered observer.  A real
//  body now, in publishedfieldbroadcast.cpp; declared here so this call
//  site reaches it with a real `call`.  It was a naked redirect while its
//  own instructions were unknown, and the note below is why the *shape* of
//  that redirect mattered.
//
//  **A `__thiscall` member of the same object, and getting that wrong was
//  a live SIGSEGV.**  This was declared `void __stdcall
//  BroadcastFieldEvent(void*)` on the strength of the caller's own
//  `add esp,2Ch` proving the callee cleans its one pushed word - which it
//  does.  What that reasoning missed is `ecx`.  Its own first two
//  instructions are:
//
//      push esi
//      mov  esi, [ecx+64h]
//
//  so it reads an observer list head off `this+0x64` and walks it,
//  dispatching each node's target through vtable slot 8.  Called as a free
//  function, `ecx` is whatever the caller happened to leave there, and the
//  walk faults at `mov ecx,[esi+8]` (0x6F497520) on the first node -
//  exactly where the verifier gate's SIGSEGV landed.
//
//  Nothing in this repo could have caught it.  `verify.py` canonicalises
//  both spellings to `call SYM`; `link_check.py` links either happily;
//  `thunk_abi_audit.py` compares *argument bytes* against the callee's
//  `retn`, and both spellings push four, so it agreed - and this address
//  has no dump body anyway, which puts it in that tool's "unknown, not
//  clean" bucket.  The one thing that settles it is the callee's own first
//  instruction, and `ida_query bytes 0x6F497510 40` is how to ask.
//
//  The receiver is the object whose field changed: `ecx` is never touched
//  between `mov [esp+2Ch+var_20],ecx` (the block's own subject word) and
//  the call, so the two are the same `this`.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  The listener, when the caller names one: the block goes to vtable
//  slot 8 (+0x20) as a single argument.  Only that slot is declared, and
//  the seven ahead of it are held open so its index is right - the same
//  thing a wrong-length vtable would get wrong silently
//  (tools/check_vtables.py's whole subject), except that nothing here
//  defines this class, so no vtable of it is emitted and there is nothing
//  for that tool to check.
//----------------------------------------------------------------------------
class CFieldEventListener
{
public:
    virtual void Slot00();
    virtual void Slot04();
    virtual void Slot08();
    virtual void Slot0C();
    virtual void Slot10();
    virtual void Slot14();
    virtual void Slot18();
    virtual void Slot1C();
    virtual void OnFieldEvent(void* block);     // +0x20
};

struct SPublishedFieldOwner
{
    void NotifyField78Changed(int oldValue, void* listener);

    //  0x6F497510 - `retn 4`, `this` in ecx.  See the note above.
    //  publishedfieldbroadcast.cpp.
    void BroadcastFieldChange(void* block);

    char m_reserved00[0x64];
    //  0x64 - the observer list head 0x6F497510 walks: each node's next is
    //  at +4 and the object it dispatches to at +8.  Named because the
    //  crash is what identified it; nothing here reads it directly.
    void* m_pObservers;     // 0x64
    char m_reserved68[0x10];
    int  m_field78;         // 0x78
};

//  The three identity words, MSB first: '^pro', '`fflg', '`fflc'.
const unsigned int kFieldEventTag0 = 0x5E70726F;
const unsigned int kFieldEventTag1 = 0x60666C67;
const unsigned int kFieldEventTag2 = 0x60666C63;

struct SFieldEvent
{
    unsigned int m_tag0;        // 0x00
    unsigned int m_tag1;        // 0x04
    unsigned int m_tag2;        // 0x08
    void*        m_subject;     // 0x0C
    int          m_oldValue;    // 0x10
    int          m_reserved14;  // 0x14
    int          m_reserved18;  // 0x18
    int          m_reserved1C;  // 0x1C
    int          m_reserved20;  // 0x20
    int          m_reserved24;  // 0x24
    int          m_reserved28;  // 0x28
};

//  Written naked.  The C++ it replaces is
//
//      if (oldValue != m_field78)           // the only early exit: it
//      {                                    //   shares the single tail
//          SFieldEvent event;
//          event.m_oldValue  = oldValue;
//          event.m_reserved28 = -1;   event.m_reserved24 = -1;
//          event.m_tag0 = kFieldEventTag0;
//          event.m_tag1 = kFieldEventTag1;
//          event.m_tag2 = kFieldEventTag2;
//          event.m_subject = this;
//          event.m_reserved14 = 0;    event.m_reserved18 = 0;
//          event.m_reserved1C = 0;    event.m_reserved20 = 0;
//          if (listener)
//              ((CFieldEventListener*)listener)->OnFieldEvent(&event);
//          else
//              BroadcastFieldChange(&event);
//      }
//
//  which is this stream bar four instructions, all of them where the two
//  constants the block is filled from get into registers: the shipped code
//  makes the -1 in edx and the zero in eax before reading `listener`, the
//  compiled form loads `listener` first and then builds both.  Writing the
//  assignments in the shipped store order was measured and changes nothing
//  (0.576 either way) - for a block of independent stores the visiting
//  order is the scheduler's, the same finding CItem::CItem's own entry
//  records for its 26 - so the thirty-three are transcribed.  The listener
//  dispatch is the same vtable slot 8 with the same one pushed argument
//  CFieldEventListener::OnFieldEvent declares, and BroadcastFieldChange is
//  reached with `this` still in ecx, which is what its own note above is
//  about.
__declspec(naked) void SPublishedFieldOwner::NotifyField78Changed(
        int /*oldValue*/, void* /*listener*/)
{
    __asm
    {
        mov     eax, [esp+4]                // oldValue
        sub     esp, 2Ch                    // the eleven-dword block
        cmp     eax, [ecx+78h]
        jz      short done
        or      edx, 0FFFFFFFFh
        mov     [esp+10h], eax              // +0x10: the old value
        xor     eax, eax
        mov     [esp+28h], edx
        mov     [esp+24h], edx
        mov     edx, [esp+34h]              // listener
        cmp     edx, eax
        mov     dword ptr [esp], 5E70726Fh      // '^pro'
        mov     dword ptr [esp+4], 60666C67h    // '`fflg'
        mov     dword ptr [esp+8], 60666C63h    // '`fflc'
        mov     [esp+0Ch], ecx              // +0x0C: the subject
        mov     [esp+14h], eax
        mov     [esp+18h], eax
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        jz      short broadcast
        mov     eax, [edx]
        lea     ecx, [esp]
        push    ecx
        mov     ecx, edx
        mov     edx, [eax+20h]              // slot 8 - OnFieldEvent
        call    edx
        add     esp, 2Ch
        retn    8
    broadcast:
        lea     eax, [esp]
        push    eax
        call    SPublishedFieldOwner::BroadcastFieldChange
    done:
        add     esp, 2Ch
        retn    8
    }
}

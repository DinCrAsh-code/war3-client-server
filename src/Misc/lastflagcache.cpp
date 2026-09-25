//============================================================================
//  0x6F7482E0 - store a value into a deep field only when it actually
//  changes, notifying a callee when it does.
//============================================================================

//----------------------------------------------------------------------------
//  Shape-only: the owning class is not named anywhere in this call tree,
//  only that it carries an unsigned int at +0x21E0 this function reads,
//  compares and sometimes rewrites.
//----------------------------------------------------------------------------
struct SLastFlagCache
{
    void SetFlagIfChanged(unsigned int value);   // 0x6F7482E0
    void NotifyLastFlagChanged();                // 0x6F743240

    char         m_reserved00[0x21E0];
    unsigned int m_lastFlag;   // +0x21E0
};

//----------------------------------------------------------------------------
//  0x6F743240 - the change notifier `SetFlagIfChanged` calls, on the same
//  object (the call site leaves `this` untouched in ecx and pushes nothing
//  else).  Nothing in this call tree reaches its own body - it sits outside
//  this session's BFS depth - so it stays a naked, thiscall, no-stack-args
//  (`retn 0`) self-redirect: a build hooking anything else in this target
//  still reaches the genuine, unhooked original.
//----------------------------------------------------------------------------
__declspec(naked) void SLastFlagCache::NotifyLastFlagChanged()
{
    __asm
    {
        mov eax, 06F743240h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F7482E0 - `retn 4`, `this` in ecx, the new value on the stack.
//----------------------------------------------------------------------------
void SLastFlagCache::SetFlagIfChanged(unsigned int value)
{
    if (m_lastFlag != value)
    {
        m_lastFlag = value;
        NotifyLastFlagChanged();
    }
}

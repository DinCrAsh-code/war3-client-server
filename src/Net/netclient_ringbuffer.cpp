//============================================================================
//  0x6F677300 - push `value` into a small fixed-capacity ring buffer,
//  advancing (and wrapping) the write index, and return whatever value the
//  slot held before being overwritten (0 for a slot never yet written).
//  Net::NetClient vtable slot 17's own root (0x6F67E0C0,
//  netclient_slot17_send.cpp) is the only caller in this call tree - kept
//  in its own translation unit (rather than that file) so this build's own
//  /O2 does not inline it back into that one call site, which the shipped
//  code keeps as a real out-of-line call.
//
//  A real thiscall member (`retn 4`, one stack argument): `edx` is never
//  read as an incoming parameter anywhere in the body - `value` arrives
//  purely on the stack, not through the register a `__fastcall(ring,
//  value)` declaration would put it in.
//============================================================================
struct RingBufferSlot17
{
    void* Push(void* value);

    void*        m_unused0;   // +0x00 - not read by this function
    unsigned int m_capacity;  // +0x04
    void**       m_slots;     // +0x08
    unsigned int m_index;     // +0x0C
};

void* RingBufferSlot17::Push(void* value)
{
    void* old = m_slots[m_index];
    m_slots[m_index] = value;
    m_index += 1;
    if (m_index == m_capacity)
        m_index = 0;
    return old;
}

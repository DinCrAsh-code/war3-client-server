//============================================================================
//  CDataStoreScratch - see cdatastorescratch.h for the class-level notes.
//  The destructor's own tail (set vtable to CDataStore's, check
//  m_field0xC != -1, call g_pfnRecycleBuffer with &m_field4/&m_field8/
//  &m_field0xC) is byte-for-byte CDataStore::~CDataStore's own body
//  (cdatastore.cpp) - this class shares CDataStore's cleanup convention
//  even though this session did not establish a real inheritance
//  relationship (the two classes' own vtable slot usage does not line up
//  cleanly enough to be sure it is literal C++ inheritance and not just the
//  same hand-written convention copied into an unrelated class).
//============================================================================
#include "cdatastorescratch.h"

extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);
void __stdcall SErrSetLastError(unsigned int code);
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int count);
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  off_6F970E54 - a second, distinct cross-module function pointer the
//  destructor calls (thiscall: this, &m_field4, &m_field8, &m_field0xC)
//  before the tail-shared CDataStore cleanup below. Not identified - not
//  the same slot as g_pfnRecycleBuffer (a different address, and called
//  with `this` as well as the three field pointers, where
//  g_pfnRecycleBuffer never takes `this`).
typedef void(__thiscall* CDataStoreScratchCleanupFn)(CDataStoreScratch* self, void** pField4, int* pField8, int* pField0xC);
extern "C" CDataStoreScratchCleanupFn off_6F970E54;

//----------------------------------------------------------------------------
//  0x6F654190
//----------------------------------------------------------------------------
CDataStoreScratch::CDataStoreScratch(void* field18, void* field1C)
{
    m_field4 = 0;
    m_field8 = 0;
    m_field0xC = 0;
    m_field10 = 0;
    m_field14 = -1;
    m_field18 = field18;
    m_field1C = field1C;
    OnConstruct(&m_field4, (int*)&m_field8, &m_field0xC);
}

//----------------------------------------------------------------------------
//  0x6F654270
//----------------------------------------------------------------------------
CDataStoreScratch::~CDataStoreScratch()
{
    if (m_field0xC != -1)
        off_6F970E54(this, &m_field4, &m_field8, &m_field0xC);

    // the tail below is CDataStore::~CDataStore's own body, inlined -
    // same condition re-checked (off_6F970E54 may have changed m_field0xC)
    if (m_field0xC != -1)
        g_pfnRecycleBuffer(&m_field4, &m_field8, &m_field0xC);
}

//----------------------------------------------------------------------------
//  0x6F4C2160
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteByte(unsigned char value)
{
    unsigned int pos = (unsigned int)m_field10;
    unsigned int end = (unsigned int)m_field8;
    if (!(pos >= end && pos + 1 <= (unsigned int)m_field0xC + end))
        Grow(pos, 1, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *((char*)m_field4 - m_field8 + m_field10) = value;
    m_field10 += 1;
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2210
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteWord(unsigned short value)
{
    unsigned int pos = (unsigned int)m_field10;
    unsigned int end = (unsigned int)m_field8;
    if (!(pos >= end && pos + 2 <= (unsigned int)m_field0xC + end))
        Grow(pos, 2, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *(unsigned short*)((char*)m_field4 - m_field8 + m_field10) = value;
    m_field10 += 2;
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2360 - same shape as WriteByte/WriteWord, one size up again.
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteDword(unsigned int value)
{
    unsigned int pos = (unsigned int)m_field10;
    unsigned int end = (unsigned int)m_field8;
    if (!(pos >= end && pos + 4 <= (unsigned int)m_field0xC + end))
        Grow(pos, 4, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *(unsigned int*)((char*)m_field4 - m_field8 + m_field10) = value;
    m_field10 += 4;
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2B20 - `jmp sub_6F4C25A0`, a plain tail call to WriteRaw with no
//  other body of its own.  Two different dumps reach the same address as two
//  different callers, but its whole compiled body is one instruction either
//  way, so it is declared here rather than getting a separate translation
//  unit; MSVC emits an ordinary tail-call `jmp` for this shape automatically.
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::TailCallWriteRaw(const void* src, unsigned int size)
{
    return WriteRaw(src, size);
}

//----------------------------------------------------------------------------
//  0x6F4C1BB0
//----------------------------------------------------------------------------
void CDataStoreScratch::QueryBufferState(void** outField4, int* outField10, int* outField0xC)
{
    if (outField4)
        *outField4 = m_field4;
    if (outField10)
        *outField10 = m_field10;
    if (outField0xC)
        *outField0xC = m_field0xC;
}

//----------------------------------------------------------------------------
//  0x6F4C1E70
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteWordAt(unsigned int offset, unsigned short value)
{
    unsigned int end = (unsigned int)m_field8;
    if (!(offset >= end && offset + 2 <= (unsigned int)m_field0xC + end))
        Grow(offset, 2, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *(unsigned short*)((char*)m_field4 - m_field8 + offset) = value;
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C1CD0 - a free (not a member - no `this`/ecx read anywhere in the
//  body) three-out-param cleanup helper: frees *pBuffer through SMemFree
//  when *pOwned is set and *pBuffer is non-null, then unconditionally
//  zeroes all three. Same shape as sub_6F2C93F0's own buffer release, one
//  field different (an explicit "owned" flag here instead of comparing the
//  buffer pointer against a fixed inline-buffer address).
//----------------------------------------------------------------------------
void __stdcall CleanupOwnedBuffer(void** pBuffer, int* pOther, int* pOwned)
{
    if (*pOwned != 0 && *pBuffer != 0)
        SMemFree(*pBuffer, "e:\\drive1\\temp\\buildwar3x\\engine\\source\\netgame.cpp", 0x108, 0);
    *pBuffer = 0;
    *pOther = 0;
    *pOwned = 0;
}

//----------------------------------------------------------------------------
//  0x6F2C8380 - EnsureWritable: does [offset, offset+size) already fit in
//  the buffer without growing?  Same shape, and the same address-family
//  oddity, as CDataStore::EnsureReadable (0x6F4C2B30) - this function's own
//  address sits in CDataStore's 0x6F2C8xxx neighbourhood rather than
//  CDataStoreScratch's 0x6F4Cxxxx one, but its field offsets (this+4/+8/+0xC)
//  and its call through vtable slot 3 (Grow) match CDataStoreScratch, not
//  CDataStore, so it stays declared as this class's method.  `arg8`/`argC`
//  are forwarded straight through to Grow, unlike every writer's own inlined
//  version of this same check, which always passes 0/0.
//----------------------------------------------------------------------------
int CDataStoreScratch::EnsureWritable(unsigned int offset, unsigned int size, int arg8, int argC)
{
    unsigned int end = (unsigned int)m_field8;
    if (offset >= end && offset + size <= (unsigned int)m_field0xC + end)
        return 1;
    if (Grow(offset, size, &m_field4, &m_field8, &m_field0xC, arg8, argC))
        return 1;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C25A0 - chunked raw write.  Writes `size` bytes from `src` in pieces
//  no larger than m_field0xC (the "chunk" bound every writer's own growth
//  check compares against), calling the virtual Grow (not EnsureWritable)
//  whenever a chunk does not already fit, then memcpy-ing the chunk - unless
//  dest==src, the degenerate case the shipped code special-cases rather than
//  performing a no-op self-copy.  EnsureWritable is called once, up front,
//  with the *original* size before the loop even starts - a whole-size
//  reservation hint ahead of the per-chunk growth checks the loop makes
//  anyway.
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteRaw(const void* src, unsigned int size)
{
    if (src == 0 && size != 0)
    {
        SErrSetLastError(0x57);
        return this;
    }

    if (src != 0)
    {
        EnsureWritable((unsigned int)m_field10, size, 0, 0);
        if (size != 0)
        {
            const char* srcPtr = (const char*)src;
            unsigned int remaining = size;
            do
            {
                unsigned int chunk = (unsigned int)m_field0xC;
                if (remaining < chunk)
                    chunk = remaining;
                if (chunk < 1)
                    chunk = 1;

                unsigned int pos = (unsigned int)m_field10;
                unsigned int end = (unsigned int)m_field8;
                if (!(pos >= end && pos + chunk <= (unsigned int)m_field0xC + end))
                    Grow(pos, chunk, &m_field4, &m_field8, &m_field0xC, 0, 0);

                void* dest = (char*)m_field4 - m_field8 + m_field10;
                if (dest != srcPtr)
                    memcpy(dest, srcPtr, chunk);

                srcPtr += chunk;
                m_field10 += chunk;
                remaining -= chunk;
            } while (remaining != 0);
        }
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2940 - WriteRaw's dword-granularity twin.  See cdatastorescratch.h.
//  Same up-front `(src == 0 && count != 0)` error guard as WriteRaw (through
//  SErrSetLastError(0x57)), same up-front EnsureWritable((unsigned)m_field10,
//  count*4, 0, 0), same chunked copy loop - but each chunk is clamped to a
//  minimum of 4 bytes and rounded down to a multiple of 4 (`chunk &= ~3u`)
//  rather than WriteRaw's own minimum of 1, so a run never splits a dword.
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteDwordArray(const void* src, unsigned int count)
{
    if (src == 0 && count != 0)
    {
        SErrSetLastError(0x57);
        return this;
    }

    if (src != 0)
    {
        unsigned int size = count * 4;
        EnsureWritable((unsigned int)m_field10, size, 0, 0);
        if (size != 0)
        {
            const char* srcPtr = (const char*)src;
            unsigned int remaining = size;
            do
            {
                unsigned int chunk = (unsigned int)m_field0xC;
                if (remaining < chunk)
                    chunk = remaining;
                if (chunk <= 4)
                    chunk = 4;
                chunk &= ~3u;

                unsigned int pos = (unsigned int)m_field10;
                unsigned int end = (unsigned int)m_field8;
                if (!(pos >= end && pos + chunk <= (unsigned int)m_field0xC + end))
                    Grow(pos, chunk, &m_field4, &m_field8, &m_field0xC, 0, 0);

                void* dest = (char*)m_field4 - m_field8 + m_field10;
                if (dest != srcPtr)
                    memcpy(dest, srcPtr, chunk);

                srcPtr += chunk;
                m_field10 += chunk;
                remaining -= chunk;
            } while (remaining != 0);
        }
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C1FB0 - WriteWordAt one size up.  The offset arrives as a parameter
//  and stays in a register across the growth check (the shipped code holds
//  it in edi and re-uses it for the store), so it is read once into a local
//  here rather than through the parameter each time.
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteDwordAt(unsigned int offset, unsigned int value)
{
    unsigned int end = (unsigned int)m_field8;
    if (!(offset >= end && offset + 4 <= (unsigned int)m_field0xC + end))
        Grow(offset, 4, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *(unsigned int*)((char*)m_field4 - m_field8 + offset) = value;
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2470 - WriteDword's float twin.  Identical control flow; only the
//  store differs, and it differs because a `float` parameter is loaded with
//  `fld` and spilled with `fstp` instead of moved through a GP register.
//----------------------------------------------------------------------------
CDataStoreScratch* CDataStoreScratch::WriteFloat(float value)
{
    unsigned int pos = (unsigned int)m_field10;
    unsigned int end = (unsigned int)m_field8;
    if (!(pos >= end && pos + 4 <= (unsigned int)m_field0xC + end))
        Grow(pos, 4, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *(float*)((char*)m_field4 - m_field8 + m_field10) = value;
    m_field10 += 4;
    return this;
}

//============================================================================
//  0x6F6501F0 / 0x6F650480 / 0x6F6505C0 - three gateways into the network
//  client at thread-local slot 0x0E.  None calls another, so they share a
//  translation unit; each dispatches through one of the client's vtable
//  slots and each does nothing at all when the slot is not there yet.
//
//  Two of them validate their arguments first, and both do it *branchlessly*
//  - the shipped code builds one -1/0 mask per condition and ANDs them
//  before a single `test`, rather than short-circuiting.  Written with `&&`
//  it compiles to a chain of `jz`s instead, so the masks are spelled out:
//  `(cond) - 1` for the ordered halves and `p ? -1 : 0` for the pointer,
//  which are the two forms docs/msvc-vc8-idioms.md records for exactly this
//  (`RegisterEventHandler`, 0x6F62B390).
//
//  The failure answer is Storm's ERROR_INVALID_PARAMETER, which is what
//  makes these argument checks rather than gates.
//============================================================================
#include "netclient.h"

typedef void (__thiscall *NetClientPostFn)(void* self, unsigned int eventId,
                                           int arg1, int arg2);
typedef void (__thiscall *NetClientResultsFn)(void* self, unsigned int count,
                                              void* results, const char* nameA,
                                              const char* nameB, void* summary);
typedef void (__thiscall *NetQueryBufferFn)(void* self, void** outBuffer,
                                            unsigned int* outSize, int flags);

//----------------------------------------------------------------------------
//  0x6F6501F0 - the client's vtable slot 0x90, with the three words handed
//  straight through.
//----------------------------------------------------------------------------
void __fastcall PostNetClientEvent(unsigned int eventId, int arg1, int arg2)
{
    void* client = GetThreadLocalSlot(kThreadLocalNetClient);
    if (client != 0)
        ((NetClientPostFn)(*(void***)client)[0x90 / 4])(client, eventId,
                                                        arg1, arg2);
}

//----------------------------------------------------------------------------
//  0x6F650480 - drain a send queue into the client.
//
//  The queue is asked for its buffer through its own vtable slot 9; the
//  first eight bytes are a header the client does not want, so the pointer
//  is advanced past them and the length reduced by the same eight.  An
//  empty remainder is not an error and answers silently; anything outside
//  (0, 0x400] is.
//----------------------------------------------------------------------------
void CNetSendQueue::Flush()
{
    void* buffer;
    unsigned int size;

    ((NetQueryBufferFn)(*(void***)this)[0x24 / 4])(this, &buffer, &size, 0);

    buffer = (char*)buffer + 8;
    size -= 8;
    if (size == 0)
        return;

    if (((0u < size ? -1 : 0) & ((0x400u < size) - 1)) == 0)
    {
        SErrSetLastError(kErrorInvalidParameter);
        return;
    }

    CNetClient* client = (CNetClient*)GetThreadLocalSlot(kThreadLocalNetClient);
    if (client != 0)
        client->SendPacket(buffer, size);
}

//----------------------------------------------------------------------------
//  0x6F6505C0 - hand the client an array of game results.
//
//  Three argument masks up front (a count in (0, 0x10], a non-null array)
//  and then a walk of the array *backwards* in twenty-byte steps checking
//  every element: its first byte in [1, 0x10] and its second word in
//  [3, 6).  Every failure lands on the same one-line error exit, which is
//  why the walk's four tests all branch to it rather than breaking out.
//----------------------------------------------------------------------------
void __fastcall SubmitGameResults(unsigned int count, void* results,
                                  const char* nameA, const char* nameB,
                                  void* summary)
{
    if ((((0u < count ? -1 : 0) & ((0x10u < count) - 1))
         & (results != 0 ? -1 : 0)) == 0)
    {
        SErrSetLastError(kErrorInvalidParameter);
        return;
    }

    if (count != 0)
    {
        //  The cursor starts one past the end and steps back before each
        //  read, which is what the shipped `lea eax,[esi+esi*4]` /
        //  `lea ecx,[edi+eax*4]` pair sets up.
        const char* cursor = (const char*)results + count * 20;
        unsigned int left = count;
        do
        {
            cursor -= 20;
            left--;

            if (*(const unsigned char*)cursor < 1
                || *(const unsigned char*)cursor > 0x10)
            {
                SErrSetLastError(kErrorInvalidParameter);
                return;
            }
            if (*(const int*)(cursor + 4) < 3
                || *(const int*)(cursor + 4) >= 6)
            {
                SErrSetLastError(kErrorInvalidParameter);
                return;
            }
        }
        while (left != 0);
    }

    void* client = GetThreadLocalSlot(kThreadLocalNetClient);
    if (client != 0)
        ((NetClientResultsFn)(*(void***)client)[0x84 / 4])(client, count,
                                                           results, nameA,
                                                           nameB, summary);
}

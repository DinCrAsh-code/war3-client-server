//============================================================================
//  0x6F6DF290 - NetConnSubmitAsyncOp.  A null-checked forwarder onto the
//  connection's real submit at 0x6F6DF230, which takes one more argument
//  than this does: the caller's `edx` becomes the third *stack* argument and
//  everything else shifts along.
//
//  `this` is passed on in ecx untouched, which is why the forwarder never
//  loads it.
//============================================================================
#include <windows.h>

void __stdcall SErrSetLastError(unsigned int error);

//: 0x6F6DF230 - retn 14h.  Five stack arguments plus ecx.
int __fastcall NetConnSubmitAsyncOpReal(void* conn, int a0, int a1, int b,
                                        int a2, unsigned int id);

int __fastcall NetConnSubmitAsyncOp(void* conn, int b, int a0, int a1,
                                    int a2, unsigned int id)
{
    if (conn == 0)
    {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return NetConnSubmitAsyncOpReal(conn, a0, a1, b, a2, id);
}

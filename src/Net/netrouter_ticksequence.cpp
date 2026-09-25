//============================================================================
//  0x6F663CA0 - a monotonic "next id" generator: dword_6FACFE7C +=
//  GetTickCount(), then += the caller's own delta, and the running total is
//  both stored back and returned - so repeated calls with delta 0 still
//  advance the counter (by whatever GetTickCount moved), and the delta lets
//  a caller fold in its own contribution in the same read-modify-write.
//  Own translation unit so its one call site (netprovider_slot25.cpp's own
//  BuildRecord) does not let /Ob2 fold it back in - the shipped body is a
//  genuine out-of-line call there.  `retn` bare - __fastcall(ecx=delta).
//============================================================================
extern "C" unsigned int __stdcall GetTickCount(void);
extern "C" int dword_6FACFE7C;

int __fastcall AdvanceRouterTickCounter(int delta)
{
    dword_6FACFE7C = (int)GetTickCount() + dword_6FACFE7C;
    dword_6FACFE7C += delta;
    return dword_6FACFE7C;
}

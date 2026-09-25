//============================================================================
//  0x6F6C5440 - a one-shot "pause new work" latch: atomically swap the
//  latch's own dword with 0 (releasing it) and hand back the previous
//  owner, poke it once via 0x6F6C5230 (a thiscall member whose own body is
//  outside this session's scope - `add ecx,0x14; jmp 0x6F6D8520`, i.e. it
//  forwards into a further object at +0x14), then re-arm the latch to the
//  previous owner and busy-wait up to 20ms, calling Sleep(0) once per lap
//  while the latch is still armed.
//
//  InterlockedExchangeAt is Sync/critsec.cpp's own `lock xchg` wrapper
//  (0x6F6C40A0), already reconstructed there.
//============================================================================
#include <windows.h>

long __fastcall InterlockedExchangeAt(volatile long* target, long value);   // critsec.cpp

//  0x6F6C5230 - `add ecx,0x14 / jmp 0x6F6D8520`: a thiscall forwarder into a
//  sub-object at +0x14 of whatever this poke's receiver is.  Outside this
//  batch's own 43 (0x6F6D8520 is deep Sync/thread-pool machinery, not
//  reconstructed this session) - declared and thunked to its own real
//  address so this call site is a normal direct `call`, matching the dump.
__declspec(naked) void __fastcall PokeThreadPoolOwner(void*)
{
    __asm { mov eax, 06F6C5230h }
    __asm { jmp eax }
}

extern long g_pauseLatch;    // dword_6FAD1408

void __fastcall PauseAndWaitBriefly()
{
    void* owner = (void*)InterlockedExchangeAt(&g_pauseLatch, 0);
    if (owner != 0)
    {
        PokeThreadPoolOwner(owner);

        InterlockedExchangeAt(&g_pauseLatch, (long)owner);

        DWORD start = GetTickCount();
        if (GetTickCount() - start < 0x14)
        {
            for (;;)
            {
                if (g_pauseLatch == 0)
                    break;
                Sleep(0);
                if (GetTickCount() - start >= 0x14)
                    break;
            }
        }
    }
}

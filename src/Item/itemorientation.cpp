//============================================================================
//  0x6F2B6270 - BuildItemGroundOrientation used to live here.  It now lives
//  in src/jasssetitemposition.cpp, `static`, next to its one caller, because
//  that is the only way to reproduce its calling convention: the shipped
//  function takes its 36-byte output matrix in ESI and cleans its three
//  float arguments itself (`retn 0Ch`), and at /O2 MSVC only invents a
//  convention like that for a function with internal linkage whose every
//  call site it can see.  See that file's own banner comment, and
//  docs/msvc-vc8-idioms.md, "A `static` function in its one caller's TU gets
//  a made-up calling convention".
//
//  This file stays, empty, rather than being deleted.  The real bug it works
//  around is fixed - build_mix.py now deletes build artifacts whose source
//  is gone - but that fix lives in matching_decomp_injection_win10, and an
//  empty translation unit costs nothing and keeps this tree buildable
//  against an older copy of it.  Deleting a .cpp from an incrementally-built
//  tree used to leave its stale .obj to be linked (LNK2019 against the old
//  __cdecl GetItemGroundOffset) and its stale .asm to be parsed, where the
//  old mangling collided with the new __fastcall one and silently renamed
//  the function out of funcmap.py's reach.  See
//  docs/notes/hook-abi-crash-classes.md.
//============================================================================

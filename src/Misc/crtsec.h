//============================================================================
//  The tiny slice of MSVC8's /GS runtime that sub_6F430C80's call tree
//  reaches: the per-call cookie check the compiler inserts ahead of any
//  __except_handler4 frame's epilogue.
//============================================================================
#ifndef CRTSEC_H
#define CRTSEC_H

//: dword_6FAAE140 - MSVC8's __security_cookie, xored into every
//: __except_handler4 frame's saved fs:[0] chain link (see
//: docs/msvc-vc8-idioms.md, "An __except_handler4 frame under /GS- /EHs-c-").
extern unsigned int g_securityCookie;

//  0x6F7E1B62 - the CRT's cookie-mismatch handler.  Defined in gsreport.cpp
//  from MSVC8's own gs_report.c (vs8/VC/crt/src/gs_report.c) - see that
//  file's header comment.  Declared here so SecurityCheckCookie() below has
//  a real symbol to tail-jump to.
void __report_gsfailure();

//  0x6F7E1059 - MSVC8's @_security_check_cookie@4: fall straight through on
//  a matching cookie, otherwise hand off to __report_gsfailure and never
//  return.  A fastcall parameter compared without ever being spilled to a
//  stack slot, and a tail jump, are not reachable from ordinary C++, so this
//  one is the disassembly transcribed - the same technique already used for
//  CFloatToInt/CountLeadingZeros in softfloat.cpp.
void __fastcall SecurityCheckCookie(unsigned int cookie);

#endif

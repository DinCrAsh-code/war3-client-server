//============================================================================
//  0x6F4AF4D0 - concatenate two CFloat 3x3 matrices.
//
//  Its own translation unit rather than sitting beside CMatrix3::RotateAxis
//  (0x6F4B00C0, cmatrix3_rotate.cpp), which is its only caller here: the
//  shipped build calls it for real (`call sub_6F4AF4D0`), so it must not be
//  inlined away, and the house rule is a separate TU rather than a
//  __declspec(noinline) the original never had.
//
//  Row-major, result[i][j] = sum_k a[i][k] * b[k][j] - pinned by the
//  disassembly's very first group, which multiplies a's +0x18/+0x1C/+0x20
//  (row 2) against b's +0x08/+0x14/+0x20 (column 2) and stores the sum at
//  the result's own +0x20.
//
//  The nine sums are the nine arguments of one CMatrix3 constructor call,
//  not nine named locals: MSVC evaluates arguments right to left, which is
//  exactly the order the shipped code computes them in - element 8 first,
//  element 0 last, then nine dereference-and-store pairs in ascending
//  address order at the end, which is the inlined constructor.  Splitting
//  them into statements computes them the other way round and costs the
//  match.
//============================================================================
#include "cmatrix3.h"

CMatrix3 __fastcall operator*(const CMatrix3& a, const CMatrix3& b)
{
    return CMatrix3(
        a.m_00 * b.m_00 + a.m_01 * b.m_10 + a.m_02 * b.m_20,
        a.m_00 * b.m_01 + a.m_01 * b.m_11 + a.m_02 * b.m_21,
        a.m_00 * b.m_02 + a.m_01 * b.m_12 + a.m_02 * b.m_22,
        a.m_10 * b.m_00 + a.m_11 * b.m_10 + a.m_12 * b.m_20,
        a.m_10 * b.m_01 + a.m_11 * b.m_11 + a.m_12 * b.m_21,
        a.m_10 * b.m_02 + a.m_11 * b.m_12 + a.m_12 * b.m_22,
        a.m_20 * b.m_00 + a.m_21 * b.m_10 + a.m_22 * b.m_20,
        a.m_20 * b.m_01 + a.m_21 * b.m_11 + a.m_22 * b.m_21,
        a.m_20 * b.m_02 + a.m_21 * b.m_12 + a.m_22 * b.m_22);
}

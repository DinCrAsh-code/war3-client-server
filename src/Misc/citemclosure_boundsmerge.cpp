//============================================================================
//  0x6F527E30 - a fully self-contained leaf found at BFS depth 11 out of
//  CItem's vtable slots 32/104 (see Misc/citemclosure_terrain_gates.cpp's
//  own header for the walk this is part of).  No vtable call, no Storm
//  allocation, no class identity to guess at - a real reconstruction.
//
//  Expands a four-int bounds record by another one, component-wise: the
//  first two ints (signed, `jge`) take the minimum of the two operands, the
//  last two (`jg`) take the maximum - the classic "grow this bounding box
//  to also cover that one" shape, most plausibly {minX, minY, maxX, maxY}.
//  The merged record is written into both `this` and a caller-supplied
//  output record, and the output pointer is handed back in `eax` - the
//  shipped stream never clobbers it after the last `mov eax, ...arg_0`, so
//  the member returns it rather than being `void`.  `retn 8`
//  (__thiscall, two stack args): __thiscall pushes right to left, and the
//  shipped stream reads the *output* pointer from the lower stack offset
//  (pushed last, i.e. declared first) and the *other* operand from the
//  higher one (pushed first, i.e. declared second) - `Union(out, other)`,
//  not the other way round.
//============================================================================

struct SBoundsRect4
{
    int m_min0;   // 0x00
    int m_min1;   // 0x04
    int m_max0;   // 0x08
    int m_max1;   // 0x0C

    SBoundsRect4* Union(SBoundsRect4* out, const SBoundsRect4* other);
};

SBoundsRect4* SBoundsRect4::Union(SBoundsRect4* out, const SBoundsRect4* other)
{
    int max1 = m_max1 > other->m_max1 ? m_max1 : other->m_max1;
    int max0 = m_max0 > other->m_max0 ? m_max0 : other->m_max0;
    int min1 = m_min1 < other->m_min1 ? m_min1 : other->m_min1;
    int min0 = m_min0 < other->m_min0 ? m_min0 : other->m_min0;

    m_min0 = min0;
    out->m_min0 = min0;
    m_min1 = min1;
    out->m_min1 = min1;
    m_max0 = max0;
    out->m_max0 = max0;
    m_max1 = max1;
    out->m_max1 = max1;

    return out;
}

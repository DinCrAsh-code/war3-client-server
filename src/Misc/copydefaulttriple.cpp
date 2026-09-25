//============================================================================
//  0x6F6C7A80 - copy a fixed three-dword default into `dst`, unless `dst`
//  is null.  Its only caller in this batch's own closure, sub_6F314120, is
//  itself not reconstructed (Unit/-neighbourhood address, out of this
//  batch's own 35), so nothing here says what class owns `dst` or what the
//  three dwords mean; the three globals carry no name anywhere in this
//  tree or in `agent_worktrees/names.json`.  Left generic rather than
//  guessed - only the touched three dwords are modelled, per CLAUDE.md's
//  own "only touched members get names" rule, applied here to a global
//  default record instead of a struct member.
//============================================================================

struct SDefaultTriple
{
    unsigned int m_a;
    unsigned int m_b;
    unsigned int m_c;
};

//  dword_6FAD163C / dword_6FAD1640 / dword_6FAD1644 - three consecutive,
//  otherwise-unnamed globals this is the only reader of in this closure.
extern const unsigned int g_defaultTripleA;   // dword_6FAD163C
extern const unsigned int g_defaultTripleB;   // dword_6FAD1640
extern const unsigned int g_defaultTripleC;   // dword_6FAD1644

void __fastcall CopyDefaultTriple(SDefaultTriple* dst)
{
    if (dst != 0)
    {
        dst->m_a = g_defaultTripleA;
        dst->m_b = g_defaultTripleB;
        dst->m_c = g_defaultTripleC;
    }
}

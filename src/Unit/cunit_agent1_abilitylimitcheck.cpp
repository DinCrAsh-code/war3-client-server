//============================================================================
//  0x6F0762A0 - a depth-1/2 CUnit-vtable-BFS leaf (cunit_agent1_worklist.md)
//  reached from five different ability-shaped callers (0x6F078970,
//  0x6F11B900, 0x6F155920, 0x6F279A90, 0x6F2844D0), none of them in this
//  batch and none sharing a committed class name here - so `this` is
//  modelled as a bare receiver rather than a guessed class. Its own vtable
//  slot 0x264/4 = 153 is past CUnit's own committed 118-slot table
//  (docs/targets/vtables/CUnit.txt), which is the tell that the receiver
//  is some other, deeper-hierarchy object (an ability instance, going by
//  its five callers), not CUnit itself - the two touched data fields at
//  +0x3C/+0x40/+0x44 are given generic names for exactly that reason
//  rather than borrowed from CWidget's own same-offset `m_settingNN`
//  quartet (widget.h), which would claim a class relationship this call
//  tree does not establish.
//============================================================================

//  A bare receiver: only the two vtable slots this function dispatches
//  through and the two data fields it reads are named; everything else is
//  an unnamed gap.  vtable_dispatch_audit.py has no committed
//  docs/targets/vtables/<Class>.txt for this type and reports it as
//  skipped rather than judged - the argument counts below come directly
//  from this function's own dump (the pushes before each `call`), not a
//  typedef guess.
typedef int (__thiscall *QueryCountFn)(void*);
typedef int (__thiscall *CheckRangeFn)(void*, int, int, int);

struct SAbilityLimitHost
{
    char m_reserved00[0x3C];
    int  m_limit3C;   // +0x3C
    int  m_limit40;   // +0x40
    int  m_limit44;   // +0x44
};

struct SAbilityLimitCheckLeaf { int __thiscall CheckCountAndRange(int a, int b, int expected); };

//  0x6F0762A0.  `retn 0Ch` - three stack ints.
int __thiscall SAbilityLimitCheckLeaf::CheckCountAndRange(int a, int b, int expected)
{
    SAbilityLimitHost* self = (SAbilityLimitHost*)this;

    if (expected != ((QueryCountFn)(*(void***)self)[0xA0 / 4])(self))
        return 0xDD;

    int result = ((CheckRangeFn)(*(void***)self)[0x264 / 4])(self, a, b, 3);
    if (result != 0)
        return result;

    if (self->m_limit40 > 0)
        return 0xDD;

    if (self->m_limit3C > 0)
        return 0xB9;

    return (self->m_limit44 > 0) ? 0xB9 : 0;
}

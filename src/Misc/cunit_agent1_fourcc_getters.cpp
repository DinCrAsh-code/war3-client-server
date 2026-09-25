//============================================================================
//  Bare FourCC-constant leaves - the fixed agile-type-id each of this
//  family's CAgentPtr_<tag> instantiations checks a candidate against
//  (agent.cpp's CAgentPtr::AssignChecked is the same shape against
//  GetAgileRootTypeId; these are its siblings, one leaf per fixed type).
//  Each is the identical `mov eax, <imm32> / retn` shape misc_trivial_getters.cpp
//  already documents for the same family - pulled in as depth-1/2 callees off
//  CUnit's own vtable BFS (cunit_agent1_worklist.md).  None of these has its
//  real ability/unit name recoverable from this call tree, so each keeps the
//  literal FourCC in its name rather than guessing a business meaning.
//============================================================================

//  0x6F021B40 - 'Aaha'
unsigned int GetFourCC_6F021B40()
{
    return 0x41616861;
}

//  0x6F022F10 - 'Aarm'
unsigned int GetFourCC_6F022F10()
{
    return 0x4161726D;
}

//  0x6F196E60 - 'Abrf'
unsigned int GetFourCC_6F196E60()
{
    return 0x41627266;
}

//  0x6F1988C0 - 'Abur'
unsigned int GetFourCC_6F1988C0()
{
    return 0x41627572;
}

//  0x6F199040 - 'Achl'
unsigned int GetFourCC_6F199040()
{
    return 0x4163686C;
}

//  0x6F194F90 - 'ACad'
unsigned int GetFourCC_6F194F90()
{
    return 0x41436164;
}

//  0x6F1D4640 - 'Adda'
unsigned int GetFourCC_6F1D4640()
{
    return 0x41646461;
}

//  0x6F07CCF0 - 'Advc'
unsigned int GetFourCC_6F07CCF0()
{
    return 0x41647663;
}

//  0x6F07F3B0 - 'Aetl'
unsigned int GetFourCC_6F07F3B0()
{
    return 0x4165746C;
}

//  0x6F024A90 - 'Adts'
unsigned int GetFourCC_6F024A90()
{
    return 0x41647473;
}

//  0x6F10E7A0 - 'ANpi'
unsigned int GetFourCC_6F10E7A0()
{
    return 0x414E7069;
}

//  0x6F1569A0 - 'ANth'
unsigned int GetFourCC_6F1569A0()
{
    return 0x414E7468;
}

//  0x6F195E20 - 'AEar'
unsigned int GetFourCC_6F195E20()
{
    return 0x41456172;
}

//  0x6F197260 - 'ANbs'
unsigned int GetFourCC_6F197260()
{
    return 0x414E6273;
}

//  0x6F1995C0 - 'ANca'
unsigned int GetFourCC_6F1995C0()
{
    return 0x414E6361;
}

//  0x6F07BE00 - 'AUdd'
unsigned int GetFourCC_6F07BE00()
{
    return 0x41556464;
}

//  0x6F07E0F0 - 'AOeq'
unsigned int GetFourCC_6F07E0F0()
{
    return 0x414F6571;
}

//  0x6F081130 - 'AHfs'
unsigned int GetFourCC_6F081130()
{
    return 0x41486673;
}

//  0x6F0829F0 - 'AOhx'
unsigned int GetFourCC_6F0829F0()
{
    return 0x414F6878;
}

//  0x6F110CE0 - 'AEmb'
unsigned int GetFourCC_6F110CE0()
{
    return 0x41456D62;
}

//  0x6F197D80 - 'ANrf'
unsigned int GetFourCC_6F197D80()
{
    return 0x414E7266;
}

//  0x6F195360 - 'ANsl'
unsigned int GetFourCC_6F195360()
{
    return 0x414E736C;
}

//  0x6F159CA0 - 'ANsq'
unsigned int GetFourCC_6F159CA0()
{
    return 0x414E7371;
}

//  0x6F159520 - 'AOwd'
unsigned int GetFourCC_6F159520()
{
    return 0x414F7764;
}

//  0x6F194980 - 'ANtm'
unsigned int GetFourCC_6F194980()
{
    return 0x414E746D;
}

//  0x6F080500 - 'ANia'
unsigned int GetFourCC_6F080500()
{
    return 0x414E6961;
}

//  0x6F0DCBA0 - 'AIim'
unsigned int GetFourCC_6F0DCBA0()
{
    return 0x4149696D;
}

//  0x6F0DD6A0 - 'AIfm'
unsigned int GetFourCC_6F0DD6A0()
{
    return 0x4149666D;
}

//  0x6F0800D0 - 'AIfu'
unsigned int GetFourCC_6F0800D0()
{
    return 0x41496675;
}

//  0x6F0DBC80 - 'AIlb'
unsigned int GetFourCC_6F0DBC80()
{
    return 0x41496C62;
}

//  0x6F0DD4A0 - 'AIrb'
unsigned int GetFourCC_6F0DD4A0()
{
    return 0x41497262;
}

//  0x6F07CB10 - 'AIda'
unsigned int GetFourCC_6F07CB10()
{
    return 0x41496461;
}

//  0x6F10FB70 - 'AIpm'
unsigned int GetFourCC_6F10FB70()
{
    return 0x4149706D;
}

//  0x6F1D6740 - 'AIsp'
unsigned int GetFourCC_6F1D6740()
{
    return 0x41497370;
}

//  0x6F0DB5A0 - 'AIfc'
unsigned int GetFourCC_6F0DB5A0()
{
    return 0x41496663;
}

//  0x6F0D9E60 - 'AIca'
unsigned int GetFourCC_6F0D9E60()
{
    return 0x41496361;
}

//  0x6F0D8FA0 - 'ANbu'. Already reconstructed and scored as cunit_agent4's
//  own GetFixedAgileTypeId_414E6275 (tools/funcmap.py); this copy is not
//  claimed there and exists only so cunit_agent1's own ANbu family compiles.
unsigned int GetFourCC_6F0D8FA0()
{
    return 0x414E6275;
}

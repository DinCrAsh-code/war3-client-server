# Extraction report: 05_MiniRtsDebugInfo.exe

- Backend: `ida`
- SHA256: `61efce715257f36bb5287ba7a6ad1b8d80e4c1cf9dc1f51494c14d13b91726a9`
- Architecture: x86

## Counts

- sections: 4
- imports: 49
- exports: 1
- functions: 487
- strings: 54
- vtables: 4

## Vtables / recovered classes

- `0x0041B0D0` **(unnamed)** (25 entries, rtti=None)
- `0x0041B13C` **(unnamed)** (2 entries, rtti=None)
- `0x0041D250` **(unnamed)** (30 entries, rtti=None)
- `0x0041D370` **const CUnit::`vftable'** (340 entries, rtti=ida_name)

## Top-level functions

- `0x00401000` ??3CObject@@SAXPAX@Z (size=67, src=ida)
- `0x00401050` _main (size=601, src=ida)
- `0x004012B0` ??0CGame@@QAE@XZ (size=236, src=ida)
- `0x004013A0` ??1CTimerQueue@@QAE@XZ (size=69, src=ida)
- `0x004013F0` ??1CReplay@@QAE@XZ (size=147, src=ida)
- `0x00401490` ??1CGame@@QAE@XZ (size=484, src=ida)
- `0x00401680` ??0?$TArray@UListener@CEventBus@@@@QAE@XZ (size=13, src=ida)
- `0x00401690` ??1?$TArray@UListener@CEventBus@@@@QAE@XZ (size=69, src=ida)
- `0x004016E0` ?DistSq@@YAMABUVec2@@0@Z (size=47, src=ida)
- `0x00401710` ?FootprintCenter@@YA?AUVec2@@HHH@Z (size=55, src=ida)
- `0x00401750` ?Init@CGame@@QAE_NIH@Z (size=2060, src=ida)
- `0x00401F60` ?SetupPlayers@CGame@@AAEXI@Z (size=233, src=ida)
- `0x00402050` ?SpawnStartingUnits@CGame@@AAEXH@Z (size=1294, src=ida)
- `0x00402560` ?CreateUnit@CGame@@QAEPAVCUnit@@HHABUVec2@@M_N@Z (size=571, src=ida)
- `0x004027A0` ?CreateItem@CGame@@QAEPAVCItem@@HABUVec2@@@Z (size=171, src=ida)
- `0x00402850` ?OnUnitDeath@CGame@@QAEXPAVCUnit@@0@Z (size=291, src=ida)
- `0x00402980` ?OnUnitDamagedEvent@CGame@@CAXABUEventData@@PAX@Z (size=38, src=ida)
- `0x004029B0` ?GetWidget@CGame@@QBEPAVCWidget@@I@Z (size=49, src=ida)
- `0x004029F0` ?GetUnit@CGame@@QBEPAVCUnit@@I@Z (size=63, src=ida)
- `0x00402A30` ?FindNearestTree@CGame@@QBEPAVCDestructable@@ABUVec2@@M@Z (size=282, src=ida)
- `0x00402B50` ?FindDropoff@CGame@@QBEPAVCUnit@@HABUVec2@@_N@Z (size=762, src=ida)
- `0x00402E50` ?FindNearestMine@CGame@@QBEPAVCGoldMine@@ABUVec2@@M@Z (size=621, src=ida)
- `0x004030C0` ?FindNearestItem@CGame@@QBEPAVCItem@@ABUVec2@@M@Z (size=596, src=ida)
- `0x00403320` ?IsVisibleTo@CGame@@QBE_NHPBVCUnit@@@Z (size=120, src=ida)
- `0x004033A0` ?UpdateTimeOfDay@CGame@@AAEXXZ (size=188, src=ida)
- `0x00403460` ?UpdateAI@CGame@@AAEXXZ (size=128, src=ida)
- `0x004034E0` ?UpdateUnits@CGame@@AAEXXZ (size=55, src=ida)
- `0x00403520` ?UpdateMissiles@CGame@@AAEXXZ (size=386, src=ida)
- `0x004036B0` ?UpdateFog@CGame@@AAEXXZ (size=206, src=ida)
- `0x00403780` ?DestroyUnit@CGame@@AAEXPAVCUnit@@@Z (size=206, src=ida)
- `0x00403850` ?CleanupDead@CGame@@AAEXXZ (size=231, src=ida)
- `0x00403940` ?ComputeChecksum@CGame@@QBEIXZ (size=675, src=ida)
- `0x00403BF0` ?SetDefeated@CGame@@QAEXH@Z (size=97, src=ida)
- `0x00403C60` ?CheckVictory@CGame@@QAEXXZ (size=199, src=ida)
- `0x00403D30` ?Tick@CGame@@QAEXXZ (size=288, src=ida)
- `0x00403E50` ?Run@CGame@@QAEIH@Z (size=249, src=ida)
- `0x00403F50` ?Shutdown@CGame@@QAEXXZ (size=425, src=ida)
- `0x00404100` ?Grow@?$TArray@PAVCMissile@@@@AAEXH@Z (size=66, src=ida)
- `0x00404150` ?Init@CPlayer@@QAEXHHHEABUVec2@@@Z (size=160, src=ida)
- `0x004041F0` ?Spend@CPlayer@@QAE_NHH@Z (size=30, src=ida)
- `0x00404210` ?DepositGold@CPlayer@@QAEHH@Z (size=66, src=ida)
- `0x00404260` ?HasFood@CPlayer@@QBE_NH@Z (size=32, src=ida)
- `0x00404280` ?MeetsRequirements@CPlayer@@QBE_NPBUUnitTypeData@@@Z (size=36, src=ida)
- `0x004042B0` ?IsEnemy@CPlayer@@QBE_NH@Z (size=49, src=ida)
- `0x004042F0` ?OnUnitCreated@CPlayer@@QAEXPBVCUnit@@_N@Z (size=68, src=ida)
- `0x00404340` ?OnUnitRemoved@CPlayer@@QAEXPBVCUnit@@@Z (size=104, src=ida)
- `0x004043B0` ?DamageBonus@CPlayer@@QBEMPBUUnitTypeData@@@Z (size=172, src=ida)
- `0x00404460` ?ArmorBonus@CPlayer@@QBEMPBUUnitTypeData@@@Z (size=108, src=ida)
- `0x004044D0` ?StructureCount@CPlayer@@QBEHXZ (size=50, src=ida)
- `0x00404510` ?MaxF@@YAMMM@Z (size=39, src=ida)
- ... and 437 more, see functions.json


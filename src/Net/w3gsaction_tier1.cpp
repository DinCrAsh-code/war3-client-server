//============================================================================
//  batch-B-w3gsaction_thunks - four adjacent opcode handlers
//  (0x6F667FC0-0x6F668030, Probably_W3GS_ActionHandler's cases 53/54/56/57)
//  that never touch `self` at all: each parses one small field out of the
//  caller's raw buffer through its own ParseXAction wrapper
//  (w3gsaction_parsers.cpp) and turns the parser's own success flag into
//  this dispatcher family's standard status pair (1 = reject, 2 = queued/
//  handled) via the shipped `neg/sbb/neg/add 1` boolean-invert-and-offset
//  idiom - `!parsed + 1`.
//============================================================================

int __fastcall ParseDword3Action_6F666CF0(void* src, unsigned int declaredLength, unsigned int* out);
int __fastcall ParseDwordAction_6F666DA0(void* src, unsigned int declaredLength, unsigned int* out);
int __fastcall ParseByteAction_6F666E50(void* src, unsigned int declaredLength, unsigned char* out);
int __fastcall ParseDwordAction_6F666F00(void* src, unsigned int declaredLength, unsigned int* out);

//  0x6F667FC0 - case 53. Three-dword field.
int __stdcall W3GSAction_6F667FC0(void*, unsigned int arg4, unsigned int arg8)
{
    unsigned int local[3];
    return ParseDword3Action_6F666CF0((void*)arg4, arg8, local) ? 2 : 1;
}

//  0x6F667FF0 - case 54. One dword field.
int __stdcall W3GSAction_6F667FF0(void*, unsigned int arg4, unsigned int arg8)
{
    unsigned int local;
    return ParseDwordAction_6F666DA0((void*)arg4, arg8, &local) ? 2 : 1;
}

//  0x6F668010 - case 56. One byte field.
int __stdcall W3GSAction_6F668010(void*, unsigned int arg4, unsigned int arg8)
{
    unsigned char local;
    return ParseByteAction_6F666E50((void*)arg4, arg8, &local) ? 2 : 1;
}

//  0x6F668030 - case 57. One dword field.
int __stdcall W3GSAction_6F668030(void*, unsigned int arg4, unsigned int arg8)
{
    unsigned int local;
    return ParseDwordAction_6F666F00((void*)arg4, arg8, &local) ? 2 : 1;
}

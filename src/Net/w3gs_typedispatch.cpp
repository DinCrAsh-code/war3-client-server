//============================================================================
//  0x6F663700 - dispatches a parsed W3GS message by its type byte (47..51 /
//  0x2F..0x33: the LAN game-list family - SearchGame, GameInfo, CreateGame,
//  RefreshGame, DecreateGame) into one handler apiece. `this` (whatever the
//  caller last set ecx to - the NetProviderLTCP instance,
//  w3gs_packetcheck.cpp) is never touched here and passes straight through
//  to whichever handler gets called, none of which this session
//  reconstructs (see docs/targets/ContainsW3GSBytePacketCheck.md) - only
//  this switch's own shape is established.
//============================================================================

//  Not reconstructed this session - only the calling shape (5 stack
//  arguments, `this` passed through in ecx untouched) is established.
extern "C" void __stdcall W3GS_SearchGame(void* arg0, void* arg4, void* data, int dataLen);
extern "C" void __stdcall W3GS_GameInfo(void* arg0, void* arg4, void* data, int dataLen);
extern "C" void __stdcall W3GS_CreateGame(void* arg0, void* arg4, void* data, int dataLen);
extern "C" void __stdcall W3GS_RefreshGame(void* arg0, void* arg4, void* data, int dataLen);
extern "C" void __stdcall W3GS_DecreateGame(void* arg0, void* arg4, void* data, int dataLen);

void __stdcall W3GSDispatchByType(void* arg0, void* arg4, int msgType, void* data, int dataLen)
{
    switch (msgType)
    {
    case 0x2F: W3GS_SearchGame(arg0, arg4, data, dataLen); break;
    case 0x30: W3GS_GameInfo(arg0, arg4, data, dataLen); break;
    case 0x31: W3GS_CreateGame(arg0, arg4, data, dataLen); break;
    case 0x32: W3GS_RefreshGame(arg0, arg4, data, dataLen); break;
    case 0x33: W3GS_DecreateGame(arg0, arg4, data, dataLen); break;
    default: break;
    }
}

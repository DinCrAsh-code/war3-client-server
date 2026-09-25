//============================================================================
//  0x6F665Fxx-0x6F666Fxx - a "ParseXAction" family: each builds a local,
//  non-owning `CDataStore` view over a caller-supplied raw buffer (fields
//  set directly, not through the real constructor - the same shape
//  ParseSingleDwordAction already establishes, w3gsaction2_parsesingledword.cpp),
//  calls exactly one already-reconstructed field reader to fill the
//  caller's own output struct, and reports success only if the reader
//  consumed *exactly* the declared length (`local.m_readPos == local.m_field10`).
//
//  Reached from the W3GSAction_* handlers in src/Net/w3gsaction_*.cpp, one
//  member per handler; each carries its own real (already-hookable) field
//  reader's address in its own comment below.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  Field readers this family calls into - all already reconstructed
//  elsewhere (see each function's own comment for its home file).
CDataStore* __fastcall NetMsgReadDword_6F683DF0(CDataStore* self, unsigned int* dst);              // netmsgio_683.cpp
CDataStore* __fastcall NetMsgReadWord_6F6844B0(CDataStore* self, unsigned short* dst);              // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadDwordDwordDword_6F684830(CDataStore* self, unsigned int* dst);     // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadDword_6F684870(CDataStore* self, unsigned int* dst);               // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadByte_6F684920(CDataStore* self, unsigned char* dst);                // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadDword_6F684940(CDataStore* self, unsigned int* dst);               // netmsgio_684.cpp
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C00(CDataStoreScratch* self, const void*);      // netmsgio_683_empty.cpp
CDataStore* __fastcall NetMsgReadByteDword_6F684380(CDataStore* self, unsigned char* dst);          // netmsgio_684.cpp
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C20(CDataStoreScratch* self, const void*);      // netmsgio_683_empty.cpp
CDataStore* __fastcall NetMsgReadDword_6F684350(CDataStore* self, unsigned int* dst);               // netmsgio_684.cpp
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C80(CDataStoreScratch* self, const void*);      // netmsgio_683_empty.cpp
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C40(CDataStoreScratch* self, const void*);      // netmsgio_683_empty.cpp
CDataStore* __fastcall NetMsgReadRawDwordDword_6F6843F0(CDataStore* self, unsigned char* dst);      // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadDword_6F6843B0(CDataStore* self, unsigned int* dst);               // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadDwordByteDword_6F684510(CDataStore* self, unsigned char* dst);     // netmsgio_684.cpp
CDataStore* __fastcall Reader_6F6851B0(CDataStore* self, unsigned char* dst);                       // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadByteByteDwordDword_6F684A20(CDataStore* self, unsigned char* dst); // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadByteByteDword_6F684A90(CDataStore* self, unsigned char* dst);      // netmsgio_684.cpp
CDataStore* __fastcall NetMsgReadDword_6F684330(CDataStore* self, unsigned int* dst);               // netmsgio_684.cpp

//  0x6F665FE0
int __fastcall ParseDwordAction_6F665FE0(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F683DF0(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666980
int __fastcall ParseWordAction_6F666980(void* src, unsigned int declaredLength, unsigned short* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadWord_6F6844B0(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666CF0
int __fastcall ParseDword3Action_6F666CF0(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDwordDwordDword_6F684830(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666DA0
int __fastcall ParseDwordAction_6F666DA0(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F684870(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666E50
int __fastcall ParseByteAction_6F666E50(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadByte_6F684920(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666F00
int __fastcall ParseDwordAction_6F666F00(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F684940(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666350 - the reader is one of the write-side family's own "no
//  fields" no-ops (NetMsgIoNoFields_6F683C00, netmsgio_683_empty.cpp): the
//  shipped body reuses the same one-instruction `return this` stub for both
//  read and write dispatch since it never touches the object either way, so
//  this call carries a plain reinterpret to the type that stub was already
//  declared with.
int __fastcall ParseNoFieldsAction_6F666350(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoNoFields_6F683C00((CDataStoreScratch*)&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666560
int __fastcall ParseByteDwordAction_6F666560(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadByteDword_6F684380(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666610 - see 0x6F666350 above for the reused no-fields stub.
int __fastcall ParseNoFieldsAction_6F666610(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoNoFields_6F683C20((CDataStoreScratch*)&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F6664B0
int __fastcall ParseDwordAction_6F6664B0(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F684350(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666820 - see 0x6F666350 above for the reused no-fields stub.
int __fastcall ParseNoFieldsAction_6F666820(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoNoFields_6F683C80((CDataStoreScratch*)&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F6666C0 - see 0x6F666350 above for the reused no-fields stub.
int __fastcall ParseNoFieldsAction_6F6666C0(void* src, unsigned int declaredLength, void* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgIoNoFields_6F683C40((CDataStoreScratch*)&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666770
int __fastcall ParseRawDwordDwordAction_6F666770(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadRawDwordDword_6F6843F0(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F6661F0
int __fastcall ParseDwordAction_6F6661F0(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F6843B0(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666AE0
int __fastcall ParseDwordByteDwordAction_6F666AE0(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDwordByteDword_6F684510(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666A30
int __fastcall ParseBigRecordAction_6F666A30(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    Reader_6F6851B0(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666B90
int __fastcall ParseByteByteDwordDwordAction_6F666B90(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadByteByteDwordDword_6F684A20(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666C40
int __fastcall ParseByteByteDwordAction_6F666C40(void* src, unsigned int declaredLength, unsigned char* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadByteByteDword_6F684A90(&local, out);
    return local.m_readPos == local.m_field10;
}

//  0x6F666400
int __fastcall ParseDwordAction_6F666400(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F684330(&local, out);
    return local.m_readPos == local.m_field10;
}

CDataStore* __fastcall NetMsgReadDword_6F684130(CDataStore* self, unsigned int* dst);

//  0x6F675BE0
int __fastcall ParseDwordAction_6F675BE0(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;
    NetMsgReadDword_6F684130(&local, out);
    return local.m_readPos == local.m_field10;
}

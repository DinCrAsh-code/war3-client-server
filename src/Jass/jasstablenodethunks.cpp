//============================================================================
//  See jasstablenodethunks.h for why every one of these stays a redirect.
//============================================================================
#include "jasstablenodethunks.h"
#include "storm.h"   // SMemFree

#define TABLENODE_THUNK(addr) \
    {                          \
        __asm { mov eax, addr } \
        __asm { jmp eax }        \
    }

//----------------------------------------------------------------------------
//  The real "two embedded TSLink members, unlink most-recently-declared
//  first" family (see jasstablenodethunks.h's own header comment) plus the
//  plain `Free(obj)` wrapper each one has.
//----------------------------------------------------------------------------
void FuncAddr::Dtor()
{
    m_reserved0C.Unlink();
    m_reserved04.Unlink();
}
void __stdcall FreeFuncAddr(FuncAddr* obj)
{
    obj->Dtor();
    SMemFree(obj, ".?AUFuncAddr@@", -2, 0);
}

void String2HandleReg::Dtor()
{
    m_reserved0C.Unlink();
    m_reserved04.Unlink();
}
void __stdcall FreeString2HandleReg(String2HandleReg* obj)
{
    obj->Dtor();
    SMemFree(obj, ".?AUString2HandleReg@@", -2, 0);
}

void FuncAddr2HandleReg::Dtor()
{
    m_reserved0C.Unlink();
    m_reserved04.Unlink();
}
void __stdcall FreeFuncAddr2HandleReg(FuncAddr2HandleReg* obj)
{
    obj->Dtor();
    SMemFree(obj, ".?AUFuncAddr2HandleReg@@", -2, 0);
}

void JassHandle::Dtor()
{
    m_reserved0C.Unlink();
    m_reserved04.Unlink();
}
void __stdcall FreeJassHandle(JassHandle* obj)
{
    obj->Dtor();
    SMemFree(obj, ".?AUJassHandle@@", -2, 0);
}

//  0x6F44BE40 - the extra owned-buffer free ahead of the two Unlink()s, its
//  own generic Storm header tag rather than this record's own type
//  descriptor (see the .h's own note).
void HashedString::Dtor()
{
    if (m_ownedBuffer != 0)
        SMemFree(m_ownedBuffer, "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA29, 0);
    m_reserved0C.Unlink();
    m_reserved04.Unlink();
}
void __stdcall FreeHashedString(HashedString* obj)
{
    obj->Dtor();
    SMemFree(obj, ".?AUHashedString@@", -2, 0);
}

//  ScriptData reuses the already-thunked ReleaseLocalSlot as its own base
//  release - see jasstablenodethunks.h's own note.
void __stdcall FreeScriptData(ScriptData* obj)
{
    ReleaseLocalSlot(obj);
    SMemFree(obj, ".?AUScriptData@@", -2, 0);
}

//---------------------------------------------------------------- NativeFunc
__declspec(naked) void __stdcall NewNativeFuncNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F44FC30h)
__declspec(naked) void __stdcall NewNativeFuncNodeAlias(void*, unsigned int, void*)
    TABLENODE_THUNK(06F4527B0h)
__declspec(naked) void NativeFuncNode::DeletingDestructor(unsigned int)
    TABLENODE_THUNK(06F44FB90h)
__declspec(naked) void NativeFuncTable::Destruct()
    TABLENODE_THUNK(06F451BC0h)

//-------------------------------------------------------------- HashedString
__declspec(naked) void __stdcall NewHashedStringNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F450270h)
__declspec(naked) void __stdcall NewHashedStringNodeAlias(void*, unsigned int, void*)
    TABLENODE_THUNK(06F452C70h)
__declspec(naked) void HashedStringTable::Destruct()
    TABLENODE_THUNK(06F450370h)
__declspec(naked) void HashedStringTable::ConstructComposite()
    TABLENODE_THUNK(06F450410h)

//----------------------------------------------------------------- ScriptData
__declspec(naked) void __stdcall NewScriptDataNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F450530h)
__declspec(naked) void __stdcall NewScriptDataNodeAlias(void*, unsigned int, void*)
    TABLENODE_THUNK(06F452DF0h)
__declspec(naked) void ScriptDataTable::Destruct()
    TABLENODE_THUNK(06F450640h)
__declspec(naked) void ScriptDataTable::ConstructComposite()
    TABLENODE_THUNK(06F4506E0h)

//------------------------------------------------------------------- FuncAddr
__declspec(naked) void __stdcall NewFuncAddrNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F4509A0h)
__declspec(naked) void __stdcall NewFuncAddrNodeAlias(void*, unsigned int, void*)
    TABLENODE_THUNK(06F452FC0h)
__declspec(naked) void FuncAddrTable::Destruct()
    TABLENODE_THUNK(06F450AA0h)
__declspec(naked) void FuncAddrTable::ConstructComposite()
    TABLENODE_THUNK(06F450B40h)

//----------------------------------------------------------------- JassHandle
__declspec(naked) void __stdcall NewJassHandleNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F450CA0h)
__declspec(naked) void __stdcall NewJassHandleNodeAlias(void*, unsigned int, void*)
    TABLENODE_THUNK(06F4530F0h)
__declspec(naked) void JassHandleTableDtor::Destruct()
    TABLENODE_THUNK(06F450DA0h)
__declspec(naked) void JassHandleTableDtor::ConstructComposite()
    TABLENODE_THUNK(06F450E40h)

//------------------------------------------------------------ String2HandleReg
__declspec(naked) void __stdcall NewString2HandleNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F451100h)
__declspec(naked) void __stdcall NewString2HandleNodeAlias(void*, unsigned int, void*)
    TABLENODE_THUNK(06F453220h)
__declspec(naked) void String2HandleTableDtor::Destruct()
    TABLENODE_THUNK(06F451200h)
__declspec(naked) void String2HandleTableDtor::ConstructComposite()
    TABLENODE_THUNK(06F4512A0h)

//-------------------------------------------------------- FuncAddr2HandleReg
__declspec(naked) void __stdcall NewFuncAddr2HandleNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F451560h)
__declspec(naked) void __stdcall NewFuncAddr2HandleNodeAlias(void*, unsigned int, void*)
    TABLENODE_THUNK(06F453350h)
__declspec(naked) void FuncAddr2HandleTableDtor::Destruct()
    TABLENODE_THUNK(06F451660h)

//---------------------------------------------------------- FuncAddr2NameReg
__declspec(naked) void __stdcall NewFuncAddr2NameNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F4519C0h)
__declspec(naked) void __stdcall FreeFuncAddr2NameReg(void*)
    TABLENODE_THUNK(06F451A50h)
__declspec(naked) void __stdcall ResolveFuncAddr2NameRecord(void*)
    TABLENODE_THUNK(06F453410h)
__declspec(naked) void FuncAddr2NameTableDtor::Destruct()
    TABLENODE_THUNK(06F451AE0h)

//---------------------------------------------------------- Script2HandleReg
__declspec(naked) void __stdcall NewScript2HandleNode(void*, unsigned int, void*)
    TABLENODE_THUNK(06F452A80h)
__declspec(naked) void Script2HandleTableDtor::Destruct()
    TABLENODE_THUNK(06F4535A0h)

#undef TABLENODE_THUNK

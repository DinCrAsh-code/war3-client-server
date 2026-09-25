//============================================================================
//  0x6F00BAB0 - GameMain's own first call (Game/gamemain.cpp).  Loosens the
//  current process's DACL so a lower-privilege instance of the game can
//  still open it: build the well-known "Everyone" SID
//  (SECURITY_WORLD_SID_AUTHORITY, one subauthority of 0), an ACL denying it
//  every access right (0xF01FFFFE), and stamp that ACL onto the process
//  handle via SetSecurityInfo - all four advapi32 entry points resolved
//  dynamically through ResolveModuleProc (gamemaingetprocaddr.cpp) rather
//  than statically imported, which is why the module is LoadLibraryA'd by
//  name first.
//
//  Every one of the four resolved calls is genuinely indirect in the dump
//  (`call [esp+...]` through the slot ResolveModuleProc just filled), so
//  each is modelled as a local function-pointer variable rather than a
//  direct call - matching CLAUDE.md's vtable_dispatch_audit.py guidance for
//  a hand-written indirect call: the pointer type here is the real Win32
//  prototype, not a guess from the call site.
//============================================================================
#include <windows.h>
#include <aclapi.h>

int __fastcall ResolveModuleProc(HMODULE hModule, const char* procName, void** out);   // gamemaingetprocaddr.cpp

typedef BOOL (WINAPI *AllocateAndInitializeSidFn)(
    PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
    BYTE nSubAuthorityCount,
    DWORD nSubAuthority0, DWORD nSubAuthority1, DWORD nSubAuthority2, DWORD nSubAuthority3,
    DWORD nSubAuthority4, DWORD nSubAuthority5, DWORD nSubAuthority6, DWORD nSubAuthority7,
    PSID* pSid);
typedef BOOL (WINAPI *InitializeAclFn)(PACL pAcl, DWORD nAclLength, DWORD dwAclRevision);
typedef BOOL (WINAPI *AddAccessDeniedAceFn)(PACL pAcl, DWORD dwAceRevision, DWORD AccessMask, PSID pSid);
typedef DWORD (WINAPI *SetSecurityInfoFn)(HANDLE handle, SE_OBJECT_TYPE ObjectType,
                                          SECURITY_INFORMATION SecurityInfo,
                                          PSID psidOwner, PSID psidGroup,
                                          PACL pDacl, PACL pSacl);

int __fastcall SetupWorldSecurityDescriptor()
{
    SID_IDENTIFIER_AUTHORITY worldAuthority = { {0, 0, 0, 0, 0, 1} };   // SECURITY_WORLD_SID_AUTHORITY
    PSID pSid = 0;
    int result = 0;

    HANDLE hProcess = GetCurrentProcess();
    HMODULE hAdvapi32 = LoadLibraryA("advapi32.dll");
    if (hAdvapi32)
    {
        AllocateAndInitializeSidFn pAllocateAndInitializeSid;
        InitializeAclFn pInitializeAcl;
        AddAccessDeniedAceFn pAddAccessDeniedAce;
        SetSecurityInfoFn pSetSecurityInfo;

        if (ResolveModuleProc(hAdvapi32, "AllocateAndInitializeSid", (void**)&pAllocateAndInitializeSid) &&
            ResolveModuleProc(hAdvapi32, "InitializeAcl", (void**)&pInitializeAcl) &&
            ResolveModuleProc(hAdvapi32, "AddAccessDeniedAce", (void**)&pAddAccessDeniedAce) &&
            ResolveModuleProc(hAdvapi32, "SetSecurityInfo", (void**)&pSetSecurityInfo))
        {
            char aclBuffer[0x200];

            if (pAllocateAndInitializeSid(&worldAuthority, 1, 0, 0, 0, 0, 0, 0, 0, 0, &pSid) &&
                pInitializeAcl((PACL)aclBuffer, sizeof(aclBuffer), ACL_REVISION) &&
                pAddAccessDeniedAce((PACL)aclBuffer, ACL_REVISION, 0xF01FFFFE, pSid))
            {
                if (pSetSecurityInfo(hProcess, SE_KERNEL_OBJECT,
                                     DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
                                     0, 0, (PACL)aclBuffer, 0) == 0)
                {
                    result = 1;
                }
            }
        }

        FreeLibrary(hAdvapi32);
    }

    if (pSid)
        FreeSid(pSid);

    return result;
}

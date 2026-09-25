//============================================================================
//  JASS_SetItemPosition_callee1's own Storm/Terrain-gateway frontier - see
//  docs/notes/jass-setitemposition-storm-gates.md for the BFS methodology
//  and why each of these is a redirect rather than a reconstruction.
//
//  Each function below is directly reached only through
//  ConstructStormSingletonA (0x6F768D10, lazysingleton.cpp, already a
//  redirect thunk) and its own subtree confirms it: NTempest-namespaced
//  vtables, TSHashTable<TerrainImageTextureHash>/TSHashTable<ShadowImageHash>
//  instantiations, config keys unrelated to item placement ("Terrain",
//  "Light"/"Direction", "FoggedTerrain"/"FogOfWar", "AUOBJECTCHANCE"), and
//  in one case a tail-call straight into the Concurrency Runtime's own
//  internals - none of it item-positioning logic, all of it exactly the
//  "deep, non-interesting chunk" CLAUDE.md's "work outward... not into
//  Storm's internals" already tells this repo to stop at.  Same pattern as
//  ConstructStormSingletonA/EnsureStormSingletonACapacity/
//  DefaultMoveFilter/PathPlacementFilter: a direct `mov eax,ADDR / jmp eax`
//  redirect into the game's own code at its real address, so a build
//  hooking anything else in this target still calls the genuine original
//  rather than link-failing on an unresolved symbol.  0% instruction match
//  by construction and not worth chasing further - see each BEHAVIOUR
//  entry in tools/funcmap.py.
//
//  A later session re-audited every one of these against its own dump's
//  `retn` and register use, the same check that found seven wrong slot
//  signatures in the TRefCnt..CItem chain (docs/msvc-vc8-idioms.md, "A
//  naked thunk has no `ret`").  Two results worth knowing:
//
//  * **Twelve of these are thiscall members, not `__cdecl` free
//    functions.**  Their bodies take `this` in ecx (`mov esi, ecx`, or a
//    direct `[ecx+...]` read) even though nothing pushes it here.  They
//    are left spelled `__cdecl ()` because *nothing in `src/` calls any of
//    them* - grep says zero references outside this file - so no call with
//    the wrong convention is ever generated.  The declaration exists only
//    to give `build_mix.py` a symbol at the right address.  If you ever
//    add a real call site to one of these, fix its declaration first.
//  * **Two of them also clean the stack**, which is the crash class
//    `abi_audit.py` calls FATAL rather than a convention nicety, so those
//    two are spelled honestly below (`SStormTerrainReleaseTarget`).
//============================================================================

//  The two gates whose shipped body is `retn 4` - one stack argument on top
//  of `this` in ecx.  Declared `void __cdecl X()` they would have the callee
//  pop four bytes no caller pushed.  Nothing calls them today, but a wrong
//  `retn` is the one thing in this file that is a crash rather than an
//  inaccuracy, so both are members of a shape-only struct - the same
//  spelling item_radius.cpp's SPositionRadiusSink uses for a thiscall
//  callee whose real class this repo has not identified.  It is a claim
//  about the calling convention, not about a class named this existing.
struct SStormTerrainReleaseTarget
{
    //  0x6F765240 - `mov esi, ecx`, reads [esp+arg_0], `retn 4`.
    void ReleaseGate4(int which);
    //  0x6F74B550 - same shape; its subtree stamps NTempest::CMemBlock and
    //  NTempest::CEntity vtables (a rendering-engine destructor chain).
    void ReleaseEntity(int which);
};

__declspec(naked) void __cdecl StormTerrainReleaseGate1()
{
    __asm
    {
        mov eax, 06F767D80h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainReleaseGate2()
{
    __asm
    {
        mov eax, 06F768090h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainReleaseGate3()
{
    __asm
    {
        mov eax, 06F767C40h
        jmp eax
    }
}

//  0x6F71C2F0 - an SEH-framed lazy-init shape (its own `dword_6FADB0E4`
//  guard, `push offset SEH_6F71C2F0`), the sub-subsystem
//  StormTerrainReleaseGate1/2/3 each call into before their own release
//  loop.
__declspec(naked) void __cdecl StormTerrainSubsystemInitGate()
{
    __asm
    {
        mov eax, 06F71C2F0h
        jmp eax
    }
}

__declspec(naked) void SStormTerrainReleaseTarget::ReleaseGate4(int)
{
    __asm
    {
        mov eax, 06F765240h
        jmp eax
    }
}

//  0x6F74C2D0 - reads config under "Terrain" plus a "revwarnings" table.
__declspec(naked) void __cdecl StormTerrainWarningsGate()
{
    __asm
    {
        mov eax, 06F74C2D0h
        jmp eax
    }
}

__declspec(naked) void SStormTerrainReleaseTarget::ReleaseEntity(int)
{
    __asm
    {
        mov eax, 06F74B550h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate1()
{
    __asm
    {
        mov eax, 06F761C90h
        jmp eax
    }
}

//  0x6F74E750 - calls the already-established GetDefaultConfigInt, but its
//  own subtree also reaches a Storm-hashtable growth path keyed on the type
//  descriptor "AUOBJECTCHANCE" - unrelated to item placement, not worth
//  splitting the config call out on its own.
__declspec(naked) void __cdecl StormTerrainAutoObjectConfigGate()
{
    __asm
    {
        mov eax, 06F74E750h
        jmp eax
    }
}

//  0x6F7511A0 - tail-calls into a "FoggedTerrain"/"FogOfWar" config reader.
__declspec(naked) void __cdecl StormFogTerrainGate()
{
    __asm
    {
        mov eax, 06F7511A0h
        jmp eax
    }
}

//  0x6F742A50 - a day/night "Light"/"Direction" angle-to-lightmap-index
//  calculation; reuses GetDefaultConfigVec3/FVec3::Normalize but the rest
//  of its own body is terrain-lighting arithmetic this session did not
//  pursue further.
__declspec(naked) void __cdecl StormTerrainLightDirectionGate()
{
    __asm
    {
        mov eax, 06F742A50h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate2()
{
    __asm
    {
        mov eax, 06F7625E0h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate3()
{
    __asm
    {
        mov eax, 06F762500h
        jmp eax
    }
}

//  0x6F749100 - stamps NTempest::CPriorityQ<TerrainLayerPriority,...>'s own
//  vtable then tail-calls into the CMemBlock/CEntity destructor chain.
__declspec(naked) void __cdecl StormTerrainEntityDtorGate()
{
    __asm
    {
        mov eax, 06F749100h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate4()
{
    __asm
    {
        mov eax, 06F7560B0h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate5()
{
    __asm
    {
        mov eax, 06F756090h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate6()
{
    __asm
    {
        mov eax, 06F7560C0h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate7()
{
    __asm
    {
        mov eax, 06F7560A0h
        jmp eax
    }
}

__declspec(naked) void __cdecl StormTerrainArrayReleaseGate8()
{
    __asm
    {
        mov eax, 06F751020h
        jmp eax
    }
}

//  0x6F75F640 - stamps TSHashTable<TerrainImageTextureHash,HASHKEY_STRI>'s
//  own vtable directly in its own first instruction.
__declspec(naked) void __cdecl StormTerrainImageTextureHashGate()
{
    __asm
    {
        mov eax, 06F75F640h
        jmp eax
    }
}

//  0x6F75F320 - same shape, TSHashTable<ShadowImageHash,HASHKEY_STRI>.
__declspec(naked) void __cdecl StormShadowImageHashGate()
{
    __asm
    {
        mov eax, 06F75F320h
        jmp eax
    }
}

//  0x6F525F10 - not Storm/Terrain at all: a plain tail-call straight into
//  the Concurrency Runtime's own internals
//  (Concurrency::details::UMSThreadVirtualProcessor::GetDefaultDestination),
//  reached from this dump's own generic-container allocator fallback path.
//  Just as out of scope, for the same reason.
__declspec(naked) void __cdecl ConcurrencyRuntimeThreadGate()
{
    __asm
    {
        mov eax, 06F525F10h
        jmp eax
    }
}

//  __ftol2_sse (0x6F7E11E0) - the compiler's own float-to-int CRT helper.
//  Unlike _CIsqrt/_CIcos/_CIsin (one-instruction `jmp ds:__imp_*` thunks
//  the linker writes), this one has a real, several-instruction body - but
//  vs8/ ships only the compiled .obj for it
//  (vs8/VC/crt/src/intel/dll_lib/ftol2.obj), not source the way
//  gsreport.cpp's ___report_gsfailure had, so there is nothing to compile
//  it *from*.  Any C++ elsewhere in this repo that truncates a float to an
//  int already emits a call to this same symbol automatically; this
//  redirect is what lets that call resolve without inventing a body.
__declspec(naked) void __cdecl CrtFloatToIntHelperGate()
{
    __asm
    {
        mov eax, 06F7E11E0h
        jmp eax
    }
}

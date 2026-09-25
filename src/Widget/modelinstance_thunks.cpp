//============================================================================
//  The model layer's callees this slice reaches but does not reconstruct.
//  Same rules as the other *_thunks.cpp here: each body jumps to the shipped
//  function at its real address, so an unreconstructed callee simply runs
//  original code, and each one goes away as its real body lands.
//
//  NOT reconstructions.  Never offer them as hookable.
//
//  Every `retn <n>` below was read off the shipped body with dumpfn.py.
//============================================================================
#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F511F10 - apply a resolved block index and value to the model's
//  track set.  CModelAnim::SetTrackValue (modelinstance.cpp) is its only
//  caller: `this` in ecx, the track set (still live in edx from the
//  caller's own `mov edx,[ecx+44h]`, never reloaded here) as the second
//  argument, and the block index and value as two stack dwords - `retn 8`.
//  120 instructions of per-block flag/interpolation bookkeeping, well
//  outside this session's scope.
class CModelTrackBlockSetter
{
public:
    void __fastcall ApplyTrackValue(void* trackSet, unsigned int block,
                                    int value);
};
ADDR_THUNK(void __fastcall
           CModelTrackBlockSetter::ApplyTrackValue(void*, unsigned int, int),
           0x6F511F10)

//  0x6F505890 - fill `out` with everything one sequence of this model
//  instance holds, and say whether there was one.  `retn 4`: the instance in
//  ecx, the sequence index in edx and only the record pushed.
//
//  It is not a reconstruction and cannot become one.  Its last act is
//
//      lea     eax, [ecx+eax*4]
//      call    sub_6F505010
//
//  with the record still in *edi* - two arguments in registers no MSVC
//  calling convention names, which is the whole-program convention
//  tools/unhookable.py catalogues for CConfigFile::GetIntValue and ValueAt.
//  Redirected, so the real shipped body runs.
class CModelInstance;
struct SModelHandleSet;
ADDR_THUNK(int __fastcall QueryModelSequence(CModelInstance*, unsigned int,
                                             SModelHandleSet*), 0x6F505890)

//  0x6F501930 - `retn 4`, and it reads neither ecx nor edx: a plain
//  __stdcall lookup of a model record by its specification.
ADDR_THUNK(void* __stdcall FindModelRecord(const void*), 0x6F501930)

//  0x6F4FF9C0 - `retn 0`: take a handle on the model object, registering it
//  under the "HMODEL" handle type if it is not registered yet.
ADDR_THUNK(void* __fastcall AcquireModelObjectHandle(void*), 0x6F4FF9C0)

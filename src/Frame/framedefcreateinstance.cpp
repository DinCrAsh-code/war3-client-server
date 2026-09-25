//============================================================================
//  0x6F5C94D0 - CreateSimpleFrameFromDef: build one simple frame from a
//  definition the registry has already resolved.
//
//  Was a thunk (Frame/framethunks.cpp); the redirect is deleted in the same
//  change, so one shipped address still carries one C++ name.
//
//  The definition caches its create handler at +0x12C, and a definition that
//  has not resolved one yet does it here - by the type name it keeps at
//  +0x1C and the kind at +0x2C - but **does not write the answer back**.
//  That is the shipped body: the cache is filled somewhere else, and this
//  function re-resolves on every call until it is.  A definition whose type
//  name does not resolve to a handle at all is looked up under the empty
//  string rather than skipped.
//
//  The handler makes the frame; the definition's own properties go in after
//  it (+0x28 straight into the frame's +0x78, then everything else through
//  0x6F5CCC80); and the frame gets the last word through slot 0x24, which
//  is handed the caller's context and the context the handler lookup
//  produced.  A frame that answers zero there is thrown away through slot
//  8 - argument 1, i.e. "free the storage too" - and the caller gets null.
//
//  Three hand-written dispatches, none of them on `this`, so
//  tools/vtable_dispatch_audit.py cannot judge any of them and there is no
//  committed vtable for CSimpleFrame to check them against.  What fixes
//  each argument count is the shipped call site: the handler at 0x6F5C950E
//  pushes nothing and sets only ecx, slot 0x24 at 0x6F5C9533 pushes exactly
//  two dwords, and slot 8 at 0x6F5C9542 pushes exactly one.
//
//  Own translation unit: four real calls out of it.
//============================================================================
#include "framereg.h"
#include "frame.h"
#include "handleobject.h"

//  Slot 0x24 - "you have your properties; do you want to live?"  Non-zero
//  is yes.
typedef int  (__thiscall *SimpleFrameAcceptFn)(void* self, void* context,
                                               void* handlerContext);
//  Slot 0x08 - destroy; the one argument is the "free the storage" flag.
typedef void (__thiscall *SimpleFrameDestroyFn)(void* self, int freeStorage);

//  The empty string the shipped code substitutes for an unresolvable type
//  name - IDA's `ValueName`, four zero bytes at 0x6F87529C, the same shared
//  constant Frame/backdropgenerate.cpp already records.
static const char kEmptyName[] = "";

CSimpleFrame* __fastcall CreateSimpleFrameFromDef(BASEFRAMEHASHNODE* def,
                                                  CSimpleFrame* parent,
                                                  void* context)
{
    void* handlerContext = 0;

    SimpleFrameCreateFn handler =
        (SimpleFrameCreateFn)def->m_simpleCreateHandler;

    if (handler == 0)
    {
        //  The kind is read into its own local *before* the name lookup,
        //  which is what the shipped stream does (`mov esi, [edi+2Ch]` sits
        //  above the `lea`/`call` pair).  Left inside the resolve call's
        //  argument list MSVC reads it after, and the function loses a
        //  callee-saved register with it.
        void* kind = def->m_createKind;

        const char* name =
            (const char*)GetHandleOrZero((const SHandleHolder*)def->m_typeName);
        if (name == 0)
            name = kEmptyName;

        handler = ResolveSimpleFrameCreateHandler(name, kind,
                                                  &handlerContext);
    }

    CSimpleFrame* frame = handler(parent);

    //  +0x78 is written before the property push and directly, not through
    //  it: the shipped code reloads the definition's +0x28 straight into the
    //  frame.
    *(void**)((char*)frame + 0x78) = def->m_createArg;

    ApplyDefToSimpleFrame(def, context, frame);

    if (((SimpleFrameAcceptFn)(*(void***)frame)[0x24 / 4])(
            frame, context, handlerContext) == 0)
    {
        ((SimpleFrameDestroyFn)(*(void***)frame)[0x08 / 4])(frame, 1);
        return 0;
    }

    return frame;
}

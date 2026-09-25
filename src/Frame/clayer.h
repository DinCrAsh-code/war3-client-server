//============================================================================
//  CLayer and CLayer::LAYERNODE - the frame framework's parent/child tree.
//
//  Both names are the shipped binary's own, and so is the node's base class.
//  0x6F5FF4B0 and 0x6F5FF5F0 stamp `??_7LAYERNODE@CLayer@@6B@` (0x6F96D9B8)
//  into every node they allocate and hand Storm the node's RTTI descriptor
//  name `".?AULAYERNODE@CLayer@@"` (0x6FA9A4B0) as its allocation tag; the
//  RTTI base chain beside it (`??_R13?0A@EA@?$TSLinkedNode@ULAYERNODE@CLayer
//  @@@@8`) says LAYERNODE derives from TSLinkedNode<LAYERNODE>, which is what
//  puts the vftable at +0x00 and the intrusive link at +0x04 and makes every
//  list of them a TSList with a *compile-time* link offset of 4.
//
//  Only the fields this closure touches are named.  A layer is much bigger
//  than what is here - +0xA8 and +0xB0 are read with nothing else around
//  them - so the gaps are exact and unnamed.
//============================================================================
#ifndef CLAYER_H
#define CLAYER_H

#include "storm.h"
#include "dataallocator.h"
#include "observer.h"

class CLayer;

//: 0x6F96D9B8 - LAYERNODE's real vtable.  Written as its address, not built
//: from a `virtual` declaration: doing that would make this build stamp a
//: vtable of its own, which the instruction score cannot see and
//: abi_audit.py reports as VTABLE SYMBOL / WRONG CLASS.  See
//: docs/msvc-vc8-idioms.md, "Two vtable pointers a build must write as
//: literals".
const unsigned int kLayerNodeVftable = 0x6F96D9B8u;

//: 0x6F96D9EC / 0x6F96D9D8 - CLayer's own vtable and its shortcut table's,
//: both written as literals for the reason this file's LAYERNODE comment
//: gives.  kCLayerVftable is kept for the record only: CLayer derives from
//: the polymorphic CObserver, so MSVC stamps ??_7CLayer@@6B@ itself and no
//: source in this repo writes the address.
const unsigned int kCLayerVftable = 0x6F96D9ECu;
const unsigned int kShortcutRegTableVftable = 0x6F96D9D8u;

//----------------------------------------------------------------------------
//  A TSList<T,0> with **no constructor of its own**, initialised by an
//  explicit Init() instead.
//
//  That is not tidiness: a member's constructor runs before its owner's
//  constructor *body*, and CSimpleFrame's shipped constructor initialises its
//  seven lists in the middle of the body, interleaved with the float stores.
//  With a real constructor MSVC has to emit all forty-two of their
//  instructions up front and every displacement after them shifts.  The
//  accessors and the layout are storm.h's TSList's, field for field.
//----------------------------------------------------------------------------
//  A TSLink<T> with no constructor, so that neither this nor its owner gets
//  an implicit one - a member with a user-provided constructor is enough to
//  put a `vector constructor iterator' back over the five-element array.
template <class T>
struct SRawLink
{
    TSLink<T>*  m_next;
    int         m_prevlink;
};

template <class T, int LINKOFFSET = 0>
class CFrameNodeList
{
public:
    //  The dead `m_prevlink = 0` first is TSLink's own default constructor,
    //  which the shipped code emits and then overwrites.
    void Init()
    {
        m_terminator.m_prevlink = 0;
        m_terminator.m_next = (TSLink<T>*)&m_terminator;
        m_linkoffset = 0;
        m_terminator.m_prevlink = ~(int)&m_terminator;
    }

    TSLink<T>* Link(T* ptr)
    {
        if (ptr)
            return (TSLink<T>*)(LINKOFFSET + (int)ptr);
        return (TSLink<T>*)&m_terminator;
    }

    int TailLink()   { return Link(0)->m_prevlink; }
    T*  Tail()       { int t = TailLink(); return t > 0 ? (T*)t : 0; }
    T*  Prev(T* ptr) { return (T*)Link(ptr)->m_prevlink; }

    //  Forced for the same reason storm.h's own are: /Ob2's cost model
    //  declines a body this size and the shipping build plainly inlined it.
    __forceinline void LinkToHead(T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        TSLink<T>* head = m_terminator.m_next;
        link->m_next = head;
        link->m_prevlink = head->m_prevlink;
        head->m_prevlink = (int)ptr;
        m_terminator.m_next = link;
    }

    __forceinline void LinkAfter(TSLink<T>* where, T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        TSLink<T>* next = where->m_next;
        link->m_next = next;
        link->m_prevlink = next->m_prevlink;
        next->m_prevlink = (int)ptr;
        where->m_next = link;
    }

    int          m_linkoffset;  // 0x00
    SRawLink<T>  m_terminator;  // 0x04
};

//----------------------------------------------------------------------------
//  One membership of one layer list.  It is refcounted glue and not the
//  layer itself: the node owns a reference on the layer it names, and its
//  own vtable slot 0 is the deleting destructor the removers dispatch
//  through.
//----------------------------------------------------------------------------
struct LAYERNODE
{
    //  m_link is default-constructed before the body runs, which is what
    //  puts its two zero stores *ahead* of the vtable store in the shipped
    //  stream; writing all four in a member-initialiser list would put the
    //  vtable first instead.
    LAYERNODE()
    {
        m_vtable = (void*)kLayerNodeVftable;
        m_pLayer = 0;
    }

    void*             m_vtable;     // +0x00 - ??_7LAYERNODE@CLayer@@6B@
    TSLink<LAYERNODE> m_link;       // +0x04
    CLayer*           m_pLayer;     // +0x0C
};

//  The node's own vtable slot 0 - the scalar deleting destructor every
//  refcounted object in this binary carries there.
typedef void (__thiscall *LayerNodeDeleteFn)(void* self, int flags);
//  A layer's vtable slot 0xA4, dispatched when its enabled state changes.
typedef void (__thiscall *LayerEnabledChangedFn)(void* self);
//  A refcounted object's own vtable slot 0.
typedef void (__thiscall *LayerReleaseFn)(void* self);

//----------------------------------------------------------------------------
//  The shortcut registry every layer carries at +0x80.  The shipped class is
//  `TSHashTable<SHORTCUTREGNODE, HASHKEY_NONE>` - 0x6F600430 stamps
//  `??_7?$TSHashTable@USHORTCUTREGNODE@@VHASHKEY_NONE@@@@6B@` (0x6F96D9D8)
//  into it - and it is deliberately *not* spelled as storm.h's
//  TSHashTable<T,K>: that template declares `AllocNode` pure virtual, so
//  instantiating it would emit a vtable full of `__purecall` that
//  tools/link_check.py cannot resolve.  The layout below is that template's,
//  field for field, and the vtable pointer is a plain store either way.
//----------------------------------------------------------------------------
struct SHORTCUTREGNODE;

struct SHORTCUTREGTABLE
{
    //  0x6F600430.
    SHORTCUTREGTABLE();
    //  0x6F5FE450 - `retn 4`, thiscall + one stack bool.  Same shape as
    //  Render/facechartable.cpp's CHARCODEDESC_TABLE::Clear - see that
    //  file's own header comment for why the vtable-slot-0 call inside it
    //  is a raw indirect call rather than a `virtual` member.
    void Clear(unsigned int freeNodes);

    void*                            m_vtable;   // +0x00
    TSExplicitList<SHORTCUTREGNODE>  m_list;     // +0x04
    unsigned int                     m_probes;   // +0x10
    TSGrowableArray<TSExplicitList<SHORTCUTREGNODE> >
                                     m_buckets;  // +0x14
    unsigned int                     m_mask;     // +0x24
};

class CLayer : public CObserver
{
public:
    //  0x6F600720 - construct under `owner`, or as a root when it is null.
    //  Its CObserver base is a real base subobject and not a member: the
    //  shipped body calls CObserver's constructor (0x6F629A70) and *then*
    //  stamps ??_7CLayer@@6B@ over the vtable pointer, which is exactly what
    //  MSVC emits for a derived class of a polymorphic base - so this build
    //  writes the vtable operand as the symbol the shipped code names rather
    //  than as the literal address the "CObserver as a member" spelling
    //  needed.
    CLayer(CLayer* owner, int a, int order);

    //  0x6F5FBF70 - clayerchildren.cpp.
    int  AncestorsHaveFlags(unsigned int mask);
    //  0x6F5FDCB0 - clayerchildren.cpp.
    void RemoveChild(CLayer* child);
    //  0x6F5FDEC0 - clayerchildren.cpp.
    void PropagateEnabled(unsigned char enabled, int force);
    //  0x6F5FF4B0 - clayerinsert.cpp.
    void AddChild(CLayer* child);
    //  0x6F5FF6D0 - clayersetowner.cpp.
    void SetOwner(CLayer* owner, int addToRootWhenNone, int detachFromOld);

    //====================================================================
    //  The event half of the same class.  0x6F601840 / 0x6F614590 /
    //  0x6F601D60 address the very fields named below - +0x0C and +0x10 are
    //  the flag pair the layer half calls m_flags/m_flagMask, and +0x20 is
    //  m_pOwner - which is what says the "event frame" and the layer are one
    //  class and not two.
    //====================================================================
    //  0x6F601840 - bind or unbind `listener` for this layer's own event
    //  class.  The bind arm is a *tail jump* into CObserver's slot 2, which
    //  is why the shipped code shuffles the three stack arguments in place
    //  rather than pushing new ones.
    void SetEventListener(int a, void* listener, int enable);
    //  0x6F614590 - the same, against event table 0x400B0064.
    void SetAltEventListener(int event, void* listener, int enable);
    //  0x6F5FAD20 - swap the sprite owned at +0x2C, telling slot 0x84
    //  either way.  `retn 4`.  layersetsprite.cpp.
    void SetSprite(void* sprite);
    //  0x6F5FAF10 - take or give up the keyboard.  `retn 0Ch`.
    //  layerkeyboardfocus.cpp.
    void SetKeyboardFocus(int take, int reason, CLayer* candidate);
    //  0x6F601D60 - or `bits` into the layer's pending-state word (the same
    //  read-the-neighbour shape SDirtyFlagOwner::OrField1CCWithField1D0 has
    //  at 0x6F626730) and, when that actually changed anything, tell slot
    //  0x3D which bits moved.
    void AddPendingState(int bits);

    //  Slot 61 (+0xF4) - "these state bits just changed".  A tail call in the
    //  shipped stream, which is why AddPendingState rewrites its own argument
    //  slot rather than pushing a new one.
    void NotifyStateChanged(int changedBits)
    {
        typedef void (__thiscall *Fn)(void*, int);
        ((Fn)(*(void***)this)[0xF4 / 4])(this, changedBits);
    }

    //  +0x00..+0x0B is the CObserver base subobject above.
    unsigned int            m_flags;        // +0x0C - bit 3 "hidden",
                                            //         bit 6 the cache mark
    unsigned int            m_flagMask;     // +0x10 - or'd into m_flags
    //  Raw lists initialised by CLayer's constructor body rather than by a
    //  member constructor - see CFrameNodeList above for why.
    CFrameNodeList<LAYERNODE, 4> m_children;    // +0x14
    CLayer*                 m_pOwner;       // +0x20
    int                     m_field24;      // +0x24 - the constructor's `a`
    int                     m_field28;      // +0x28
    int                     m_field2C;      // +0x2C
    int                     m_field30;      // +0x30
    float                   m_float34;      // +0x34
    void*                   m_field38;      // +0x38 - a refcounted reference
    int                     m_field3C;      // +0x3C
    int                     m_field40;      // +0x40
    CLayer*                 m_field44;      // +0x44 - set to `this`
    char                    m_reserved48[0x08];
    int                     m_field50;      // +0x50
    int                     m_field54;      // +0x54
    int                     m_field58;      // +0x58
    int                     m_field5C;      // +0x5C
    char                    m_reserved60[0x04];
    int                     m_field64;      // +0x64
    CLayer*                 m_field68;      // +0x68 - set to `this`
    float                   m_float6C;      // +0x6C
    float                   m_float70;      // +0x70
    CFrameNodeList<LAYERNODE, 4> m_list74;      // +0x74
    //  Raw storage: its constructor is a real out-of-line call, and the
    //  shipped stream has it two thirds of the way through the body.
    char                    m_shortcuts[0x28];  // +0x80
    int                     m_order;        // +0xA8 - the sort key children
                                            //         are kept in
    unsigned char           m_enabled;      // +0xAC
    char                    m_reservedAD[0x03];

    //  +0xB0, read two ways in one function and so declared both ways.
    //  SetOwner tests bits 0 and 1 of the *owner's* copy together
    //  (`test byte ptr [ecx+0B0h], 3`) and then writes bit 1 of its own
    //  with the read-xor-mask-xor sequence that only a bitfield produces
    //  (docs/msvc-vc8-idioms.md, "A single-bit field write is a bitfield,
    //  and only a bitfield reproduces it").
    union
    {
        unsigned int m_stateBits;               // +0xB0
        struct
        {
            unsigned int m_stateBit0    : 1;
            unsigned int m_ownerVisible : 1;
            unsigned int m_stateRest    : 30;
        } m_state;
    };
};

//: unk_6FACE728 - the pool every LAYERNODE is allocated from and freed to.
extern CDataAllocator g_layerNodeAllocator;
//: off_6FA9A420 / dword_6FA9A424 - the root list's terminator, the two words
//: of a TSList<LAYERNODE,4>'s TSLink.  Declared as two separate globals
//: because that is how IDA names them and the canonicaliser compares operand
//: *symbols*: one object with `+4`/`+8` subscripts would cost every access
//: past the first its match (docs/msvc-vc8-idioms.md, "Three separate byte
//: arrays for one three-byte-stride table").
extern TSLink<LAYERNODE>* g_layerRootNext;
extern int                g_layerRootPrevLink;
//: dword_6FACE67C - the layer that currently owns the keyboard, and
//: dword_6FACE680 the lock that freezes it.  Both are read and written by
//: CLayer::SetKeyboardFocus (0x6F5FAF10, layerkeyboardfocus.cpp) and by
//: nothing else in this closure.
extern CLayer* g_pKeyboardFocusLayer;
extern int     g_keyboardFocusLocked;
//: flt_6FACE660 - zeroed on every ownership change.  That it is a float and
//: that every writer stores 0.0f into it is all this closure establishes;
//: nothing here reads it.
extern float g_layerElapsedTime;

//  The RTTI descriptor name the shipped allocator calls are tagged with,
//  reproduced as a literal because this build has RTTI off.
extern const char g_layerNodeTypeName[];

#endif

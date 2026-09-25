//============================================================================
//  A handful of trivial `mov eax, <imm32> / retn` leaves from sub_6F29ED60's
//  own call tree - each returns one fixed 32-bit constant and touches
//  nothing else.
//
//  This file used to say "none decode to a readable FourCC or ASCII string"
//  and name every one of them after its address.  That was wrong for three
//  of them, and the thing that showed it was mapping the *agile type id*
//  family across the whole CAgent hierarchy (see agiletype.h and
//  docs/targets/CWidget_CSelectable_CItem__vtables.md).  Read most
//  significant byte first - the order agiletype.h's kAgileRootTypeId is
//  written in - the family is:
//
//      '+aga'  0x2B616761   CAgent            (agentdefaults.cpp)
//      '+w3a'  0x2B773361   CAgentWar3        (agentwar3_slots.cpp)
//      '+w3i'  0x2B773369   CWar3Image        (war3image_leaves.cpp)
//      '+w3w'  0x2B773377   CWidget           (widget_vtable_leaves.cpp)
//      '+w3s'  0x2B773373   CSelectable       (selectable_vtable.cpp)
//      '+w3d'  0x2B773364   CDestructable     (0x6F266190, slot 7)
//      '+w3u'  0x2B773375   CUnit             (0x6F26C1F0, slot 7)
//      'item'  0x6974656D   CItem             (item.cpp)
//
//  Every class in the chain uses a synthetic '+w3<initial>' tag except
//  CItem, which uses the plain WC3 object rawcode 'item' - the same
//  constant GetItemTypeFourCC (itemhandletable.cpp) hands back, which is
//  why the JASS natives can type-check an item handle with
//  AgileTypeIsDerivedFrom(id, GetItemTypeFourCC()).
//
//  The rest still have no established meaning and keep an address-derived
//  name.  Several of them *are* readable in memory order rather than
//  MSB-first ("deou", "depu", "dtou", "doup", "dopp", "dotp"), but
//  nothing in any dump here says what they select, and a plausible-looking
//  guess is worse than an honest address.
//============================================================================

//  0x6F26C1C0 - '+w3u', the same constant CUnit's own vtable slot 7 returns
//  from 0x6F26C1F0, 0x30 bytes further on.  That pairing is not a
//  coincidence and it is not the linker folding anything: CItem has exactly
//  the same pair, GetItemTypeFourCC at 0x6F2B5AE0 and CItem::GetAgileTypeId
//  at 0x6F2B5B10, the same 0x30 apart (item.cpp records why the shipped
//  code loads the immediate inline rather than calling the free function).
//  This image was linked without /OPT:ICF (agentdefaults.cpp), so the two
//  bodies really are emitted twice.  This is the free-function half.
unsigned int GetUnitTypeFourCC()  { return 0x2B773375; }

//  0x6F266160 - '+w3d', the free-function half of exactly the same pair for
//  CDestructable: CDestructable::GetAgileTypeId (0x6F266190,
//  unit_destructable_typeid.cpp) sits 0x30 bytes further on, the same
//  spacing CUnit and CItem have.  SCheckedDestructableSlot::Assign
//  (destructabletypedassign.cpp) is the caller that proves this half is
//  emitted at all: it is the only member of the checked-slot family whose
//  required type id arrives from a call rather than as an immediate.
unsigned int GetDestructableTypeFourCC() { return 0x2B773364; }

//  0x6F3D9E30 - 'ally', MSB first.  Plainly readable, which is the whole
//  reason this file's old blanket claim did not survive review.
unsigned int GetAllyFourCC()      { return 0x616C6C79; }

//  0x6F2712B0 - '+ord', MSB first: the same '+' prefix the agile type ids
//  above carry, so an agile type of its own rather than an object rawcode.
unsigned int GetOrderAgileTypeId() { return 0x2B6F7264; }

//  0x6F4326D0 - '+loc', MSB first: the agile type id of the JASS `location`
//  object, and the eighth member of the '+'-prefixed family above.  It is
//  the type the `Location` native (0x6F3D03B0, jassnatives_locationmake.cpp)
//  looks up to find the pool a new location comes from, and the type
//  SCheckedLocationSlot::Assign (locationtypedassign.cpp) checks a candidate
//  against - the same two uses '+w3w' has for widgets.  Read in memory order
//  it is "col+", which is what kept it in the unidentified list until the
//  `Location` call tree showed both of its call sites.
unsigned int GetLocationAgileTypeId() { return 0x2B6C6F63; }

//  0x6F3DA570 - '+cst', MSB first: the agile type id of the JASS
//  `camerasetup` object, checked by SCheckedCameraSetupSlot::Assign
//  (camerasetuptypedassign.cpp), which is what ResolveCameraSetupHandle
//  stages a candidate through.  Read out of the live database rather than
//  out of a dump - no call tree here is rooted at it - but it is the same
//  two-instruction `mov eax, <imm32>` / `retn` leaf as its fifteen
//  siblings above.
unsigned int GetCameraSetupAgileTypeId() { return 0x2B637374; }

//  0x6F271330 - 'ord.', MSB first: COrderPoint's agile type id.  Four of
//  these sit 0x80 apart in one block starting at GetOrderAgileTypeId, and
//  the block is the order-class table: '+ord' (COrder, the base agile
//  type), 'ord.' (COrderPoint), 'or.2' (COrderPoint2), 'ordt'
//  (COrderTarget), 'ort2' (COrderTarget2).  The class names are the
//  binary's own RTTI strings, and the pairing is fixed by what uses each
//  tag: 0x6F294B30 builds a point order out of 'ord.' and 0x6F294D40
//  builds a target order out of 'ordt'.  The third character is the
//  variant ('.' point, 't' target) for the plain form and moves one place
//  left in the "2" form, which is why '.' shows up where a letter would.
unsigned int GetOrderPointAgileTypeId()   { return 0x6F72642E; }

//  0x6F271430 - 'ordt', COrderTarget's.  Its two siblings are at
//  0x6F2713B0 ('or.2', COrderPoint2) and 0x6F2714B0 ('ort2',
//  COrderTarget2); neither is reached by anything reconstructed here, so
//  neither has a body yet - CUnit::FindAbilityForOrder switches on both as
//  inline immediates, which is where the two values came from.
unsigned int GetOrderTargetAgileTypeId()  { return 0x6F726474; }

//  0x6F271940 - 'tskA', MSB first: CTaskAction's agile type id, from the
//  parallel CTask* family the same RTTI block names (CTask, CTaskParam,
//  CTaskRotate, CTaskAction, CTaskOrderParam, CTaskPoint, CTaskPoint2,
//  CTaskPointAndTarget, CTaskTarget, CTaskTarget2, CTaskGhost).  Same
//  "three letters plus the variant" shape the order tags have, and
//  CTaskAction is the only member of that family whose name starts with
//  the 'A' this one ends in.  CUnit::FinishCurrentOrder (unitorderfinish
//  .cpp) makes one object of this type and nothing else does.
unsigned int GetTaskActionAgileTypeId()   { return 0x74736B41; }

//  Still unidentified - see the header comment.
unsigned int GetConstant_6F270F30() { return 0x756F6564; }
unsigned int GetConstant_6F270FB0() { return 0x75706564; }
unsigned int GetConstant_6F271030() { return 0x756F7464; }
unsigned int GetConstant_6F3DB6A0() { return 0x2B71756D; }
unsigned int GetConstant_6F40C990() { return 0x70756F64; }
unsigned int GetConstant_6F40CA10() { return 0x70706F64; }
unsigned int GetConstant_6F40CA90() { return 0x70746F64; }
//  0x6F431EA0 - 'wscd', MSB first: CScriptEventData's own agile type id, and
//  the class name is the binary's rather than an inference.  Its vftable
//  ??_7CScriptEventData@@6B@ puts a body at slot 7 (the agile-type-id slot
//  the whole family above uses) that is literally `mov eax, 77736364h` /
//  `retn`.  Checked by SCheckedEventDataSlot::Assign
//  (scripteventdatatypedassign.cpp), which is what
//  ResolveEventDataHandle stages a candidate through.
unsigned int GetScriptEventDataAgileTypeId() { return 0x77736364; }

//  0x6F082470 - 'Agmb' MSB-first, an ability rawcode by the leading 'A' the
//  WC3 object editor gives every ability.  Its one observed use is as an
//  argument pushed straight into 0x6F0787D0 alongside a
//  three-of-{1,0,1}-plus-a-flag argument list, which says it selects
//  something without saying what, so the name stays literal.
unsigned int GetConstant_6F082470() { return 0x41676D62; }

//  0x6F200960 - 'Amov' MSB-first: the standard WC3 object-editor rawcode for
//  the built-in Move ability every unit carries, from the CUnit vtable
//  closure walk (cunit_agent2_worklist.md).
unsigned int GetMoveAbilityRawcode() { return 0x416D6F76; }

//  0x6F2717C0 - 'task' MSB-first: CTask's own base agile type id, the one
//  SCheckedTaskSlot::Assign (checkedtaskslot_assign.cpp) gates on - the
//  family's base rather than one of its derived leaves like CTaskAction
//  ('tskA' above). Turned out to already be cunit_agent6's own worklist
//  item, properly claimed and scored there under a different name
//  (GetFourCC_6F2717C0, src/Misc/cunit_agent6_leaves.cpp); this copy
//  carries no funcmap row of its own (a distinct name, so it compiles
//  and links fine - just not the canonical claim for this address).
unsigned int GetTaskAgileTypeId() { return 0x7461736B; }

//  0x6F0D91A0 - GetConstant_6F0D91A0(), just below, is cunit_agent8's own
//  worklist item and is defined there instead - a genuine duplicate
//  *definition* under the identical name would not even link, unlike this
//  batch's other cross-agent address overlaps (different names, same
//  address). SCheckedSlot_6F0D91A0::Assign (checkedslot_6F0D91A0_assign.cpp)
//  still gates on 'AUbu' the same way.

//  Three more from cunit_agent5_worklist.md (BFS depth 1-2 off CUnit's own
//  vtable), the same shape as every other member of this file: no readable
//  FourCC or ASCII string, no dump body for any caller here to say what
//  selects them, so the name stays address-derived.
unsigned int GetConstant_6F028940() { return 0x416D6169; }
unsigned int GetConstant_6F0D9020() { return 0x41486275; }
unsigned int GetConstant_6F1D6100() { return 0x41736C61; }

//  --- cunit_agent7 batch, CUnit closure BFS depth 1-2 ---
//  0x6F0D9120 - 'AEbu' MSB-first, an ability rawcode by the leading 'A'.
//  Called from three of this build's own SCheckedXxxSlot::Assign members
//  (0x6F275530, 0x6F294010, 0x6F29AAD0), none of them in this batch's own
//  worklist, as the type id checked via AgileTypeIsDerivedFrom - the same
//  shape SCheckedAAbtSlot/SCheckedAHbuSlot (Agent/checkedaabtslot.cpp,
//  checkedahbuslot.cpp) use for their own siblings.  Nothing in any dump
//  here says what class or ability "AEbu" actually names.
unsigned int GetConstant_6F0D9120() { return 0x41456275; }

//  0x6F0D91A0 / 0x6F15C300 - two more of the same `mov eax, <imm32> / retn`
//  leaves, reached via CUnit's own vtable BFS closure (cunit_agent8_worklist
//  .md) rather than any call tree already in this file.  Both decode as
//  'A'-first ability rawcodes, the same family GetConstant_6F082470 above
//  is, and neither has an established real ability behind it in anything
//  read so far, so both keep their literal FourCC.
unsigned int GetConstant_6F0D91A0() { return 0x41556275; }   // 'AUbu'
unsigned int GetConstant_6F15C300() { return 0x41757067; }   // 'Aupg'

//  0x6F24F580 / 0x6F2CFBD0 - two more `mov eax, <imm32> / retn` leaves, from
//  the CreateUnitForPlayer neighbourhood (createunit-G3's own worklist
//  slice, checkedslot_createunit_cluster.cpp).  Both are called out-of-line
//  by their own SCheckedSlot_<addr>::Assign, same as every other member of
//  this file - so, same as GetUnitTypeFourCC/GetConstant_6F0D91A0 above,
//  they belong in this TU rather than beside their one caller, or this
//  build inlines the one-instruction body straight into it where the
//  shipped code keeps a real `call`.  Neither decodes to a class or ability
//  identity anything in this closure slice ties down: 0x6F24F580 is '+ccp'
//  MSB-first, plausibly a sibling of the '+'-prefixed agile type id family
//  above (a synthetic tag, not a WC3 object rawcode) but not confirmed by
//  any caller in this slice; 0x6F2CFBD0 is 'AIcs', 'A'-first like the
//  ability rawcodes but likewise unconfirmed.  Both stay literal.
unsigned int GetFourCC_6F24F580() { return 0x2B636370; }   // '+ccp'
unsigned int GetFourCC_6F2CFBD0() { return 0x41496373; }   // 'AIcs'

//  0x6F24F510 - same family, one call site: SCheckedSlot_6F24F510::Assign/
//  Construct/MakeAndAssign (Agent/checkedslot_createunit_cluster.cpp).
//  '+ccu' MSB-first.
unsigned int GetFourCC_6F24F510() { return 0x2B636375; }   // '+ccu'

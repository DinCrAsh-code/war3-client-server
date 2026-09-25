//============================================================================
//  The agile type ids the JASS handle resolvers check against - one
//  `mov eax, <imm32>` / `retn` leaf each, in the same '+'-prefixed FourCC
//  family misc_rawcode_getters.cpp catalogues for the CAgent hierarchy.
//  Read most significant byte first, every one of them is legible:
//
//      '+frc'  0x2B667263   force        0x6F3DAB70
//      '+tmr'  0x2B746D72   timer        0x6F3DC600
//      '+flt'  0x2B666C74   filterfunc   0x6F3B1420
//      'bExp'  0x62457870   boolexpr     0x6F3B15D0
//      '+rct'  0x2B726374   rect         0x6F3DB8A0
//      '+fgm'  0x2B66676D   fogmodifier  0x6F3DAA60
//      '+trg'  0x2B747267   trigger      0x6F443D50
//
//  Two members of the same set are not here.  '+ply' (0x6F40AFA0) was
//  already reconstructed as GetTypeTag_6F40AFA0 (trivialaccessors_04.cpp)
//  and keeps that name, because its one caller is EXACT against a
//  declaration that takes a dead receiver in ecx and re-spelling it would
//  cost that match (playertypedassign.cpp says why).  The region type
//  '+agr' (0x2B616772) has no getter at all: its Assign loads the immediate
//  inline, the same way CItem's own slot 7 does (item.cpp), which is what
//  makes that body forty instructions where every other copy is forty-two.
//
//  'bExp' is the one that does not carry the '+' prefix, and it is the one
//  type in the set JASS itself names in mixed case - which is a small piece
//  of evidence that these tags were written by hand, one per type, rather
//  than generated.
//
//  Leaves with no calls, so nothing here can be inlined into anything that
//  has to stay a real call; one translation unit for all seven.
//============================================================================

//  0x6F3DAB70 - '+frc'.
unsigned int GetForceAgileTypeId() { return 0x2B667263; }

//  0x6F3DC600 - '+tmr'.
unsigned int GetTimerAgileTypeId() { return 0x2B746D72; }

//  0x6F3B1420 - '+flt'.
unsigned int GetFilterAgileTypeId() { return 0x2B666C74; }

//  0x6F3B15D0 - 'bExp'.
unsigned int GetBoolExprAgileTypeId() { return 0x62457870; }

//  0x6F3DB8A0 - '+rct'.
unsigned int GetRectAgileTypeId() { return 0x2B726374; }

//  0x6F3DAA60 - '+fgm'.
unsigned int GetFogModifierAgileTypeId() { return 0x2B66676D; }

//  0x6F443D50 - '+trg'.
unsigned int GetTriggerAgileTypeId() { return 0x2B747267; }

//  0x6F4327D0 - '+evt', the JASS `event` object's own agile type id and the
//  eleventh member of the family above.  Checked by
//  SCheckedEventSlot::Assign (eventtypedassign.cpp).
unsigned int GetEventAgileTypeId() { return 0x2B657674; }

//  0x6F028B10 - 'AAsm'.  The tag SCheckedSpellAbilitySlot::Assign
//  (spellabilitytypedassign.cpp) checks a candidate against, and the one
//  every `GetSpell*` native's object has to carry.  Unlike the ten above it
//  is not a '+'-prefixed JASS type but an ability rawcode, in the same
//  module and the same 'AA__' shape as CAbilitySpell's own 'AAsp'
//  (0x6F028F60) - the leading 'A' the WC3 object editor gives every
//  ability.  No class in the image returns it from a vftable slot, so it is
//  a type checked *against* rather than one an object reports.
unsigned int GetSpellAbilityAgileTypeId() { return 0x4141736D; }

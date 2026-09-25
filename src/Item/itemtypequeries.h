//============================================================================
//  The item-type predicate probes (itemtypequeries.cpp).  Declared in a
//  header so that the JASS natives that tail-jump into them and the
//  definitions here cannot drift into two mangled names - the failure
//  docs/notes/verifier-gate-link-failures.md calls cause 1.
//============================================================================
#ifndef ITEMTYPEQUERIES_H
#define ITEMTYPEQUERIES_H

int __fastcall ItemTypeIsPowerup(unsigned int fourCC);   // 0x6F2B8DD0
int __fastcall ItemTypeIsSellable(unsigned int fourCC);  // 0x6F2B8E00
int __fastcall ItemTypeIsPawnable(unsigned int fourCC);  // 0x6F2B8E30

//  0x6F2B8D70 - a fourth, same-shape probe; see itemtypequeries.cpp.  Used
//  from abilityinventorydispatch.cpp, hence the header (same
//  two-mangled-names reason as the three above).
unsigned int __fastcall ItemTypeAbilitySlotCategory(unsigned int fourCC);

#endif

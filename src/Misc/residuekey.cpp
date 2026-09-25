//============================================================================
//  0x6F4AB660 - stamp a value and its four-modulus residue signature into a
//  two-word record.
//
//  The arithmetic is four constant divisions the compiler turns into
//  multiply-high sequences, and the moduli are 59, 61, 53 and 47 - four
//  primes, each residue fitting in six bits and each shifted into its own
//  six-bit field of the second word:
//
//      bits  2- 7   value % 61
//      bits 10-15   value % 59
//      bits 18-23   value % 53
//      bits 26-31   value % 47
//
//  The last one is why the shipped stream ends in `x + 17*q` where the other
//  three end in `x - m*q`: the field is the *top* six bits, so everything
//  above bit 31 is discarded by the `shl eax, 1Ah` anyway, and 17 is -47
//  modulo 64.  MSVC folds the subtraction into that.
//
//  **What the signature is for is not established.**  The whole corpus -
//  four call trees - contains exactly one call: CMinimap's constructor
//  (0x6F36D9F0) hands the module-global record at 0x6FAB6214 the minimap
//  object's own address.  Nothing reads either word back, so there is no
//  evidence here for a better name than "the residues of a value".
//
//  A second, unrelated caller surfaced later (checksum-provider-registry,
//  provider-slots-0-4 session): CRandData's own 45-record history buffer
//  (Sync/randdata.h) seeds every record through this exact function, so
//  the struct moved into residuekey.h for both to share.
//============================================================================
#include "residuekey.h"

void SResidueKey::Set(unsigned int value)
{
    //  One field at a time, in the shipped order: `|` is associative and
    //  MSVC reassociates a single four-term expression, which moves the
    //  47 block up next to the 59 one.
    unsigned int residues = (value % 59) << 10;
    residues |= (value % 61) << 2;
    residues |= (value % 53) << 18;
    residues |= (value % 47) << 26;

    m_residues = residues;
    m_value = value;
}

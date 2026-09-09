# CD interrupt dispatch candidate

Reconstructed directly from retail assembly at 0x8007C13C, length 0xD8.
Despite its historical name, this function drains getintr(), dispatches ready
(bit 4) and complete (bit 2) callbacks, then restores the saved CD register bank.

candidate.c is semantic C without register pins or instruction assembly.
With native GCC 2.8.1 and -mno-split-addresses its .text is 0xE0 bytes.
The remaining eight bytes are NOP delay slots at the indirect callback calls:
the target schedules the result-buffer address addiu in each jalr delay slot.
The candidate currently materializes those addresses before each call.
This is not an exact match and must not replace the ASM manifest entry yet.

## Follow-up experiments

Removing `-mno-split-addresses` fills the callback delay slots, but hoists
addresses into additional saved registers and increases the stack frame from
0x28 to 0x30. The resulting text remains 0xE0, so this is not a match.
Trials disabling strength reduction, either instruction scheduler, or expensive
optimizations also retained 0xE0 text in that configuration.

With the saved candidate, callback declarations using unspecified arguments,
`unsigned char` status, or `void *` buffers each retained 0xE0 text.
MASPSX ASPSX profiles 1.07, 2.08, 2.21, 2.34, 2.79, and 2.86 also retained
0xE0 text. These size checks rule out an exact match for those trials; they
are not relocated instruction comparisons or proof of SDK provenance.

## Direct GNU-as diagnostic

Assembling raw GCC281 output directly with GNU as (`-EL -G0 -march=r3000
-no-pad-sections`) produces the target 0xD8 bytes. After linking with
diagnostic.ld, only byte 0x3C differs: GNU as encodes the move into s0 as
`or s0,v0,zero` (0x00408025), whereas retail uses `addu s0,v0,zero`
(0x00408021). Both callback delay slots then have the retail address adds.
This is not an exact byte match. More importantly, ASM_AND_GTE_POLICY.md
requires final builds through stock MASPSX; this diagnostic does not authorize
bypassing that pipeline. It isolates scheduling from semantic reconstruction.

## Shared event and callback types

The candidate now includes `psyq_cd.h`, uses the shared `CdlCB` declarations,
and addresses `CdInterruptEvents.ready` and `.sync` directly. This removes
the two local callbacks with incorrect int event parameters and the pointer
subtraction from the single ready-event byte to the preceding sync byte.
The event accesses retain their shared volatile qualification.

All 224 linked candidate text bytes are identical before and after this
cleanup; the instruction-scheduling mismatch with the 216-byte retail body
remains. Objdiff reports 93.888885% with the named structure-member relocation
instead of the old ready-byte alias. This percentage is not new matched code,
and no production change or new behavioral-equivalence claim is made.

## SDK provenance

Psy-Q 4.6 LIBCD.LIB's BIOS_1.OBJ contains this exact 216-byte dispatcher
at text offset 0x14A4. Its non-relocation bits match retail
0x8007C13C..0x8007C214 throughout; differences are confined to 16 relocation
sites. CD_initintr's relocations at 0x1130/0x1134 form this local function's
address, followed by the InterruptCallback call at 0x1138. CD_init also
references the same local entry. The dispatcher is not an exported function
named Cd_SetIntrMask in that object. Its historical repository name should
not be used as evidence for interpreting the routine as a mask setter.
This establishes membership in BIOS_1, not a byte match for the whole object.

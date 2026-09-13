# Psy-Q attribution in the progress report

Report categories follow verified linked address ranges, not decompilation
status or directory names. `configs/USA/psyq_provenance.json` records 160
signature comparisons: 153 complete SDK objects and seven function ranges.
These identify 313 current linked code units, including 178 outside `psyq/`.
Some identical SDK implementations have multiple possible object identities;
the database retains those alternatives instead of claiming an exact origin.

Each entry records SDK version, library/object, label offsets, known-byte count,
signature JSON SHA-256 and the complete retail-range SHA-256. Signature sources
are the external `psx-ruby/.tools/psyq-sigs` collection. Verification is explicit:

```
python3 tools/scripts/verify_psyq_provenance.py --sdk-dir /path/to/psyq-sigs
```

No SDK signature bytes or retail code are embedded in the metadata. Ordinary
report generation reads the checked-in attribution and needs no signature pack.
The original source-directory category remains a fallback for previously
identified SDK code whose exact library revision does not match these signatures.
Code without either form of attribution remains in `main-game`; the attribution
set is not claimed to identify every remaining SDK function.

The generator classifies a unit as SDK code only if the union of verified ranges
covers every byte of all its linked text sections. A gap or a mixed game/SDK TU
cannot acquire SDK ownership from a partial overlap. Where identity is unique,
the grid path is `main/psyq/<library>/<SDK object>/<SDK label>`. Ambiguous matches
retain a neutral path. This is grouping of actual linked units, not a claim that
we have already rebuilt every original SDK translation unit.

The four functions from 7a55a39b now use `_dirFailAuto`,
`LIBPAD_PADCMD_text_3A0`, `LIBDS_DSSYS_2_text_170` and
`LIBDS_DSSYS_1_text_8B8` as link symbols. `text_*` are private signature labels,
not recovered original C names; the library/object prefixes disambiguate them.
Their previous MemCard/CardObj/Render names no longer obscure them in function
lists. Existing source paths are still supplied separately for GitHub links.

Attribution never grants progress: original ASM and instruction-constrained C
still receive no matching base object. Whole-program semantic totals must remain
unchanged when this classification alone changes. Category percentages can
change substantially because previously misclassified SDK code moves from
`main-game` to `main-psyq`, including functions still written in ASM.

The site displays the last successfully ingested Actions report. A pushed commit
is not visible until its build/report job finishes and decomp.dev ingests it.

Validation of this correction: `make -j8 check`, all 325 tests in `make ci`,
and `make report` pass. Global credited code remains 2,400,916 bytes / 10,517
functions. Moving 238 functions into the SDK category changes `main-psyq` to
385/516 functions and 74.50% code. All four renamed functions remain at 100%.

## Symbolic stores in the two SPU setters

`SpuSetIRQCallback` and `SpuSetTransferMode` (historical link names
`Spu_SetTransferMode` and `Spu_SetReverbMode`) previously matched the retail
EXE after linking, but their object comparisons were below 100%. Their final
stores used a hard-coded `0x800A0000` page base and negative offsets instead
of HI16/LO16 relocations against `_spu_IRQCallback` and `D_8009B418`.

Direct C assignments through the existing shared declarations, compiled with
stock native GCC 2.7.2 and the existing GNU assembler route, retain every
linked byte and now match the symbolic object comparisons at 100%. No report
masking, toolchain changes or new instruction macros are involved. The fake
page structure, two register pins, one empty barrier and per-function compiler
and scheduling overrides have been removed.

`test_spu_symbolic_setters.py` checks the complete 60-byte and 48-byte linked
ranges and their exact relocation offsets, types and symbols. Thus returning
to fixed addresses fails the regression even if the final EXE still matches.
This corrects report credit for two existing C reconstructions; it is not a
claim that 108 previously unimplemented code bytes were newly decompiled.

## SDK object padding

The `padding` list in `psyq_provenance.json` records 27 additional zero-filled
object tails, totaling 196 physical bytes. Every range immediately follows
`jr $ra` and its delay slot, ends at a verified complete SDK object boundary,
and contains no SDK label. `main.yaml` now represents those ranges as `pad`;
the linker retains their bytes without manufacturing one-function ASM units.
`test_psyq_padding.py` checks the manifest against the SDK boundary records and,
when retail is available, checks every zero byte and the preceding return.
The full executable SHA-1 remains the final layout and byte check.

This removes 27 spurious functions (108 reported code bytes), not 27 newly
completed decompilations. Padding is still included in the expected-object
coverage audit. The nonzero SDK signatures discussed below and the exception-handler
instruction templates are not included in this zero-padding correction.

## SDK signature records

The eight-byte records at `0x80073C54` and `0x80077F74` belong to the verified
`A63.OBJ` and `MSC00.OBJ` images. Their first SDK function labels are `puts`
and `InitGeom`, both at object offset eight. The records are readonly data,
not callable functions or zero padding.

The manifest places their `.psyq_signature` input sections in the original
text order. A distinct readonly section prevents the linker's earlier
`.rodata*` collection from moving them to the beginning of the executable.
Objdiff therefore counts the 16 bytes as data, while the full retail SHA-1
still verifies their exact content and placement. No code match is awarded.

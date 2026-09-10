# PsyQ Source Donor Audit

## Scope And Confidence

This audit examines public decompilation source, not redistributed proprietary
SDK archives. Donor checkouts live outside the repository in
`/tmp/pe-psyq-donors/`. No donor build scripts, Windows compilers, or SDK object
archives were installed or executed.

The local README and compiler wrapper reference PsyQ 4.0 headers. That does
**not** establish that every library linked into Parasite Eve is version 4.0.
Likewise, a decomp.me compiler preset identifies a toolchain, not necessarily a
runtime library revision. SDK functions can remain unchanged across releases.
We have not yet fingerprinted every retail library against multiple SDK releases.
Consequently, no game below is certified as having an identical complete SDK.

There is already a practical positive result: an adapted public reconstruction
of `SpuClearReverbWorkArea` matches PE's 404-byte function exactly with our stock
native toolchain. Compatibility can be established function by function without
pretending that the whole SDK version question is settled.

## Repositories Inspected

| Project | Primary evidence | Practical verdict |
| --- | --- | --- |
| [Psyz](https://github.com/Xeeynamo/psyz/blob/main/decomp/README.md) | The matching `decomp/` explicitly targets PsyQ 4.0; it describes its origin in SOTN and Sozud's SDK decomp. | Best version-specific reference. Use `decomp/src`, not the separate native port implementations. Its root license manifest does not clearly assign a license to `decomp/src`; trace individual source provenance before copying. |
| [psy-q-decomp](https://github.com/sozud/psy-q-decomp/blob/master/build.py) | README starts with 3.5, but the build explicitly supports 3.3, 3.5, 3.6 and 4.0, including a 4.0 SPU list. Root MIT license. | Preferred direct donor when the implementation is compatible. A listed C file is not proof of a match: some entries, including `s_crwa.c` in an earlier-version list, are commented out. |
| [Castlevania: Symphony of the Night](https://github.com/Xeeynamo/sotn-decomp/tree/master/src/main/psxsdk) | Public reconstructed SDK C. The inspected `libspu` files carry MIT SPDX headers independently of the root AGPL license. US permuter config names `psyq4.0`; some GPU units select older assembler settings. | Strong SPU donor and independent reference. Do not label all its SDK modules 4.0 from the permuter preset. |
| [Croc](https://github.com/Xeeynamo/croc) | README recommends the PsyQ 4.0 decomp.me compiler. `src/croc/3038.c` still includes ASM for `SpuClearReverbWorkArea`. | Useful symbols and toolchain clue; not a ready source donor for this function. Runtime release equality remains unproved. |
| [Chrono Cross](https://github.com/jdperos/chrono-cross-decomp/blob/master/Makefile) | Makefile downloads preconverted PsyQ objects; its comment explicitly says the older SDK assembler version is uncertain. Sound engine C exists separately. | Useful for Square sound/AKAO research, but imported SDK objects are not semantic C and cannot increase our decompilation count. No same-version claim. |
| [Final Fantasy VII](https://github.com/Xeeynamo/ff7-decomp/blob/master/src/main/psxsdk.c) | `psxsdk.c` has a `PSYQ=3.3` build annotation and a mixture of C and `INCLUDE_ASM`; it also contains game-specific CD wrappers. | Relevant Square engine context, not evidence of a uniformly matching 4.0 SDK. Audit individual routines and license before reuse. |
| [Legend of Mana](https://github.com/celophi/lom-decomp/tree/master/src/psyq) | Reconstructed SDK tree exists, but inspected `libgpu/SYS.c` contains `INCLUDE_ASM` declarations. Dockerfile supplies several compiler families, not a definitive runtime release. | Secondary donor to inspect per function. File/directory presence alone substantially overstates immediately reusable C. |

Psyz, SOTN and Sozud are related sources, not three independent confirmations of
the same reconstruction. Prefer the original attributed implementation where
possible. Do not apply SOTN's root license to every SDK file, or assume Psyz's
MIT-covered tools automatically license its other directories.

Checkouts used for the central comparison:

- Sozud: `6edf9b24721ba02a53eb423b8a7497b82ba0768c`.
- Psyz: `abef9558c18109e9fddef208cdfee5cf525e7fda`.
- SOTN: `a61b68779fbc7ef1b249554e5f00a92b1484f97e`.

## First Verified Adaptation

The original `SpuClearReverbWorkArea` name describes its actual role:
`SpuClearReverbWorkArea`. It validates the selected preset and SPU allocation,
temporarily switches transfer state and disables the transfer callback, clears
the reverb work area in chunks of up to 1024 bytes, waits for DMA completion,
then restores the previous transfer state and callback.

The adapted source is `src/main/psyq/libspu/SpuClearReverbWorkArea.c`, based on Sozud's
`src/spu/s_crwa.c`, with attribution and MIT terms in `THIRD_PARTY_NOTICES.md`.
The old exported symbol is retained to avoid unrelated caller churn.

- Retail span: `0x8008D610..0x8008D7A4`, 404 bytes, one function.
- Stock native GCC 2.8.1, `-mno-split-addresses`, stock MASPSX.
- First straightforward adaptation: upstream asm-differ score 1135/10100.
- Final relocated comparison: **0/10100**, not an empty 0/0 comparison.
- Raw object score before refreshed target naming: 30, entirely the two old
  global names `D_8009B46C` and `D_8009B424` versus their canonical aliases.
- Full `make check`: retail main SHA matches. No target bytes were changed.
- No register pins, no instruction ASM, one empty `$2` clobber barrier.

Required source changes were the explicit table-base/address temporary, reuse
of that temporary for the byte count, the chunk-selection branch shape, and
independent materialization of the transfer-mode restore flag. A trial pin and
an additional base barrier were both removed while preserving the match.
Removing the remaining barrier previously produced a register-copy instruction
where retail materializes a constant, adding 200 scorer points.

This is **one new donor-derived match**. SPU functions committed immediately
before the donor investigation are not counted as results of this import.

## Next Useful Comparisons

1. `SpuSetIRQ`, 316 bytes: Sozud `src/spu/s_si.c` and SOTN's equivalent have C.
   Psyz's file still uses `INCLUDE_ASM`, despite the surrounding SDK coverage.
   The current local blocker is instruction scheduling around timeout printf
   calls, not uncertainty about the IRQ algorithm. Source reuse is not yet a
   verified additional match.
2. `_spu_FwriteByIO`, 448 bytes: compare SOTN's MIT `libspu/spu.c` and Psyz's
   `decomp/src/libspu/spu.c`. Focus on register-field volatility, timeout loops,
   chunk accounting and address scheduling. No imported match claimed here.
3. Expand to GPU/CD/controller functions by actual symbols and behavior, not
   just filenames. Keep SDK startup/BIOS/GTE routines under the existing ASM
   policy; original assembly wrappers are not newly decompiled C.
4. Build a per-library release fingerprint from legitimate local reference
   objects when available. Check instruction/control-flow identity and resolved
   relocations, not just function size, strings, release year or headers.

For each import retain provenance, adapt only source and declarations, compile
with the existing stock toolchain, require upstream decomp.me scoring plus full
SHA validation, regenerate the report and progress, then commit. Do not copy
donor binaries, generated ASM, nonmatching placeholders or native-port shortcuts
into the matching path. This audit is an initial donor survey, not a claim that
all reusable SDK code has already been exhausted.

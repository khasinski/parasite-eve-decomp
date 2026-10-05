# Render_SetupColorTable — matched (2026-10-05)

Promoted to `src/main/render/Render_SetupColorTable.c` and enabled in the
main manifest. The shared rectangle is in `include/pe1/textbox_open.h`;
Menu_SetTextCursorRect now uses the same record.

Stock native GCC 2.7.2 (`-O2 -G8 -funsigned-char -mips1 -mcpu=3000`) and
stock MASPSX in 2.56 mode produce decomp.me-compatible Levenshtein score **0**
and **644/644 identical linked bytes**. Compilation and comparison ran on darwine.
Target: main offset `0x27DE0`, address `0x800375E0`, size `0x284`.
The integrated build passed `make -j32 verify`: the complete `main.exe`
is byte-identical to retail. Source-policy, source-mapping and debt checks pass.

Target SHA-256: `d7f8b96cf54e7f4e98ec49c081059b5e47916ce9e25a5f9e97294fd1eb58d58a`.

The last score-140 mismatch was reversed constant-load pairs. An empty barrier
between the two control-flag updates takes the division reciprocal and page
index as inputs, preserving retail's hoisting and scheduling. The reciprocal
operand is matching debt, not additional game behavior.

Debt: five pins (input cursor, page index, input number, first quotient,
digit counter), three empty barriers, scoped sentinel and reciprocal locals,
and reuse of the page-index register for slot initialization. Removing the
inner quotient pin preserves the match; individually removing any of the
remaining five pins from the preceding six-pin version does not.
No CPU instruction ASM or toolchain modifications.

Research artifacts are in `scratch/Render_SetupColorTable` and
`/home/hasik/fx-search-archives/Render_SetupColorTable` on darwine.
The decisive probe is `constpos7_3`; `minpin5` removes the redundant pin.

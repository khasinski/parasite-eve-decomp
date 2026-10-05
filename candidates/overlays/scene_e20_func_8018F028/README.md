# scene_e20 func_8018F028 (flare particle callback): parked, lev 6

Not integrated; the build still uses the original ASM. Files (both include
the local `scene_e20_flare_draw.h`, a narrow header holding the declarations
that the older drafts kept as externs in the .c):

| File | Matrix loads | Result |
|---|---|---|
| `RoomEffect_FlareParticle_8018F028.c` | C reads of the eight words via `GteMatrixWords` + `gte_ctc2_0..7` | **lev 6** |
| `RoomEffect_FlareParticle_8018F028_macro.c` | `gte_ldrotmatrix` / `gte_ldtransmatrix` | lev 0, scene_e20 overlay-check OK, but not admissible (CPU `lw` inside the macros, see cb992558c) |

## Palette selection: solved (2026-10-05)

Both drafts write the two spinning-glow palette selections as one expression
each, with a separate kind read per draw:

    kind = D_800F3368.palette;
    palette = D_800E1204[kind] + ((kind == 4 && D_800F3428 != 0) ? 8 : 4);
    ...
    streakKind = D_800F3368.palette;
    palette = D_800E1204[streakKind] + ((streakKind == 4 && D_800F3428 != 0) ? 7 : 3);

The instructions are the same as the if/else form, but the registers are
retail's (kind a0, the compare 4 in v1 in the kind load delay slot; second
kind in v1 against the s5 texture argument). This replaces every earlier
`special = 4` / shared-temporary / `kind`-reuse attempt (lev 4 steering,
lev 9 plain). One `kind` shared by both blocks stays at lev 4. The streak
block may also use the if/else form (still lev 0 with the macro loads).

## Matrix loads in C: the remaining lev 6

Retail's two transfer windows are the PSY-Q macro shape
`la v0,D_800BCFA4; lw t0,0(v0); lw t4,0(t0); lw t5,4(t0); ctc2 ...`.
Measured C forms (both windows rewritten the same way):

| Form | Crutches | lev |
|---|---|---|
| plain locals `a,b,c`, pointer local (also field-by-field into the ctc2 macros, eight scalars, re-read through the slot on every word, whole `GteMatrixWords` copy) | none | 47 best (51, 55, 109) |
| t4-t6 pins only | 1 pin decl | 30 |
| t4-t6 pins + empty slot-address constraint | 1 pin decl, 1 barrier | 18 (pointer in v0, not t0) |
| t4-t6 + `$8` pointer pin, no slot constraint | 2 pins | 23 |
| t4-t6 + `$8` pointer pin + slot constraint | 2 pins, 1 barrier | **6** |
| t4-t6 + slot constraint + `$2..$7` clobber on the pointer instead of the `$8` pin | 1 pin, 2 barriers | 6 |

The 6 words are three reload register choices, not matrix code: retail
`mfhi t1` (mode 1 damping), `mflo t0` (case 1 glow size), `mfhi t0` (case 2
fade); the pinned build gives t2, t1, t1. Likely cause (not traced in
the reload dump): in retail the matrix pointer is
the asm input's own reload register (the macro gets the MEM operand), so
t0 is never live as an allocated register and reload's round-robin spill
choice runs over {t0, t1}. Any C form that puts the pointer in t0 through
allocation (pin or clobber list) makes t0 ever-live, so the spill set
becomes {t1, t2} and the same rotation lands one register higher. The
user's arcing emitter (FieldEng_CosinePulse.c, 69f0d8f9e) solved the same
effect with four empty constraints plus four `-ffixed-*` reservations,
which is far above the about-three-pins budget for this function.
The unpinned pointer (lev 18 row) keeps the reload choices right but puts
the pointer in v0.

Not tried: permuter search over the pinned form, and C shapes that give
the pointer a REG_EQUIV memory and no hard register (so reload, not
allocation, supplies t0); no plain-C way to deny it a register was found.

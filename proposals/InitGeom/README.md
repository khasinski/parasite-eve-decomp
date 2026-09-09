# InitGeom reconstruction

New C reconstruction of the 128-byte retail entry at 0x80077F7C.
It saves the caller return address in D_800960AC, calls St_InstallDmaHandler,
reads CP0 Status after that call, sets bit 30 without clearing other bits,
and writes seven GTE control registers in the original order:
29=0x155, 30=0x100, 26=1000, 27=-0x1062, 28=0x01400000, 24=0, 25=0.
The function has a void interface; incidental return-register contents are
not treated as an API result.

Hardware transfers and hazard NOPs remain inline CP0/COP2 operations. The
save uses one explicit RA register binding and one empty output constraint.
The tested GCC272 __builtin_return_address(0) instead loaded an unrelated
stack slot; that variant fails the saved-return-address assertion. There is
no normal CPU instruction assembly or instruction-rewriting postpass.

**Not exact:** stock GCC272 scores 73.4375% (136 bytes versus 128 retail).
GCC281 scores 71.25%, and GCC281 unsplit 71.40625%. The maintained candidate
uses GCC272. The compiler uses a stack frame and restores RA from it; retail
uses the global save slot directly. Register allocation and hazard placement
also differ. The test assumes the installer preserves that global save slot;
it does not establish equivalence if another actor corrupts it or reenters
InitGeom. No production ASM replacement or matching-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/InitGeom/candidate.c /tmp/init-geom.o
python proposals/InitGeom/verify_behavior.py /tmp/init-geom.o
```

All **192 cases** pass against the SHA-1-checked retail body: eight initial
status words, optional installer changes to Status and an unrelated GTE
register, three installer return values, two caller return addresses and
two memory fill patterns. Assertions check the saved RA at installer entry,
complete transfer/call order, post-installer status preservation, all modeled
control registers, final saved RA, return PC, SP and global canaries. Mutants
clearing other Status bits, using the broken builtin return-address capture,
or reversing the negative control constant fail assertions.

The installer is stubbed and CP0/COP2 transfers are serviced by code hooks
after replacing those instructions with NOPs in both bodies. The verifier
compares raw control-write values; it does not implement GTE internal register
normalization, CP0 interrupt effects, hardware timing or pipeline hazards.
It verifies initialization requests and CPU-side behavior under that model,
not complete hardware equivalence.

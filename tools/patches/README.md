# maspsx-label-branch-delay.patch

Apply to the tools/maspsx (mkst/maspsx) checkout:
    cd tools/maspsx && git apply ../patches/maspsx-label-branch-delay.patch

Adds `--label-branch-delay`: when a branch/jump's inserted delay-slot nop or a
call's filled delay slot is followed by a label, the label is bound to the
delay-slot instruction's address (one word early). It also preserves stale
post-callback jump relocations that target the callback's delay slot.

`--post-call-jump-target=SYMBOL` handles the related cross-overlay fossil: it
retargets that post-callback jump to the explicitly named donor tail while the
C control flow remains shared and free of inline assembly.

Required by overlay files carrying `MASPSX_FLAGS: --label-branch-delay`.

## What this flag actually reproduces (empirical, 2026-07-20)

It is NOT documented ASPSX behavior. Verified by running the real toolchain
(CC1PSX.EXE Build 0001 and ASPSX.EXE 2.56/2.67/2.77/2.79/2.81/2.86 under
wibo) on the exact cc1 output: every ASPSX version binds such labels AFTER
the inserted nop, one word later than retail overlay bytes.

The retail overlays' one-word-early branch targets are relocation fossils:
scene functions are binary copies from an earlier donor link whose layout
had the shared tail one word earlier. The stale targets are harmless at
runtime (they jump into a dead delay slot and fall through to the same
code), so they shipped. This flag exists to reproduce those fossil bytes
from honest C.

Harness to rerun the experiment: tools/maspsx/aspsx/ (download.sh fetches
PSY-Q binaries; ASPSX.EXE runs on macOS via wibo-macos from decompals/wibo;
CRLF line endings required; util.py parses the LNK .text section).

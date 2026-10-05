# Flash burst camera-load candidate

Unmatched candidate for `func_800DE0A8`, retail address `0x800DE0A8`, 1792
bytes. It is deliberately excluded from the production build. Production
still uses the old matrix-load macros; this candidate replaces their CPU
loads with C and uses individually wrapped GTE transfers.

Stock native GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1 -mcpu=3000`) and
stock MASPSX 2.56 (`--dont-force-G0 --use-comm-section`) give linked score
**30**, improved from the earlier candidate's 80. No EABI, compiler patches,
assembler patches, or extra register-reservation flags are needed. The
candidate has three transfer-register pins and two empty constraints in its
inlined camera-load helper.

The entire function has the correct length, stack frame, control-flow targets,
and instruction sequence apart from these register choices:

| Address | Retail | Candidate |
| --- | --- | --- |
| `0x800DE290` | `mfhi t0` | `mfhi t1` |
| `0x800DE294` | `subu v0,t0,v0` | `subu v0,t1,v0` |
| `0x800DE590` | `mfhi t0` | `mfhi t1` |
| `0x800DE594` | `sra v0,t0,2` | `sra v0,t1,2` |
| `0x800DE664` | `mfhi t0` | `mfhi t1` |
| `0x800DE668` | `sra v0,t0,2` | `sra v0,t1,2` |

Retail text SHA-256:
`732c13b3bfb144bb46fe481373d2f623cd8979f2f4ac302417d799941835b52f`.

## Evidence and next step

Research lives in `scratch/flash_burst` and on darwine at
`/home/hasik/fx-search-archives/flash_burst`. `no_reservations.c` is the
verified source copied here; `no_reservations.linked.json` and `.linked.diff`
record the result. `evaluate.py` links at the retail address and compares
with `assets/USA/main.exe[0xCE8A8:0xCEFA8]`.

A diagnostic alternative (`clean_alternative.c`) reserves registers 21–23
and excludes other reload temporaries through an empty clobber. It fixes
all three multiply-high register choices, but GCC spills the camera-pointer
pseudo and retains an unused stack slot: the frame becomes `0x90` instead
of `0x88`. Its linked score is 106, with only prologue/epilogue differences.
Do not promote that alternative or hide the stack difference with padding.
The GCC reload dump identifies pseudo 148 as the spilled matrix pointer.

A 90-second darwine permuter run used `--stack-diffs` and
`--no-ignore-branch-targets`. Its score-30 result moved the broad clobber
past `break`; the checked-in C instead removes that unreachable constraint
and the unnecessary reservation flags. The helper shape fixes the earlier
texture-table/matrix-slot address ordering. The next useful experiment must
allow the camera pointer and arithmetic reload to share `t0` without the
unused spill slot, while preserving the default ABI and stock tools.

Related attempts in this session did not produce accepted matches:
`OrbitingEffect` and `SettlingSprite` can match their edited function with
TU-wide reservations, but those reservations break an adjacent function's
retail use of `s3`. `DiamondEmitter` reservations similarly disturb its
renderer. Keep their existing TUs intact; do not infer a full match from
checking only the edited function.

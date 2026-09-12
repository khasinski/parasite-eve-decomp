# RotAverageNclip4

Production `src/main/psyq/libgte/cmb_07.c` matches **176/176 linked bytes**
at `0x80079414`, the complete original TU. Native GCC 2.7.2, unmodified MASPSX
and the default build flags produce the match.

Replaces the previous CPU loads, stores, branches and labels in inline asm
with C. GTE transfers and commands each use a single-instruction macro.
The shared `GteShortVector` type describes all four input vertices.

RTPT projects the first three vertices and stores its flags before NCLIP.
A nonpositive signed clip returns immediately, leaving screen, depth and
average-depth outputs unchanged. A positive clip stores the first three
screen positions, projects the fourth vertex with RTPS, stores its screen
position and depth, ORs both projection flags and finally stores AVSZ4 OTZ.
The original clip remains the return value even after AVSZ4 changes MAC0.

Eleven pins and one empty barrier remain and are recorded in debt. The
barrier defines architectural `$0` for the original two-branch rejection
path; it emits no CPU instruction. Volatile pointer parameters preserve the
stack-argument loads, including reloading the flags pointer after RTPS.
Four redundant barriers and two redundant pins were removed before integration.

`tools/tests/test_nclip_projection.py` compiles and links the production
source at its retail address, checking all 176 bytes against a fixed retail
SHA-256. The full main executable also retains its retail SHA-1. This entry
has no separate GTE arithmetic emulator test; exact linked bytes are the
acceptance criterion.

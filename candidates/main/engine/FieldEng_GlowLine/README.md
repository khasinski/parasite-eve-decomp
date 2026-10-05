# Field glow line: CPU depth work in C

`func_800D2B58` now shifts and stores both endpoint depths in C. It no longer
uses `gte_stszotz`, whose body contains CPU `sra` and `sw` instructions.
Vector transfers, RTPS, depth reads, coordinate stores and hazard nops each
have their own single-instruction macro.

Native stock GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1 -mcpu=3000`) with
stock MASPSX 2.56 mode produces linked score **0**, with all 1468 bytes
identical to retail `main.exe[0xC3358:0xC3914]`.
Full `make -j32 verify` on darwine also passed, including the whole
`main.exe` comparison and source/debt checks.

The last two differences were an address calculation reusing t2 rather than
v0. A bounded permuter run on darwine found the fix after 1547 iterations
(105 rejected compilations): copy the pinned packet-table pointer into an
ordinary local before the depth-valid branch's packet linking. The source
uses the scoped `drawBuffers` variable for that copy.

Matching debt: three pins, four empty barriers, six hazard nops and the
ordinary packet-table copy. All 128 subsets of removing the seven pins and
barriers were checked; only the full set retained the byte match. Depth
arithmetic and stores remain ordinary C. The pin/barrier counts are recorded
in the debt baseline; the necessary nops and copy are documented here.

Research and verification artifacts:
`/home/hasik/fx-search-archives/glow_line` on darwine, including
`scoped.linked.json`, `permuter/output-0-1/diff.txt` and `main_verify.log`.
The permuter stopped on score zero; no worker was left running.

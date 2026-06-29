#!/usr/bin/env python3
"""Enumerate MIPS code overlays inside PE.IMG (Parasite Eve asset archive).

PE's executable code is not all in SLUS_006.62 (main.exe). Large amounts of
per-mode code (intro, field/room, battle, menu, effects) are stored as MIPS
overlays *inside* PE.IMG and DMA'd at runtime into reserved (zero-filled)
regions of the main exe's address space. This scanner fingerprints PSX MIPS
code -- `jr ra` (0x03E00008) at every function end, `addiu sp,sp,-N` stack
prologues, and `jal` into the main.exe .text range (0x80000000-0x801FFFFF) --
and reports the contiguous code blocks (overlays) it finds.

Usage:  python3 overlay_scan.py /path/to/PE.IMG [--md]
        --md   emit a markdown table (for docs); default is a summary.

Notes/caveats:
- Block boundaries are approximate: overlays that interleave code with data
  (e.g. the intro overlay) register only their code-dense sectors, so the
  func/sector counts under-report such overlays. The *count* of distinct code
  blocks is a good estimate of the overlay population.
- A 2048-byte logical sector = 512 u32 words. Byte offset = sector * 2048;
  this maps 1:1 into the extracted PE.IMG.
"""
import sys
try:
    import numpy as np
except ImportError:
    sys.exit("needs numpy")

SEC = 512  # u32 words per 2048-byte sector


def scan(path):
    a = np.fromfile(path, dtype="<u4")
    nsec = len(a) // SEC
    jrra = (a == 0x03E00008)                      # jr ra
    prol = ((a & 0xFFFF8000) == 0x27BD8000)       # addiu sp,sp,-N
    jalt = ((a & 0xFFF80000) == 0x0C000000)       # jal 0x8000xxxx..0x801Fxxxx

    def per(mask):
        return mask[:nsec * SEC].reshape(nsec, SEC).sum(axis=1)

    cj, cp, ct = per(jrra), per(prol), per(jalt)
    # strict: a code sector ends >=1 function AND has a prologue or several text calls
    code = (cj >= 1) & ((cp >= 1) | (ct >= 3))
    secs = np.nonzero(code)[0].tolist()

    blocks, cur = [], None
    for s in secs:
        if cur and s - cur[1] <= 2:               # tolerate small data gaps
            cur[1] = s
        else:
            if cur:
                blocks.append(cur)
            cur = [s, s]
    if cur:
        blocks.append(cur)

    rows = []
    for a0, b0 in blocks:
        rows.append({
            "sec0": a0, "sec1": b0, "off": a0 * 2048, "nsec": b0 - a0 + 1,
            "funcs": int(cj[a0:b0 + 1].sum()),
            "prologues": int(cp[a0:b0 + 1].sum()),
            "calls": int(ct[a0:b0 + 1].sum()),
        })
    return nsec, rows


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    path = sys.argv[1]
    md = "--md" in sys.argv[2:]
    nsec, rows = scan(path)
    tot = sum(r["nsec"] for r in rows)
    if md:
        print("| # | PE.IMG offset | sectors | size | ~funcs | prologues | text-calls |")
        print("|--:|--------------:|--------:|-----:|-------:|----------:|-----------:|")
        for i, r in enumerate(rows):
            print("| %d | 0x%08X | %d..%d | %dKB | %d | %d | %d |" % (
                i + 1, r["off"], r["sec0"], r["sec1"],
                r["nsec"] * 2048 // 1024, r["funcs"], r["prologues"], r["calls"]))
    print("\nPE.IMG: %d sectors (%d MB). Overlay code blocks: %d. "
          "Total code: %d sectors (~%d KB)." % (
              nsec, nsec * 2048 // (1024 * 1024), len(rows), tot, tot * 2048 // 1024),
          file=sys.stderr)


if __name__ == "__main__":
    main()

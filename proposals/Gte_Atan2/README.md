# Gte_Atan2: exact C reconstruction

`src/main/psyq/libgte/Gte_Atan2.c` matches all 372 retail bytes at 0x80079FB4,
including resolved relocations. SHA-256 of the linked function:
`e5b0edc7308d715c3fd821bd7f75478555d6937ec97c9a8278f6c7746ad0f820`.

The build uses native stock GCC 2.7.2 and unmodified GNU GAS from binutils 2.7,
with MIPS I / R3000 settings. `setup_gas27.sh` downloads the original GNU
archive, verifies its SHA-256, and builds a native assembler. Host-only C89,
POSIX and signal-header compatibility flags require no upstream source edits.
Both native macOS arm64 and Linux x86_64 assembler builds reproduce the bytes.
CI installs and caches this assembler; MASPSX remains unchanged.
The macOS bootstrap uses Homebrew GCC (`brew install gcc`); Linux uses `cc`.
Set `PE_HOST_CC` to choose the host compiler explicitly.

GAS 2.7 emits `div; bnez; nop; break 7` and uses the destination register for
indexed symbolic loads. Contemporary GAS rearranges the division checks;
MASPSX uses AT for the load and adds different MFLO spacing. Pins alone did
not fix those macro-expansion differences.

One v1 pin and four empty barriers retain the zero-axis comparison, place the
index shifts before jumps (GAS fills the delay slots), and keep the table
value in v1 before the quarter-turn correction. These are tracked as debt.
The ordinary function-section attribute avoids GAS 2.7's mandatory 16-byte
padding of its default `.text` section. The linker includes `.text*` with the
existing 4-byte subalignment. No instructions or object bytes are rewritten.

`verify_behavior.py` passes 2489 coordinate pairs plus five axis cases using
the retail lookup table. Its finite input domain excludes INT_MIN, whose
wrapped absolute value may trigger exceptional division or out-of-range
access in the original. The CI regression independently checks the full
linked instruction stream against the retail digest without needing assets.

```sh
tools/scripts/setup_gas27.sh
tools/scripts/cc.sh src/main/psyq/libgte/Gte_Atan2.c /tmp/Gte_Atan2.o
.venv/bin/python proposals/Gte_Atan2/verify_behavior.py /tmp/Gte_Atan2.o
.venv/bin/python -m unittest tools.tests.test_gte_atan2
make check
```

Original assembler source: https://ftp.gnu.org/gnu/binutils/binutils-2.7.tar.gz

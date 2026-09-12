# DsRead2

Production matches all **212/212 linked retail bytes** at `0x80081314` using
native GCC 2.7.2 and unmodified GNU as. There is no instruction asm.

With mode bit `0x100` set, bit `0x20` clears `g_DsStreamNoLocFlag`; otherwise
the flag becomes one. The function installs data/sync callbacks and sends
command `0x1B`. A failed command restores the previous callbacks and returns
zero; success preserves the command result. Without bit `0x100`, it issues
the command directly.

One pin to v0 and one empty input barrier preserve the flag branch's constant
load and store. Assigning one inside the else arm allows the mask test and
constant to reuse v0. Both constraints are recorded in the debt baseline.

`tools/tests/test_libds_read2_disk_kind.py` compiles and links the source at
the retail address and compares the complete instruction stream against a
SHA-256 derived from the original executable, including branch relocations.

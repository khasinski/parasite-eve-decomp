# LIBCD CD_initintr

Production matches all **76/76 linked retail bytes** at `0x8007BBB0` using
native GCC 2.7.2, the existing disabled scheduling passes, and unmodified
GNU as. The full `src/main/psyq/libcd/bios.c` text matches **316/316 bytes**,
including the unchanged CD_initvol neighbor.

CD_initintr clears the ready/sync callback pointers and both full status words,
resets callback handling, and installs Cd_SetIntrMask on interrupt channel two.
The status store now uses the declared D_8009AFC4 global instead of a fabricated
page and an AT register pin. GNU as schedules the store in ResetCallback's
delay slot. No new pins, barriers or instruction asm are needed.

`tools/tests/test_libcd_initintr.py` checks the complete linked TU against a
SHA-256 taken from the retail executable, with the original data and string
addresses. The older standalone candidate remains an exploratory GCC281
variant; the matching production source is bios.c.

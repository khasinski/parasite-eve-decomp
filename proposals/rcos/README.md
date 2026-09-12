# Cosine table lookup

Production `rcos` at `0x80077DC4` matches all **160/160 linked bytes** using
native GCC 2.7.2 and unmodified GNU as. Indexed signed-halfword loads replace
all three instruction-asm table macros; the explicit nop is also gone.
There are no register pins, empty barriers, or instruction asm in this TU.

The function folds the angle into a 12-bit cycle and selects the appropriate
signed quarter-wave table view. Unsigned negation preserves the retail
low-word behavior for INT_MIN without signed-overflow undefined behavior.
`tools/tests/test_cos_translation.py` verifies every linked byte and relocation
against the retail SHA-256. No assembler/compiler patches or postpasses.

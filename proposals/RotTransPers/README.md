# RotTransPers

Production matches **44/44 linked function bytes** at `0x80079244`,
using native GCC 2.7.2 and unmodified MASPSX. The typed source projects the input vector with RTPS, writes SXY2 and IR0, stores FLAG in C,
and returns SZ3 shifted right by two in C.

Every GTE transfer and command has its own single-instruction macro. The
required hazard nop is explicit via `gte_cop2_hazard_slot`. No CPU arithmetic,
loads, stores or branches are hidden in the macros. There are no register
pins or empty barriers, and no compiler/assembler patches or postpasses.

`tools/tests/test_rot_trans.py` compares all linked TU bytes against the retail
SHA-256. For RotTrans this also verifies the original eight trailing zero bytes,
which are padding and are not counted as new function bytes.

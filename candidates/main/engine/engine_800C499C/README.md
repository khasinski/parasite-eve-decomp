# engine_800C499C (func_800C499C, 0x4B4) - parked, not attempted in C

The function writes the scratchpad base into a global pointer and then
addresses scratchpad fields as folded constants:

    lui  v0, 0x1F80              ; D_800F33B4 = scratchpad (0x1F800000)
    sw   v0, %lo(D_800F33B4)(at)
    lui  t2, 0x1F80 ; ori t2, t2, 0x1C   ; &scratch->matrix as a constant

Retail therefore needs the compiler to know the scratchpad address as an
integer constant, which in C means an integer-to-pointer cast
(`(Foo *)0x1F800000`). The project rules forbid pointer/integer casts, and
a linker symbol (`D_1F800000`) assembles to lui+addiu (and lui+addiu with
%lo for the +0x1C form) instead of retail's bare lui / lui+ori, so it cannot
byte-match. Revisit only if a sanctioned scratchpad constant form appears.

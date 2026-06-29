/* ASM-ORIGIN: PSY-Q SDK library routine, originally hand-written assembly.
 * No C decompilation exists or is expected; kept as raw MIPS only for byte-match.
 * A PC port replaces this natively (GPU/GTE/CD/SPU lib, libc, or crt0) rather
 * than decompiling it. See docs/PORT_BOUNDARY.md section 1a. */
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
asm(
".text\n"
".globl __do_global_dtors\n"
".ent __do_global_dtors\n"
"__do_global_dtors:\n"
"    .word 0x3c080000\n"
"    .reloc .-4, R_MIPS_HI16, D_80094538\n"
"    .word 0x8d080000\n"
"    .reloc .-4, R_MIPS_LO16, D_80094538\n"
"    .word 0x27bdfff0\n"
"    .word 0xafb00004\n"
"    .word 0xafb10008\n"
"    .word 0xafbf000c\n"
"    .word 0x1100000d\n"
"    .word 0x00000000\n"
"    .word 0x3c100000\n"
"    .reloc .-4, R_MIPS_HI16, jtbl_80010000\n"
"    .word 0x26100000\n"
"    .reloc .-4, R_MIPS_LO16, jtbl_80010000\n"
"    .word 0x3c110000\n"
"    .word 0x26310000\n"
"    .word 0x12200007\n"
"    .word 0x00000000\n"
"    .word 0x8e080000\n"
"    .word 0x26100004\n"
"    .word 0x0100f809\n"
"    .word 0x2631ffff\n"
"    .word 0x1620fffb\n"
"    .word 0x00000000\n"
"    .word 0x8fbf000c\n"
"    .word 0x8fb10008\n"
"    .word 0x8fb00004\n"
"    .word 0x27bd0010\n"
"    .word 0x03e00008\n"
"    .word 0x00000000\n"
".end __do_global_dtors\n"
);

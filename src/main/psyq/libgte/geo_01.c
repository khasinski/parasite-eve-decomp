/* ASM-ORIGIN: PSY-Q SDK library routine, originally hand-written assembly.
 * No C decompilation exists or is expected; kept as raw MIPS only for byte-match.
 * A PC port replaces this natively (GPU/GTE/CD/SPU lib, libc, or crt0) rather
 * than decompiling it. See docs/PORT_BOUNDARY.md section 1a. */
asm(".text");
asm(".set noat");
asm(".set noreorder");

asm(".globl rcos");
asm("rcos:");
asm(".word 0x04810002");
asm("nop");
asm("subu $4,$0,$4");
asm("1:");
asm("andi $4,$4,0xFFF");
asm("slti $2,$4,0x801");
asm(".word 0x10400010");
asm("slti $2,$4,0x401");
asm(".word 0x10400008");
asm("addiu $2,$0,0x400");
asm("subu $2,$2,$4");
asm("sll $2,$2,1");
asm("lui $1,%hi(g_GteSinTable)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(g_GteSinTable)($1)");
asm(".word 0x0801DF97");
asm("nop");
asm("2:");
asm("sll $2,$4,1");
asm("lui $1,%hi(D_8009509C)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(D_8009509C)($1)");
asm(".word 0x0801DF97");
asm("subu $2,$0,$2");
asm("3:");
asm("slti $2,$4,0xC01");
asm(".word 0x14400007");
asm("addiu $2,$0,0xC00");
asm("sll $2,$4,1");
asm("lui $1,%hi(D_8009409C)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(D_8009409C)($1)");
asm(".word 0x0801DF97");
asm("nop");
asm("4:");
asm("subu $2,$2,$4");
asm("sll $2,$2,1");
asm("lui $1,%hi(g_GteSinTable)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(g_GteSinTable)($1)");
asm("nop");
asm("subu $2,$0,$2");
asm("5:");
asm("jr $31");
asm("nop");

asm(".set reorder");
asm(".set at");

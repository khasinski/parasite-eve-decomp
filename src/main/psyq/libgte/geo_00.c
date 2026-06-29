/* ASM-ORIGIN: PSY-Q SDK library routine, originally hand-written assembly.
 * No C decompilation exists or is expected; kept as raw MIPS only for byte-match.
 * A PC port replaces this natively (GPU/GTE/CD/SPU lib, libc, or crt0) rather
 * than decompiling it. See docs/PORT_BOUNDARY.md section 1a. */
asm(".text");
asm(".set noat");
asm(".set noreorder");

asm(".globl rsin");
asm("rsin:");
asm("addiu $29,$29,-0x18");
asm(".word 0x04800005");
asm("sw $31,0x10($29)");
asm(".word 0x0C01DF4C");
asm("andi $4,$4,0xFFF");
asm(".word 0x0801DF48");
asm("nop");
asm("1:");
asm("subu $4,$0,$4");
asm(".word 0x0C01DF4C");
asm("andi $4,$4,0xFFF");
asm("subu $2,$0,$2");
asm("2:");
asm("lw $31,0x10($29)");
asm("addiu $29,$29,0x18");
asm("jr $31");
asm("nop");

asm(".globl sin_1");
asm("sin_1:");
asm("slti $2,$4,0x801");
asm(".word 0x10400010");
asm("slti $2,$4,0x401");
asm(".word 0x10400006");
asm("sll $2,$4,1");
asm("lui $1,%hi(g_GteSinTable)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(g_GteSinTable)($1)");
asm(".word 0x0801DF6E");
asm("nop");
asm("1:");
asm("addiu $2,$0,0x800");
asm("subu $2,$2,$4");
asm("sll $2,$2,1");
asm("lui $1,%hi(g_GteSinTable)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(g_GteSinTable)($1)");
asm(".word 0x0801DF6E");
asm("nop");
asm("3:");
asm("slti $2,$4,0xC01");
asm(".word 0x14400009");
asm("sll $2,$4,1");
asm("addiu $2,$0,0x1000");
asm("subu $2,$2,$4");
asm("sll $2,$2,1");
asm("lui $1,%hi(g_GteSinTable)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(g_GteSinTable)($1)");
asm(".word 0x0801DF6E");
asm("subu $2,$0,$2");
asm("4:");
asm("lui $1,%hi(D_8009489C)");
asm("addu $1,$1,$2");
asm("lh $2,%lo(D_8009489C)($1)");
asm("nop");
asm("subu $2,$0,$2");
asm("5:");
asm("jr $31");
asm("nop");

asm(".set reorder");
asm(".set at");

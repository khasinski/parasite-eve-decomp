asm(".text");
asm(".set noreorder");
asm(".globl ChangeClearRCnt");
asm("ChangeClearRCnt:");
asm("addiu $10,$0,0xC0");
asm("jr $10");
asm("addiu $9,$0,0xA");

asm(".text");
asm(".set noreorder");
asm(".globl WaitEvent");
asm("WaitEvent:");
asm("addiu $10,$0,0xB0");
asm("jr $10");
asm("addiu $9,$0,0xA");

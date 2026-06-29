asm(".text");
asm(".set noreorder");
asm(".globl TestEvent");
asm("TestEvent:");
asm("addiu $10,$0,0xB0");
asm("jr $10");
asm("addiu $9,$0,0xB");

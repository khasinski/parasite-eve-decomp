asm(".text");
asm(".set noreorder");
asm(".globl ExitCriticalSection");
asm("ExitCriticalSection:");
asm("addiu $4,$0,0x2");
asm("syscall");
asm("jr $31");
asm("nop");

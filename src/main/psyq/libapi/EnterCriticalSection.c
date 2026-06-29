asm(".text");
asm(".set noreorder");
asm(".globl EnterCriticalSection");
asm("EnterCriticalSection:");
asm("addiu $4,$0,0x1");
asm("syscall");
asm("jr $31");
asm("nop");

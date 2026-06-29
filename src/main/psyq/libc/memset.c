asm(".text");
asm(".set noreorder");
asm(".globl memset");
asm("memset:");
asm("addiu   $10,$0,0xA0");
asm("jr      $10");
asm("addiu   $9,$0,0x2B");
asm("nop");

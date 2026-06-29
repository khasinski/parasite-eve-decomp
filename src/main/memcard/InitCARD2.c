asm(".text");
asm(".set noreorder");
asm(".globl InitCARD2");
asm("InitCARD2:");
asm("addiu   $10,$0,0xB0");
asm("jr      $10");
asm("addiu   $9,$0,0x4A");
asm("nop");

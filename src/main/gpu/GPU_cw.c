asm(".text");
asm(".set noreorder");
asm(".globl GPU_cw");
asm("GPU_cw:");
asm("addiu   $10,$0,0xA0");
asm("jr      $10");
asm("addiu   $9,$0,0x49");
asm("nop");

asm(".text");
asm(".set noreorder");
asm(".globl SysEnqIntRP");
asm("SysEnqIntRP:");
asm("addiu   $10,$0,0xC0");
asm("jr      $10");
asm("addiu   $9,$0,0x2");
asm("nop");

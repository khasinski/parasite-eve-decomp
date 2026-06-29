extern int g_MemCardEventF400Spec0004Flag;
extern int g_MemCardRemovedEventPending;
extern int g_MemCardEventF400Spec2000Flag;
extern int g_MemCardEventF000Spec0004Flag;
extern int g_MemCardEventF000Spec8000Flag;
extern int g_MemCardEventF000Spec2000Flag;

asm(".globl func_80042BD8");
asm("func_80042BD8 = MemCard_OnEventF400Spec0004");
asm(".globl func_80042BEC");
asm("func_80042BEC = MemCard_OnEventF400Spec8000");
asm(".globl func_80042C00");
asm("func_80042C00 = MemCard_OnEventF400Spec0100");
asm(".globl func_80042C14");
asm("func_80042C14 = MemCard_OnEventF400Spec2000");
asm(".globl func_80042C28");
asm("func_80042C28 = MemCard_OnEventF000Spec0004");
asm(".globl func_80042C3C");
asm("func_80042C3C = MemCard_OnEventF000Spec8000");
asm(".globl func_80042C50");
asm("func_80042C50 = MemCard_OnEventF000Spec0100");
asm(".globl func_80042C64");
asm("func_80042C64 = MemCard_OnEventF000Spec2000");

int MemCard_OnEventF400Spec0004(void)
{
    g_MemCardEventF400Spec0004Flag = 1;
    return 0;
}

int MemCard_OnEventF400Spec8000(void)
{
    g_MemCardRemovedEventPending = 1;
    return 0;
}

int MemCard_OnEventF400Spec0100(void)
{
    g_MemCardRemovedEventPending = 1;
    return 0;
}

int MemCard_OnEventF400Spec2000(void)
{
    g_MemCardEventF400Spec2000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec0004(void)
{
    g_MemCardEventF000Spec0004Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec8000(void)
{
    g_MemCardEventF000Spec8000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec0100(void)
{
    g_MemCardEventF000Spec8000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec2000(void)
{
    g_MemCardEventF000Spec2000Flag = 1;
    return 0;
}

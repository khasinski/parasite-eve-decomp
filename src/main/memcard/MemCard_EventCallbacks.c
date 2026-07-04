extern int g_MemCardEventF400Spec0004Flag;
extern int g_MemCardRemovedEventPending;
extern int g_MemCardEventF400Spec2000Flag;
extern int g_MemCardEventF000Spec0004Flag;
extern int g_MemCardEventF000Spec8000Flag;
extern int g_MemCardEventF000Spec2000Flag;

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

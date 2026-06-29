extern int g_MemCardActivePortOneBased;

int MemCard_GetActivePort(void)
{
    return g_MemCardActivePortOneBased - 1;
}

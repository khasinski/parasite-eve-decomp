extern int g_MemCardActivePortOneBased;
extern unsigned char D_800A0ED5[];

void MemCard_MarkActivePortState13(void) {
    int portIndex = g_MemCardActivePortOneBased - 1;
    D_800A0ED5[portIndex * 1048] = 0xD;
}

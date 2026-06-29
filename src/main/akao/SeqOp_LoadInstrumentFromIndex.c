extern int g_AkaoInstrumentTable;

void SeqOp_LoadInstrumentFromIndex(void *ptr) {
    int value;
    register int base asm("$3");

    base = (int)&g_AkaoInstrumentTable + (*(unsigned short *)((char *)ptr + 0x5A) << 6);

    value = *(unsigned char *)(base + 0x8);
    *(short *)((char *)ptr + 0x10E) = value;
    value = *(unsigned char *)(base + 0x9);
    *(short *)((char *)ptr + 0x110) = value;
    value = *(unsigned char *)(base + 0xA);
    *(short *)((char *)ptr + 0x112) = value;
    value = *(unsigned char *)(base + 0xB);
    *(short *)((char *)ptr + 0x114) = value;
    value = *(unsigned char *)(base + 0xC);
    *(short *)((char *)ptr + 0x116) = value;
    value = *(unsigned char *)(base + 0xD);
    *(int *)((char *)ptr + 0x100) = value;
    value = *(unsigned char *)(base + 0xE);
    *(int *)((char *)ptr + 0x104) = value;
    value = *(unsigned int *)((char *)ptr + 0xF4);
    base = *(unsigned char *)(base + 0xF);
    value |= 0xFF00;
    *(unsigned int *)((char *)ptr + 0xF4) = value;
    *(int *)((char *)ptr + 0x108) = base;
}

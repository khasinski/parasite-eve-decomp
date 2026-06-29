void Akao_SetNotePitch(void *ptr, int arg);

void Akao_InitVoiceState(void *ptr, void *script) {
    *(short *)((char *)ptr + 0x6C) = 0x6E00;
    *(void **)ptr = script;
    *(short *)((char *)ptr + 0xDE) = 0;
    *(short *)((char *)ptr + 0xE0) = 0;
    *(short *)((char *)ptr + 0x82) = 0;
    *(int *)((char *)ptr + 0x34) = 0;
    *(short *)((char *)ptr + 0xE4) = 0;
    *(short *)((char *)ptr + 0x7A) = 0;
    *(short *)((char *)ptr + 0xD2) = 0;
    *(short *)((char *)ptr + 0xD0) = 0;
    *(int *)((char *)ptr + 0x44) = 0x32000000;
    *(short *)((char *)ptr + 0x72) = 0;
    *(short *)((char *)ptr + 0xCE) = 0;
    *(int *)((char *)ptr + 0x38) = 0;
    *(short *)((char *)ptr + 0xEC) = 0;
    *(short *)((char *)ptr + 0x84) = 0;
    *(short *)((char *)ptr + 0xB4) = 0;
    *(short *)((char *)ptr + 0xA6) = 0;
    *(short *)((char *)ptr + 0x94) = 0;
    *(short *)((char *)ptr + 0xB6) = 0;
    *(short *)((char *)ptr + 0xA8) = 0;
    *(short *)((char *)ptr + 0x96) = 0;
    *(short *)((char *)ptr + 0xBC) = 0;
    *(short *)((char *)ptr + 0xBA) = 0;
    Akao_SetNotePitch(ptr, 0);
}

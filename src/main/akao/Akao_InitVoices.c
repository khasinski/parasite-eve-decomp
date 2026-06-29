/* CC1_PROFILE: build0001 */
typedef unsigned short u16;

extern char *g_AkaoCurTrack;
extern char g_AkaoDefaultVoiceProgram[];

void Akao_InitVoices(int arg0, char *arg1) {
    unsigned int i;
    int mode3;
    int mode1;
    char *fallback;
    int mode5;
    char *voice;
    register char *field asm("$4");
    int flags;

    if (arg0 == 0) {
        if (*(int *)(g_AkaoCurTrack + 4) != 0) {
            goto body;
        }
        return;
    } else if (arg0 != *(u16 *)(g_AkaoCurTrack + 0x54)) {
        return;
    }

body:
    *(int *)(g_AkaoCurTrack + 0x18) = 0xFFFFFF;

    i = 0;
    mode3 = 3;
    mode1 = 1;
    fallback = g_AkaoDefaultVoiceProgram;
    mode5 = 5;
    field = arg1 + 0x116;
    voice = arg1;

    do {
        i++;
        *(u16 *)(field - 0xC0) = mode3;
        *(u16 *)(field - 0xBE) = mode1;
        *(char **)(voice + 0) = fallback;
        flags = *(int *)(field - 0x22);
        voice += 0x11C;
        *(u16 *)(field + 0) = mode5;
        flags |= 0x4400;
        *(int *)(field - 0x22) = flags;
        field += 0x11C;
    } while (i < 0x18);
}

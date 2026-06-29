/* CC1_PROFILE: build0001 */
typedef unsigned int u32;

extern char *g_AkaoCurTrack;

void Seq_ClearTrackVoiceId(char *arg0, unsigned int arg1) {
    unsigned int i;
    int bit;
    int reset;
    char *ptr;
    char *state;

    if (arg1 < 24) {
        i = 0;
        reset = 24;
        state = g_AkaoCurTrack;
        bit = 1;
        ptr = arg0 + 0xF0;
        do {
            if (*(int *)ptr == arg1) {
                *(int *)ptr = reset;
                *(u32 *)(state + 0x14) &= ~(bit << i);
            }
            i++;
            ptr += 0x11C;
        } while (i < 24);
    }
}

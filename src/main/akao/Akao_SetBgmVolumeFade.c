/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern void * volatile g_AkaoBgmHandle[];
extern int g_AkaoPendingBgmVolumeFade;
int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4);

void Akao_SetBgmVolumeFade(void) {
    void * volatile *slot;
    register int value asm("$2");

    slot = g_AkaoBgmHandle;
    if (*slot != 0) {
        value = 0x7F;
        Akao_SendTableCommand(*slot, 0x450, 0x100, 0x80, value);
        goto done;
    }
    value = 0;

done:
    g_AkaoPendingBgmVolumeFade = value;
}

/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
extern void (*D_8009AFB4)(void);
extern unsigned char D_8009AFC4;
extern int D_8009AF2C[];
int CD_cw(int command, void *parameter, void *result, int mode);
int func_8007A4D0(int command, void *parameter, void *result) {
    int retries = 3;
    int status = 0;
    unsigned int code = (unsigned char)command;
    void (*saved)(void) = D_8009AFB4;
    int *needs_location = &D_8009AF2C[code];
    do {
        D_8009AFB4 = 0;
        if (code != 1 && (D_8009AFC4 & 0x10))
            CD_cw(1, 0, 0, 0);
        if (parameter && *needs_location && CD_cw(2, parameter, result, 0))
            continue;
        D_8009AFB4 = saved;
        if (!CD_cw((unsigned char)command, parameter, result, 0))
            return status + 1;
    } while (--retries != -1);
    D_8009AFB4 = saved;
    status = -1;
    return status + 1;
}

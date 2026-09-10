/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

extern volatile u8 *D_8009B27C, *D_8009B280, *D_8009B284, *D_8009B288;
extern int D_8009AFC0, D_8009AFCC;
extern u8 D_8009AFD5;
extern int D_8009B17C[], D_8009B07C[];
extern char *D_8009AFDC[];
extern u8 D_800A3460[8], D_800A3468[8], D_800A3470[8];
extern char D_80011B44[], D_80011B50[], D_80011B6C[], D_80011B80[];
int printf(const char *format, ...);
int puts(const char *text);

#include "../libcd_bios_helpers.h"

int getintr(void) {
    volatile u8 interrupt;
    volatile u8 result[8];
    int count;
    int errors = 0;
    int i;
    CdInterruptEvents *events;
    *D_8009B27C = 1;
    interrupt = *D_8009B288 & 7;
    if (interrupt == 0) return 0;
    while (interrupt != (*D_8009B288 & 7)) interrupt = *D_8009B288 & 7;
    for (count = 0; count < 8; count++) {
        if (!(*D_8009B27C & 0x20)) break;
        result[count] = *D_8009B280;
    }
    for (i = count; i < 8; i++) result[i] = 0;
    *D_8009B27C = 1;
    *D_8009B288 = 7;
    *D_8009B284 = 7;
    if (interrupt != 3 || D_8009B17C[D_8009AFD5]) {
        if (!(D_8009AFC4 & 0x10) && (result[0] & 0x10)) D_8009AFCC++;
        D_8009AFC4 = result[0];
        errors = D_8009AFC4 & 0x1D;
        D_8009AFC8 = result[1];
    }
    if (interrupt == 5 && D_8009AFC0 > 0) {
        printf(D_80011B44);
        if (D_8009AFC0 > 0)
            printf(D_80011B50, D_8009AFDC[D_8009AFD5], D_8009AFC4, D_8009AFC8);
    }
    switch (interrupt) {
    case 3:
        if (errors) {
            D_8009B294.sync = 5;
            copy_result(D_800A3460, result);
            return 2;
        }
        if (D_8009B07C[D_8009AFD5]) {
            D_8009B294.sync = 3;
            copy_result(D_800A3460, result);
            return 1;
        }
        D_8009B294.sync = 2;
        copy_result(D_800A3460, result);
        return 2;
    case 2:
        D_8009B294.sync = errors ? 5 : 2;
        copy_result(D_800A3460, result);
        return 2;
    case 1:
        if (errors && count == 1) errors = 0;
        events = &D_8009B294;
        events->ready = errors ? 5 : 1;
        copy_result(D_800A3468, result);
        *D_8009B27C = 0;
        *D_8009B288 = 0;
        return 4;
    case 4:
        D_8009B294.end = 4;
        D_8009B294.ready = D_8009B294.end;
        copy_result(D_800A3470, result);
        copy_result(D_800A3468, result);
        return 4;
    case 5:
        D_8009B294.ready = 5;
        D_8009B294.sync = D_8009B294.ready;
        copy_result(D_800A3460, result);
        copy_result(D_800A3468, result);
        return 6;
    default:
        puts(D_80011B6C);
        printf(D_80011B80, interrupt);
        return 0;
    }
}

typedef unsigned char u8;
typedef unsigned int u32;

extern struct { char _[4]; } D_8009B27C_o __asm__("D_8009B27C");
extern struct { char _[4]; } D_8009B280_o __asm__("D_8009B280");
extern struct { char _[4]; } D_8009B284_o __asm__("D_8009B284");
extern struct { char _[4]; } D_8009B288_o __asm__("D_8009B288");
extern struct { char _[4]; } D_8009AFC0_o __asm__("D_8009AFC0");
extern struct { char _[4]; } D_8009AFC4_o __asm__("D_8009AFC4");
extern struct { char _[4]; } D_8009AFC8_o __asm__("D_8009AFC8");
extern struct { char _[4]; } D_8009AFCC_o __asm__("D_8009AFCC");
extern struct { char _[1]; } D_8009AFD5_o __asm__("D_8009AFD5");
extern struct { char _[20]; } D_8009B07C_o __asm__("D_8009B07C");
extern struct { char _[20]; } D_8009B17C_o __asm__("D_8009B17C");
extern struct { char _[8]; } D_8009B294_o __asm__("D_8009B294");
extern struct { char _[8]; } D_800A3460_o __asm__("D_800A3460");
extern struct { char _[8]; } D_800A3468_o __asm__("D_800A3468");
extern struct { char _[8]; } D_800A3470_o __asm__("D_800A3470");
extern struct { char _[4]; } D_80011B44_o __asm__("D_80011B44");
extern struct { char _[4]; } D_80011B50_o __asm__("D_80011B50");
extern struct { char _[4]; } D_80011B6C_o __asm__("D_80011B6C");
extern struct { char _[4]; } D_80011B80_o __asm__("D_80011B80");
extern struct { char _[16]; } D_8009AFDC_o __asm__("D_8009AFDC");

#define D_8009B27C (*(volatile u8 **)&D_8009B27C_o)
#define D_8009B280 (*(volatile u8 **)&D_8009B280_o)
#define D_8009B284 (*(volatile u8 **)&D_8009B284_o)
#define D_8009B288 (*(volatile u8 **)&D_8009B288_o)
#define D_8009AFC0 (*(int *)&D_8009AFC0_o)
#define D_8009AFC4 (*(u32 *)&D_8009AFC4_o)
#define D_8009AFC8 (*(u32 *)&D_8009AFC8_o)
#define D_8009AFCC (*(int *)&D_8009AFCC_o)
#define D_8009AFD5 (*(u8 *)&D_8009AFD5_o)
#define D_8009B07C ((u32 *)&D_8009B07C_o)
#define D_8009B17C ((u32 *)&D_8009B17C_o)
#define D_8009B294 ((u8 *)&D_8009B294_o)
#define D_800A3460 ((u8 *)&D_800A3460_o)
#define D_800A3468 ((u8 *)&D_800A3468_o)
#define D_800A3470 ((u8 *)&D_800A3470_o)
#define D_8009AFDC ((u32 *)&D_8009AFDC_o)

int printf(const char *fmt, ...);
int puts(const char *str);

int getintr(void) {
    u8 result[8];
    u8 intr;
    int i;
    int status_mask;
    u8 *dst;

    *D_8009B27C = 1;
    intr = *D_8009B288 & 7;
    if (intr == 0) {
        return 0;
    }

    do {
        intr = *D_8009B288 & 7;
    } while (intr != (*D_8009B288 & 7));

    i = 0;
    while ((*D_8009B27C & 0x20) && i < 8) {
        result[i++] = *D_8009B280;
    }
    while (i < 8) {
        result[i++] = 0;
    }

    *D_8009B27C = 1;
    *D_8009B288 = 7;
    *D_8009B284 = 7;

    status_mask = 0;
    if (intr != 3 || D_8009B17C[D_8009AFD5] != 0) {
        if (!(D_8009AFC4 & 0x10) && (result[0] & 0x10)) {
            D_8009AFCC++;
        }
        D_8009AFC4 = result[0];
        D_8009AFC8 = result[1];
        status_mask = result[0] & 0x1D;
    }

    if (intr == 5 && D_8009AFC0 > 0) {
        printf((char *)&D_80011B44_o);
        if (D_8009AFC0 > 0) {
            printf((char *)&D_80011B50_o, D_8009AFDC[D_8009AFD5],
                   D_8009AFC4, D_8009AFC8);
        }
    }

    switch (intr) {
    case 1:
        if (status_mask != 0 && D_8009B294[1] == 1) {
            status_mask = 0;
        }
        D_8009B294[1] = status_mask ? 5 : 1;
        dst = D_800A3468;
        for (i = 0; i < 8; i++) {
            dst[i] = result[i];
        }
        *D_8009B27C = 0;
        *D_8009B288 = 0;
        return 4;

    case 2:
        D_8009B294[0] = status_mask ? 5 : 2;
        dst = D_800A3460;
        for (i = 0; i < 8; i++) {
            dst[i] = result[i];
        }
        return 2;

    case 3:
        if (status_mask != 0) {
            D_8009B294[0] = 5;
            dst = D_800A3460;
            for (i = 0; i < 8; i++) {
                dst[i] = result[i];
            }
            return 2;
        }

        if (D_8009B07C[D_8009AFD5] != 0) {
            D_8009B294[0] = 3;
            dst = D_800A3460;
            for (i = 0; i < 8; i++) {
                dst[i] = result[i];
            }
            return 1;
        }

        D_8009B294[0] = 2;
        dst = D_800A3460;
        for (i = 0; i < 8; i++) {
            dst[i] = result[i];
        }
        return 2;

    case 4:
        D_8009B294[2] = 4;
        D_8009B294[1] = D_8009B294[2];
        dst = D_800A3470;
        for (i = 0; i < 8; i++) {
            dst[i] = result[i];
        }
        dst = D_800A3468;
        for (i = 0; i < 8; i++) {
            dst[i] = result[i];
        }
        return 4;

    case 5:
        D_8009B294[1] = 5;
        D_8009B294[0] = D_8009B294[1];
        dst = D_800A3460;
        for (i = 0; i < 8; i++) {
            dst[i] = result[i];
        }
        dst = D_800A3468;
        for (i = 0; i < 8; i++) {
            dst[i] = result[i];
        }
        return 6;

    default:
        puts((char *)&D_80011B6C_o);
        printf((char *)&D_80011B80_o, intr);
        return 0;
    }
}

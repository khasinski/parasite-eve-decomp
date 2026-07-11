/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009574C_o __asm__("D_8009574C");
extern struct { char _[16]; } D_800957CC_o __asm__("D_800957CC");
extern struct { char _[16]; } D_800957D8_o __asm__("D_800957D8");
extern struct { char _[16]; } D_8009574E_o __asm__("D_8009574E");
extern struct { char _[16]; } D_80095744_o __asm__("D_80095744");
extern struct { char _[16]; } D_80095748_o __asm__("D_80095748");
extern char D_800117E0[];
extern char D_80011800[];

#define D_8009574C ((u8 *)&D_8009574C_o)
#define D_800957CC ((u16 *)&D_800957CC_o)
#define D_800957D8 ((u16 *)&D_800957D8_o)
#define D_8009574E (*(u8 *)&D_8009574E_o)
#define D_80095744 (*(u32 *)&D_80095744_o)
#define D_80095748 (*(void (**)(void *, int))&D_80095748_o)

int printf(char *fmt, ...);
int GPU_memset(void *dst, int value, int size);
int ResetCallback(void);
int GPU_cw(int cmd);
int Gpu_InitDmaQueue(int mode);

void Render_InitEntityPool(int mode) {
    int index;
    int (*callback)(int);

    index = mode & 7;
    if ((index == 0) || (index == 3)) {
        printf(D_800117E0, D_8009574C, &D_8009574C[0x80]);
    } else if (index != 5) {
        if (D_8009574E >= 2) {
            D_80095748(D_80011800, mode);
        }
        callback = *(int (**)(int))(D_80095744 + 0x34);
        callback(1);
        return;
    }

    GPU_memset(D_8009574C, 0, 0x80);
    ResetCallback();
    GPU_cw(D_80095744 & 0xFFFFFF);
    D_8009574C[0] = Gpu_InitDmaQueue(mode);
    D_8009574C[1] = 1;
    *(u16 *)&D_8009574C[4] = D_800957CC[D_8009574C[0] * 2];
    *(u16 *)&D_8009574C[6] = D_800957D8[D_8009574C[0] * 2];
    GPU_memset(&D_8009574C[0x10], -1, 0x5C);
    GPU_memset(&D_8009574C[0x6C], -1, 0x14);
}

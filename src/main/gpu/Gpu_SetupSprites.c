/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned short u16;

extern u8 g_BattleSpritePrimCountdown;
extern char D_8009E974[];
extern char D_8009E975[];
extern char D_8009E976[];
extern char D_8009EC40[];
extern char D_8009EC4E[];

void Gpu_SetupSprites(void *arg0, int arg1, int arg2) {
    void *obj;
    int i;
    char *dst_base;
    int src_base;

    obj = arg0;
    g_BattleSpritePrimCountdown = 0x1E;
    i = 0;
    dst_base = D_8009EC40;
    arg2 = (u8)arg2;
    src_base = ((arg2 << 3) - arg2) << 2;

    do {
        int i_u8;
        int dst_offset;
        register int src_offset asm("$3");
        char *dst;
        u16 val;

        i_u8 = (u8)i;
        dst_offset = ((i_u8 << 3) - i_u8) << 2;
        asm volatile("" : "=r"(dst_offset) : "0"(dst_offset));
        src_offset = (((((i_u8 * 3) << 3) - i_u8) << 2) - i_u8) << 2;
        src_offset = src_base + src_offset;
        dst = (char *)(dst_offset + (int)dst_base);

        dst[0xC] = *(u8 *)(D_8009E974 + src_offset);
        dst[0xD] = *(u8 *)(D_8009E975 + src_offset);
        *(u16 *)(dst + 0x8) = *(u16 *)((char *)obj + 0x210) - 8;
        *(u16 *)(dst + 0xA) = *(u16 *)((char *)obj + 0x212) - 0x10;
        val = *(u16 *)(D_8009E976 + src_offset);
        i++;
        *(u16 *)(D_8009EC4E + dst_offset) = val;
    } while ((u8)i < 2);
}

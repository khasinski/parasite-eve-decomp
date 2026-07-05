/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;
typedef unsigned short u16;

extern int g_ActiveDrawSlot[];
extern u8 g_BattleSpritePrimCountdown;
extern char D_8009EC38[];
extern char D_8009EC4A[];
extern char *g_OtBufferTable[];

void AddPrim(unsigned int *ot, unsigned int *prim);

void Gpu_QueuePrimitive(void) {
    int index;
    int prim_offset;
    int ot_offset;
    void *prim;
    char *ot;

    index = g_ActiveDrawSlot[0];
    prim_offset = ((index << 3) - index) << 2;
    ot_offset = index << 2;
    *(u16 *)(D_8009EC4A + prim_offset) = *(u16 *)(D_8009EC4A + prim_offset) - 2;
    asm volatile("" ::: "memory");
    prim = D_8009EC38 + prim_offset;
    ot = *(char **)((char *)g_OtBufferTable + ot_offset);
    AddPrim(ot + 0x1C, prim);
    g_BattleSpritePrimCountdown--;
}

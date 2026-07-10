typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} SVECTOR;

typedef struct {
    int x;
    int y;
    int z;
    int pad;
} VECTOR;

typedef struct {
    int data[8];
} MATRIX;

extern struct { char _[16]; } D_80010DE4_o __asm__("D_80010DE4");
extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009D250_o __asm__("D_8009D250");
extern struct { char _[16]; } D_8009E360_o __asm__("D_8009E360");
extern struct { char _[16]; } D_8009E460_o __asm__("D_8009E460");
extern struct { char _[16]; } D_8009E468_o __asm__("D_8009E468");
extern struct { char _[16]; } D_8009E498_o __asm__("D_8009E498");
extern struct { char _[16]; } D_8009E4A0_o __asm__("D_8009E4A0");
extern struct { char _[16]; } D_8009E4A2_o __asm__("D_8009E4A2");
extern struct { char _[16]; } D_8009E4A4_o __asm__("D_8009E4A4");
extern struct { char _[16]; } D_8009E4A6_o __asm__("D_8009E4A6");
extern struct { char _[16]; } D_8009E4A8_o __asm__("D_8009E4A8");
extern struct { char _[16]; } D_8009E4B2_o __asm__("D_8009E4B2");
extern struct { char _[16]; } D_8009E4B6_o __asm__("D_8009E4B6");
extern struct { char _[16]; } D_8009E4D8_o __asm__("D_8009E4D8");
extern struct { char _[16]; } D_8009E4DC_o __asm__("D_8009E4DC");
extern struct { char _[16]; } D_8009E4DD_o __asm__("D_8009E4DD");
extern struct { char _[16]; } D_8009E4DE_o __asm__("D_8009E4DE");
extern struct { char _[16]; } D_800B0E38_o __asm__("D_800B0E38");

#define g_ActiveDrawSlot (*(int *)&D_8009CDDC_o)
#define D_8009D250 (*(u32 *)&D_8009D250_o)
#define S16_AT(base, offset) (*(s16 *)((u8 *)(base) + (offset)))
#define U16_AT(base, offset) (*(u16 *)((u8 *)(base) + (offset)))
#define U8_AT(base, offset) (*(u8 *)((u8 *)(base) + (offset)))
#define ACTIVE_INDEX g_ActiveDrawSlot
#define ACTIVE_OT (*(u8 **)((u8 *)&D_800B0E38_o + (ACTIVE_INDEX << 2)))
#define STATUS_TRI ((u8 *)&D_8009E4D8_o + ACTIVE_INDEX * 20)
#define STATUS_QUAD_A ((u8 *)&D_8009E4A8_o + (ACTIVE_INDEX << 5))
#define STATUS_QUAD_B ((u8 *)&D_8009E498_o + (ACTIVE_INDEX << 5))
#define STATUS_BASE ((u8 *)&D_8009E460_o + ((ACTIVE_INDEX << 3) - ACTIVE_INDEX) * 4)
#define STATUS_TAG ((u8 *)&D_8009E468_o + ((ACTIVE_INDEX << 3) - ACTIVE_INDEX) * 4)
#define STATUS_POS_BASE ((u8 *)&D_8009E360_o + ((ACTIVE_INDEX << 1) + ACTIVE_INDEX) * 16)

void *memset(void *dst, int value, unsigned int size);
void RotMatrix(void *rot, void *matrix);
void SetRotMatrix(void *matrix);
void *TransMatrix(void *matrix, void *translation);
void SetTransMatrix(void *matrix);
void RotTrans(void *src, void *dst, int *flag);
void AddPrim(void *ot, void *prim);

void Battle_BuildStatusPrimHeader(void *entity, int compact) {
    SVECTOR verts[3];
    SVECTOR rot;
    MATRIX matrix;
    VECTOR trans;
    VECTOR out0;
    VECTOR out1;
    VECTOR out2;
    int flag;
    int i;
    int x;
    int y;
    int red;
    int size;
    u8 *prim;

    for (i = 0; i < 3; i++) {
        verts[i] = *(SVECTOR *)((u8 *)&D_80010DE4_o + i * sizeof(SVECTOR));
    }

    memset(&rot, 0, sizeof(rot));
    rot.x = 0;
    rot.y = 0;
    rot.z = (D_8009D250 << 6) & 0xFFF;

    memset(&trans, 0, sizeof(trans));
    trans.x = S16_AT(entity, 0x64);
    trans.y = S16_AT(entity, 0x66);
    trans.z = 0;

    prim = STATUS_TAG;
    S16_AT(prim, 8) = U16_AT(entity, 0x64) - 0xC;
    S16_AT(prim, 0xA) = U16_AT(entity, 0x66) - 0xC;

    if ((compact << 24) == 0) {
        red = 0x96;
        size = 0x14;
    } else {
        red = 0x32;
        size = 0xA;
    }

    U8_AT(prim, 4) = red;
    U8_AT(prim, 5) = size;
    U8_AT(prim, 6) = size;
    U8_AT(STATUS_TRI, 4) = red;
    U8_AT(STATUS_TRI, 5) = size;
    U8_AT(STATUS_TRI, 6) = size;

    RotMatrix(&rot, &matrix);
    SetRotMatrix(&matrix);
    TransMatrix(&matrix, &trans);
    SetTransMatrix(&matrix);

    RotTrans(&verts[0], &out0, &flag);
    RotTrans(&verts[1], &out1, &flag);
    RotTrans(&verts[2], &out2, &flag);

    S16_AT(STATUS_TRI, 8) = out0.x;
    S16_AT(STATUS_TRI, 0xA) = out0.y;
    S16_AT(STATUS_TRI, 0xC) = out1.x;
    S16_AT(STATUS_TRI, 0xE) = out1.y;
    S16_AT(STATUS_TRI, 0x10) = out2.x;
    S16_AT(STATUS_TRI, 0x12) = out2.y;
    AddPrim(ACTIVE_OT + 0x14, STATUS_TRI);

    x = S16_AT(entity, 0x64);
    if (x + 0x86 >= 0x12D) {
        x -= 8;
        S16_AT(STATUS_QUAD_B, 0x18) = x;
        *(s16 *)((u8 *)&D_8009E4A0_o + (ACTIVE_INDEX << 5)) = x;
        x = U16_AT(entity, 0x64) - 0x23;
        S16_AT(STATUS_QUAD_B, 0x1C) = x;
        *(s16 *)((u8 *)&D_8009E4A4_o + (ACTIVE_INDEX << 5)) = x;
        S16_AT(STATUS_POS_BASE, 0) = U16_AT(entity, 0x64) - 0x73;
    } else {
        x += 8;
        S16_AT(STATUS_QUAD_B, 0x18) = x;
        *(s16 *)((u8 *)&D_8009E4A0_o + (ACTIVE_INDEX << 5)) = x;
        x = U16_AT(entity, 0x64) + 0x23;
        S16_AT(STATUS_POS_BASE, 0) = x;
        S16_AT(STATUS_QUAD_B, 0x1C) = x;
        *(s16 *)((u8 *)&D_8009E4A4_o + (ACTIVE_INDEX << 5)) = x;
    }

    y = S16_AT(entity, 0x66);
    if (y - 0x52 < 0) {
        *(s16 *)((u8 *)&D_8009E4A2_o + (ACTIVE_INDEX << 5)) = y + 8;
        *(s16 *)((u8 *)&D_8009E4B2_o + (ACTIVE_INDEX << 5)) = U16_AT(entity, 0x66) + 9;
        y = U16_AT(entity, 0x66) + 0x23;
        S16_AT(STATUS_POS_BASE, 2) = y;
        *(s16 *)((u8 *)&D_8009E4A6_o + (ACTIVE_INDEX << 5)) = y;
        *(s16 *)((u8 *)&D_8009E4B6_o + (ACTIVE_INDEX << 5)) = U16_AT(entity, 0x66) + 0x24;
    } else {
        *(s16 *)((u8 *)&D_8009E4A2_o + (ACTIVE_INDEX << 5)) = y - 8;
        *(s16 *)((u8 *)&D_8009E4B2_o + (ACTIVE_INDEX << 5)) = U16_AT(entity, 0x66) - 7;
        y = U16_AT(entity, 0x66) - 0x23;
        S16_AT(STATUS_POS_BASE, 2) = y;
        *(s16 *)((u8 *)&D_8009E4A6_o + (ACTIVE_INDEX << 5)) = y;
        *(s16 *)((u8 *)&D_8009E4B6_o + (ACTIVE_INDEX << 5)) = U16_AT(entity, 0x66) - 0x22;
    }

    AddPrim(ACTIVE_OT + 0x1C, STATUS_QUAD_A);
    AddPrim(ACTIVE_OT + 0x18, STATUS_QUAD_B);
    AddPrim(ACTIVE_OT + 0x14, STATUS_BASE);
}

/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed short s16;
typedef unsigned short u16;
typedef int s32;

void Entity_SlideOnWall();
s16 Geo_FindNearestEdge(s16, s16, s32, u16);

extern u16 g_EntityCollisionMoveDelta;
extern u16 g_EntityCollisionWallParam;
extern s32 g_EntityCollisionWallSlot;
extern struct { char _[16]; } g_PlayerEntity_0_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_0 (*(void **)&g_PlayerEntity_0_o)
extern struct { char _[16]; } g_PlayerEntity_1_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_1 (*(void **)&g_PlayerEntity_1_o)
extern struct { char _[16]; } g_PlayerEntity_2_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_2 (*(void **)&g_PlayerEntity_2_o)
extern struct { char _[16]; } g_PlayerEntity_3_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_3 (*(void **)&g_PlayerEntity_3_o)

void Entity_ApplyCollisionResponse(void) {
    char *obj;
    s32 saved_42;
    s32 saved_4a;
    s32 product;
    u16 value_224;
    register char *obj2 asm("$2");
    register s32 arg0 asm("$4");
    register s32 arg1 asm("$5");
    register s32 arg2 asm("$6");
    register u16 arg3 asm("$7");
    s16 hit;
    s32 copy_40;
    s32 copy_44;
    s32 copy_48;

    obj = g_PlayerEntity_0;
    value_224 = *(u16 *)(obj + 0x224);
    product = value_224;
    product *= *(u16 *)(obj + 0x26);
    g_EntityCollisionMoveDelta = value_224;
    if (product < 0) {
        product += 0xFFF;
    }

    arg0 = *(s16 *)(obj + 0x2A);
    arg1 = *(s16 *)(obj + 0x32);
    arg2 = g_EntityCollisionWallSlot;
    arg3 = g_EntityCollisionWallParam;
    saved_42 = *(s16 *)(obj + 0x42);
    saved_4a = *(s16 *)(obj + 0x4A);
    g_EntityCollisionMoveDelta = (u16)(product >> 12);
    hit = Geo_FindNearestEdge(arg0, arg1, arg2, arg3);
    if (hit < 0) {
        return;
    }

    Entity_SlideOnWall(g_PlayerEntity_1, g_EntityCollisionWallSlot, g_EntityCollisionWallParam, hit, saved_42, saved_4a);
    arg2 = g_EntityCollisionWallSlot;
    obj2 = g_PlayerEntity_2;
    arg3 = g_EntityCollisionWallParam;
    if (Geo_FindNearestEdge(
            *(s16 *)(obj2 + 0x2A),
            *(s16 *)(obj2 + 0x32),
            arg2,
            arg3) >= 0) {
        obj2 = g_PlayerEntity_3;
        copy_40 = *(s32 *)(obj2 + 0x40);
        copy_44 = *(s32 *)(obj2 + 0x44);
        copy_48 = *(s32 *)(obj2 + 0x48);
        *(s32 *)(obj2 + 0x28) = copy_40;
        *(s32 *)(obj2 + 0x2C) = copy_44;
        *(s32 *)(obj2 + 0x30) = copy_48;
    }
}

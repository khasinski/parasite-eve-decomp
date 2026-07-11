typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u8 *D_8009D254;
extern u32 D_8009D2E8;
extern u32 D_8009DFB0[];
extern u16 D_8009D25C;
extern u16 D_8009D260;
extern u16 D_8009D264;
extern u16 D_8009D268;
extern u16 D_8009D26C;
extern u8 *D_8009D248[];
extern u8 *D_8009D618;
extern u8 *D_8009D63C;

u8 *Geo_ClipToFloorBoundary(s16 x, s16 z, u8 *floor);
u8 *Entity_SlideOnRamp(u8 *entity);
int Geo_PointInTri(u8 *floor, s16 x, s16 z);
u8 *Geo_ClipToFloorBoundarySub(u8 *floor, int mode, s16 x, s16 z,
                               s16 old_x, s16 old_z);
int Math_FixedMul(int a, int b);

static int Entity_UpdateAndRenderNclip(u32 xy0, u32 xy1, u32 xy2) {
    int result;

    asm volatile("mtc2 %1,$12\n\t"
                 "mtc2 %2,$13\n\t"
                 "mtc2 %3,$14\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4B400006\n\t"
                 "swc2 $24,0(%0)"
                 :
                 : "r"(&result), "r"(xy0), "r"(xy1), "r"(xy2)
                 : "memory");
    return result;
}

static int Entity_UpdateAndRenderMoved(u8 *entity) {
    return S16_AT(entity, 0x42) != S16_AT(entity, 0x2A) ||
           S16_AT(entity, 0x4A) != S16_AT(entity, 0x32) ||
           S32_AT(entity, 0x44) != S32_AT(entity, 0x2C);
}

static void Entity_UpdateAndRenderClearFloorBits(void) {
    u8 *db;
    int count;
    int i;

    db = D_8009D63C;
    if (db == 0) {
        return;
    }

    count = U16_AT(db, 8);
    for (i = 0; i < count; i++) {
        D_8009DFB0[i] = 0;
    }
}

static int Entity_UpdateAndRenderPlayerStillVisible(s16 x, s16 z,
                                                    s16 old_x, s16 old_z,
                                                    s16 radius) {
    int n0;
    int n1;
    int dx;
    int dz;
    int side_w;
    int side_h;

    if (!(D_8009D2E8 & 8)) {
        return 1;
    }

    n0 = Entity_UpdateAndRenderNclip(*(u32 *)&D_8009D25C,
                                     ((u16)z << 16) | (u16)x,
                                     *(u32 *)&D_8009D264);
    n1 = Entity_UpdateAndRenderNclip(*(u32 *)&D_8009D25C,
                                     ((u16)old_z << 16) | (u16)old_x,
                                     *(u32 *)&D_8009D264);
    if ((n0 ^ n1) < 0) {
        return 1;
    }

    if (n0 < 0) {
        n0 = -n0;
    }

    if (n1 < 0) {
        if (-n1 < n0) {
            return 0;
        }
    } else if (n1 < n0) {
        return 1;
    }

    dx = x - radius;
    dz = z - radius;
    side_w = (s16)(D_8009D25C - D_8009D260);
    side_h = (s16)(D_8009D264 - D_8009D268);

    if ((s16)D_8009D25C < dx) {
        if (side_h < side_w) {
            return 0;
        }
        if ((s16)D_8009D264 < z || z + radius < (s16)D_8009D268) {
            return 0;
        }
    }

    if (x + radius < (s16)D_8009D260) {
        if (side_h < side_w) {
            return 0;
        }
        if ((s16)D_8009D264 < z - radius || z + radius < (s16)D_8009D268) {
            return 0;
        }
    }

    if ((s16)D_8009D264 < z - radius) {
        if (side_w < side_h) {
            return 0;
        }
        if ((s16)D_8009D25C < x - radius || x + radius < (s16)D_8009D260) {
            return 0;
        }
    }

    if (z + radius < (s16)D_8009D268) {
        if (side_w < side_h) {
            return 0;
        }
        if ((s16)D_8009D25C < x - radius || x + radius < (s16)D_8009D260) {
            return 0;
        }
    }

    return 1;
}

static u8 *Entity_UpdateAndRenderFindFloor(u8 *entity, u8 *floor,
                                           s16 x, s16 z,
                                           s16 old_x, s16 old_z) {
    u8 *next;
    u16 old_slot;

    Entity_UpdateAndRenderClearFloorBits();
    next = Geo_ClipToFloorBoundary(x, z, floor);
    if (next != 0) {
        return next;
    }

    if (entity != D_8009D254) {
        return 0;
    }

    Entity_SlideOnRamp(entity);
    x = S16_AT(entity, 0x2A);
    z = S16_AT(entity, 0x32);

    Entity_UpdateAndRenderClearFloorBits();
    old_slot = D_8009D25C;
    next = Geo_ClipToFloorBoundary(x, z, floor);
    if (next != 0) {
        return next;
    }

    if (old_slot == D_8009D25C) {
        Entity_UpdateAndRenderClearFloorBits();
        D_8009DFB0[D_8009D25C >> 5] = 1 << (D_8009D25C & 0x1F);
        next = Geo_ClipToFloorBoundary(x, z, floor);
        if (next != 0) {
            return 0;
        }
    }

    Entity_SlideOnRamp(entity);
    x = S16_AT(entity, 0x2A);
    z = S16_AT(entity, 0x32);
    Entity_UpdateAndRenderClearFloorBits();
    return Geo_ClipToFloorBoundary(x, z, floor);
}

static void Entity_UpdateAndRenderApplyFloorTable(u8 *entity, u8 *floor) {
    u8 *entry;
    int index;
    int a;
    int b;
    int y;
    u32 flags;

    if (D_8009D618 != 0) {
        index = U16_AT(floor, 2);
        entry = D_8009D618 + index * 0x0C;
        a = Math_FixedMul(S32_AT(entry, 0), S32_AT(entity, 0x28));
        b = Math_FixedMul(S32_AT(entry, 8), S32_AT(entity, 0x30));
        y = Math_FixedMul(S32_AT(floor, 4) - a - b, S32_AT(entry, 4));

        if (U32_AT(entity, 0x98) & 2) {
            if (S32_AT(entity, 0x2C) < y) {
                S32_AT(entity, 0x2C) = S32_AT(entity, 0x44);
                return;
            }
            U32_AT(entity, 0x98) &= ~2U;
            S32_AT(entity, 0x6C) = 0;
        }
        S32_AT(entity, 0x2C) = y;
        return;
    }

    entry = D_8009D248[U8_AT(floor, 1)];
    flags = U32_AT(entity, 0x98);
    y = S16_AT(entry, 0);

    if (flags & 2) {
        if (S32_AT(entity, 0x2C) < y) {
            return;
        }
        if (S32_AT(entity, 0x44) < y) {
            U32_AT(entity, 0x98) = flags & ~2U;
            S32_AT(entity, 0x2C) = y;
            S32_AT(entity, 0x6C) = 0;
        }
    } else if (U8_AT(PTR_AT(entity, 0x1A4), 1) != U8_AT(floor, 1)) {
        if (((y - S16_AT(entity, 0x2E)) < 0 ? -(y - S16_AT(entity, 0x2E)) :
                                             (y - S16_AT(entity, 0x2E))) <
            ((s32)U16_AT(entity, 0x10) << 16)) {
            S32_AT(entity, 0x2C) = y << 16;
        }
    }
}

void Entity_UpdateAndRender(u8 *entity) {
    s16 x;
    s16 z;
    s16 old_x;
    s16 old_z;
    s16 radius;
    u8 *floor;
    u8 *next_floor;

    x = S16_AT(entity, 0x2A);
    z = S16_AT(entity, 0x32);
    U32_AT(entity, 0x98) &= ~0x80000U;

    old_x = S16_AT(entity, 0x42);
    old_z = S16_AT(entity, 0x4A);
    if (!Entity_UpdateAndRenderMoved(entity)) {
        if (entity == D_8009D254) {
            D_8009D2E8 &= ~8U;
        }
        return;
    }

    radius = (U16_AT(entity, 0x224) * U16_AT(entity, 0x26)) >> 12;
    floor = PTR_AT(entity, 0x1A4);
    PTR_AT(entity, 0x1A8) = floor;
    D_8009D26C = radius;

    if (entity == D_8009D254 &&
        !Entity_UpdateAndRenderPlayerStillVisible(x, z, old_x, old_z, radius)) {
        D_8009D2E8 &= ~8U;
    }

    next_floor = Entity_UpdateAndRenderFindFloor(entity, floor, x, z, old_x, old_z);
    if (next_floor == 0) {
        if (entity == D_8009D254) {
            S32_AT(entity, 0x28) = S32_AT(entity, 0x40);
            S32_AT(entity, 0x2C) = S32_AT(entity, 0x44);
            S32_AT(entity, 0x30) = S32_AT(entity, 0x48);
            U32_AT(entity, 0x98) |= 0x80000;
        }
        return;
    }

    if (Geo_PointInTri(next_floor, x, z) == 0) {
        next_floor = Geo_ClipToFloorBoundarySub(next_floor, 0, x, z, old_x, old_z);
    }

    if (next_floor != 0) {
        Entity_UpdateAndRenderApplyFloorTable(entity, next_floor);
        PTR_AT(entity, 0x1A4) = next_floor;
    }
}

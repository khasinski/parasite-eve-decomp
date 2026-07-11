typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

extern u8 *D_8009D20C;
extern u8 *D_8009D254;
extern u32 D_8009D1A0;

void *Task_AllocNode(void *pool, void *parent);
void func_80035F54(void *entity);

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

typedef void (*ScenePairCallback)(void *self, int self_part, void *other, int other_part);

static int Scene_ScaledS16(void *entity, int offset) {
    int value;

    value = S16_AT(entity, offset) * U16_AT(entity, 0x26);
    if (value < 0) {
        value += 0xFFF;
    }
    return value >> 12;
}

static int Scene_ScaleValue(int value, int scale) {
    value *= scale;
    if (value < 0) {
        value += 0xFFF;
    }
    return value >> 12;
}

static int Scene_DistanceSq3(int dx, int dy, int dz) {
    return dx * dx + dy * dy + dz * dz;
}

static int Scene_EntityMayCollide(u8 *a, u8 *b) {
    u32 flags_a;
    u32 flags_b;

    if (PTR_AT(a, 0x18C) == b || PTR_AT(b, 0x18C) == a) {
        return 0;
    }

    flags_a = U32_AT(a, 0x98);
    flags_b = U32_AT(b, 0x98);
    if ((flags_a & 0x20000) && b != D_8009D254) {
        return 0;
    }
    if ((flags_b & 0x20000) && a != D_8009D254) {
        return 0;
    }
    if (flags_b & 0x20) {
        return 0;
    }
    return 1;
}

static int Scene_NodeForEntityExists(u8 *entity, u8 *other) {
    u8 *node;
    u16 id;

    id = U16_AT(other, 0x24);
    node = PTR_AT(entity, 0xA4);
    while (node != 0) {
        if ((U16_AT(node, 8) & 1) && (U16_AT(node, 8) & 0x10) == 0 && U32_AT(node, 0x0C) == id) {
            return 1;
        }
        node = PTR_AT(node, 0x24);
    }
    return 0;
}

static void Scene_AddContactNode(u8 *entity, u8 *other) {
    u8 *node;
    u8 *head;

    if (PTR_AT(entity, 0x1A0) == 0 || Scene_NodeForEntityExists(entity, other)) {
        return;
    }

    node = Task_AllocNode(PTR_AT(entity, 0x1A0), 0);
    U16_AT(node, 8) |= 1;
    U32_AT(node, 0x0C) = U16_AT(other, 0x24);
    U32_AT(node, 0x18) = U8_AT(other, 0x0C);
    U32_AT(node, 0x1C) = U8_AT(other, 0x0D);

    head = PTR_AT(entity, 0xA4);
    if (head != 0) {
        PTR_AT(node, 0x24) = head;
        PTR_AT(head, 0x28) = node;
    }
    PTR_AT(entity, 0xA4) = node;
}

static int Scene_BaseCollisionHit(u8 *a, u8 *b, int a_side, int a_top) {
    int b_top;
    int dx;
    int dy;
    int dz;
    int radius;

    b_top = Scene_ScaledS16(b, 0x230);
    dx = S16_AT(a + 0x228, 0) - S16_AT(b, 0x228);
    dy = S16_AT(a + 0x228, 2) - S16_AT(b, 0x22A);
    dz = S16_AT(a + 0x228, 4) - S16_AT(b, 0x22C);
    radius = a_top + b_top;
    return radius * radius >= Scene_DistanceSq3(dx, dy, dz);
}

static int Scene_InnerCollisionHit(u8 *a, u8 *b, int a_side) {
    int b_side;
    int dx;
    int dz;
    int radius;

    if (S16_AT(a, 0x22C) == 0 || S16_AT(b, 0x224) == 0) {
        return 0;
    }

    b_side = Scene_ScaledS16(b, 0x224);
    dx = S16_AT(a, 0x21C) - S16_AT(b, 0x21C);
    dz = S16_AT(a, 0x220) - S16_AT(b, 0x220);
    radius = a_side + b_side;
    return radius * radius >= dx * dx + dz * dz;
}

static void Scene_UpdateFacingOnContact(u8 *a, u8 *b) {
    int ax;
    int az;
    int dx;
    int dz;

    ax = (S32_AT(a, 0x28) - S32_AT(a, 0x40)) >> 16;
    az = (S32_AT(a, 0x30) - S32_AT(a, 0x48)) >> 16;
    dx = S16_AT(a, 0x228) - S16_AT(b, 0x21C);
    dz = S16_AT(a, 0x22C) - S16_AT(b, 0x220);
    if (ax * dx + az * dz < 0) {
        func_80035F54(a);
    }

    ax = (S32_AT(b, 0x28) - S32_AT(b, 0x40)) >> 16;
    az = (S32_AT(b, 0x30) - S32_AT(b, 0x48)) >> 16;
    dx = S16_AT(a, 0x228) - S16_AT(b, 0x21C);
    dz = S16_AT(a, 0x22C) - S16_AT(b, 0x220);
    if (ax * dx + az * dz > 0) {
        func_80035F54(b);
    }

    if (a != D_8009D254 && (U32_AT(a, 0x98) & 0x01000000) == 0) {
        U16_AT(a, 0x3A) = U16_AT(a, 0x52);
    }
    if (b != D_8009D254 && (U32_AT(b, 0x98) & 0x01000000) == 0) {
        U16_AT(b, 0x3A) = U16_AT(b, 0x52);
    }
}

static void Scene_MarkPlayerContact(u8 *a, u8 *b) {
    if (a == D_8009D254) {
        U32_AT(b, 0x98) |= 0x02000000;
    } else if (b == D_8009D254) {
        U32_AT(a, 0x98) |= 0x02000000;
    }
}

static void Scene_InvokePartCallbacks(u8 *a, u8 *b) {
    ScenePairCallback cb;
    u8 *a_shape;
    u8 *b_shape;
    u8 *a_part;
    u8 *b_part;
    int a_count;
    int b_count;
    int ai;
    int bi;
    int a_height;
    int b_height;
    int dx;
    int dy;
    int dz;
    int radius;

    a_shape = PTR_AT(a, 0x1B4);
    if (a_shape == 0 || U8_AT(a_shape, 3) == 0) {
        return;
    }
    b_shape = PTR_AT(b, 0x1B4);
    if (b_shape == 0 || U8_AT(b_shape, 3) == 0) {
        return;
    }

    a_part = PTR_AT(a, 0x234);
    a_count = U8_AT(a_shape, 3);
    for (ai = 0; ai < a_count; ai++, a_part += 0x0C) {
        a_height = Scene_ScaleValue(S16_AT(a_part, 8), U16_AT(a, 0x26));
        b_part = PTR_AT(b, 0x234);
        b_count = U8_AT(b_shape, 3);
        for (bi = 0; bi < b_count; bi++, b_part += 0x0C) {
            b_height = Scene_ScaleValue(S16_AT(b_part, 8), U16_AT(b, 0x26));
            dx = S16_AT(a_part, 0) - S16_AT(b_part, 0);
            dy = S16_AT(a_part, 2) - S16_AT(b_part, 2);
            dz = S16_AT(a_part, 4) - S16_AT(b_part, 4);
            radius = a_height + b_height;
            if (radius * radius >= Scene_DistanceSq3(dx, dy, dz)) {
                cb = (ScenePairCallback)PTR_AT(a, 0x194);
                if (cb != 0) {
                    cb(a, S16_AT(a_part, 6), b, S16_AT(b_part, 6));
                }
                cb = (ScenePairCallback)PTR_AT(b, 0x194);
                if (cb != 0) {
                    cb(b, S16_AT(b_part, 6), a, S16_AT(a_part, 6));
                }
            }
        }
    }
}

static void Scene_HandlePair(u8 *a, u8 *b, int a_side, int a_top) {
    if (!Scene_EntityMayCollide(a, b)) {
        return;
    }

    if (!Scene_BaseCollisionHit(a, b, a_side, a_top)) {
        return;
    }

    if (PTR_AT(a, 0x1AC) == 0 || PTR_AT(b, 0x1AC) == 0) {
        Scene_AddContactNode(a, b);
        Scene_AddContactNode(b, a);
        return;
    }

    if (!Scene_InnerCollisionHit(a, b, a_side)) {
        if ((D_8009D1A0 & 2) == 0) {
            Scene_AddContactNode(a, b);
            Scene_AddContactNode(b, a);
        }
        return;
    }

    Scene_UpdateFacingOnContact(a, b);
    Scene_MarkPlayerContact(a, b);
    Scene_AddContactNode(a, b);
    Scene_AddContactNode(b, a);

    if (D_8009D1A0 & 2) {
        Scene_InvokePartCallbacks(a, b);
        return;
    }

    if (U8_AT(a, 0x0C) != 0 && U8_AT(b, 0x0C) != 0) {
        U32_AT(a, 0x98) |= 0x40000;
        U32_AT(b, 0x98) |= 0x40000;
    } else {
        Scene_InvokePartCallbacks(a, b);
    }
}

void Scene_UpdateEntityPositions(void) {
    u8 *entity;
    u8 *other;
    int side;
    int top;

    entity = D_8009D20C;
    while (entity != 0) {
        U32_AT(entity, 0x98) &= 0xFDFBFFFF;
        entity = PTR_AT(entity, 4);
    }

    entity = D_8009D20C;
    while (entity != 0) {
        if ((U32_AT(entity, 0x98) & 0x20) == 0) {
            side = Scene_ScaledS16(entity, 0x224);
            top = Scene_ScaledS16(entity, 0x230);
            other = PTR_AT(entity, 4);
            while (other != 0) {
                Scene_HandlePair(entity, other, side, top);
                other = PTR_AT(other, 4);
            }
        }
        entity = PTR_AT(entity, 4);
    }
}

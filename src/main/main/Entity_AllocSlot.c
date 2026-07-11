/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned int u32;

#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))

extern struct { char _[16]; } D_800109B0_o __asm__("D_800109B0");
extern struct { char _[16]; } D_800A5D58_o __asm__("D_800A5D58");
extern struct { char _[16]; } D_800A5D5C_o __asm__("D_800A5D5C");
extern struct { char _[16]; } D_8009D2EC_o __asm__("D_8009D2EC");

#define ENTITY_SLOT_TEMPLATE ((u32 *)&D_800109B0_o)
#define ENTITY_SLOT_FLAGS ((u32 *)&D_800A5D58_o)
#define ENTITY_SLOT_BODIES ((u8 *)&D_800A5D5C_o)
#define D_8009D2EC (*(u8 *)&D_8009D2EC_o)

extern u8 D_8009D2A0;

void Entity_SetActionMode(void *entity, int mode);

void Entity_AllocSlot(u8 *entity) {
    u32 scratch[0x36];
    u8 index;
    u8 next_id;
    u8 *slot;
    u32 *src;
    u32 *dst;
    u32 *end;

    src = ENTITY_SLOT_TEMPLATE;
    dst = scratch;
    end = src + 0x34;
    do {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        src += 4;
        dst += 4;
    } while (src != end);
    dst[0] = src[0];
    dst[1] = src[1];

    index = 0;
    while (index < 7) {
        if (ENTITY_SLOT_FLAGS[index * 0x37] == 0) {
            ENTITY_SLOT_FLAGS[index * 0x37] = 1;

            slot = ENTITY_SLOT_BODIES + index * 0xDC;

            src = scratch;
            dst = (u32 *)slot;
            end = (u32 *)((u8 *)scratch + 0xD0);
            do {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
                src += 4;
                dst += 4;
            } while (src != end);
            dst[0] = src[0];
            dst[1] = src[1];

            *(u8 **)entity = slot;

            next_id = D_8009D2EC + 1;
            D_8009D2EC = next_id;
            slot[7] = next_id;
            U32_AT(slot, 8) = 1 << index;

            if ((U32_AT(entity, 0x98) & 0x2000) == 0) {
                U32_AT(slot, 0x18) = (u32)(slot + 0x1C);
                Entity_SetActionMode(entity, 2);
                D_8009D2A0++;
            }
            return;
        }

        index++;
    }
}

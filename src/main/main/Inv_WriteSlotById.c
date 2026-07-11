/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

typedef struct InvCategoryPool {
    u8 pad_00[9];
    u8 base_count;
    u16 count;
    u8 pad_0C[6];
    s16 extra_capacity;
    u8 pad_14[12];
} InvCategoryPool;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern InvCategoryPool D_800A1E64[];

int Inv_WriteSlotById(u8 *item_data) {
    int result;
    int pool_index;
    int type;
    int slot;
    int item_id;
    int cap;
    int count;
    s16 *cur;
    s16 *end;
    InvCategoryPool *pool;

    result = 0;
    type = item_data[6];
    if (type < 0x10) {
        if (type != 0 && type < 8) {
            pool_index = type - 4;
            if (pool_index <= 0) {
                pool_index = 0;
            } else {
                pool_index = type - 5;
            }
        } else if (type < 0x13) {
            pool_index = -1;
        } else {
            pool_index = type - 0x13;
        }
    } else {
        pool_index = type - 0x10;
    }

    if ((unsigned int)pool_index >= 3) {
        return 1;
    }

    item_id = pool_index + 0x200;
    cur = g_InvItemPtr;
    end = cur + g_InvSlotLimit;
    while (cur < end) {
        if (*cur == item_id) {
            break;
        }
        cur++;
    }

    if (cur < g_InvItemPtr + g_InvSlotLimit) {
        slot = cur - g_InvItemPtr;
    } else {
        slot = -1;
    }

    if (slot < 0) {
        cur = g_InvItemPtr;
        end = cur + g_InvSlotLimit;
        while (cur < end) {
            if (*cur == 0) {
                break;
            }
            cur++;
        }

        if (cur < g_InvItemPtr + g_InvSlotLimit) {
            slot = cur - g_InvItemPtr;
        } else {
            slot = -1;
        }

        if (slot >= 0) {
            g_InvItemPtr[slot] = item_id;
        } else {
            result = 1;
        }
    }

    pool = &D_800A1E64[pool_index];
    pool->count += *(u16 *)(item_data + 0xA);
    count = pool->count;
    cap = pool->base_count + pool->extra_capacity;
    if (cap < 1000) {
        if (cap >= count) {
            return result;
        }
    } else if (count < 1000) {
        return result;
    }

    count = pool->base_count + pool->extra_capacity;
    if (count >= 1000) {
        count = 999;
    }
    pool->count = count;
    return result;
}

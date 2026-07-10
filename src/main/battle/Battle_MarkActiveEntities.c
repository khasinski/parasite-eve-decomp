typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D2B0_o __asm__("D_8009D2B0");
extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[16]; } g_FieldActorListHead_o __asm__("g_FieldActorListHead");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern s8 D_8009CE6C;

#define D_8009D2B0 (*(s8 *)&D_8009D2B0_o)
#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define g_FieldActorListHead (*(u8 **)&g_FieldActorListHead_o)
#define D_8009D254 (*(u8 **)&D_8009D254_o)

#define TARGET_ENTRY(table, index) ((u8 *)((u8 *)(table) + ((s8)(index) * 12)))
#define ENTITY_KIND(entity) (*(s8 *)(*(u8 **)(entity) + 5))
#define ENTITY_NEXT(entity) (*(u8 **)((u8 *)(entity) + 4))
#define ENTITY_LINKED_TARGET(entity) (*(u8 **)((u8 *)(entity) + 0x18C))
#define ENTITY_TARGET_FLAGS(entity) (*(u16 *)((u8 *)(entity) + 0x250))
#define ENTITY_TABLE_ANGLE(entry) (*(s16 *)((u8 *)(entry) + 8))
#define MARK_ENTITY(entity) (ENTITY_TARGET_FLAGS(entity) |= 0x20)

void Battle_MarkActiveEntities(void *target_table, int selected_index) {
    u8 *entry;
    u8 *entity;
    u8 *iter;
    u8 *active_entity;
    s16 center;
    s16 min_angle;
    s16 max_angle;
    int slot;
    int mode;
    int wraps;

    if (D_8009D2B0 == 0) {
        return;
    }

    D_8009CE6C = 0;
    mode = (*(u32 *)(*(u8 **)(D_8009D278 + 0x68) + 0x10) >> 6) & 3;
    if (mode == 0) {
        entry = TARGET_ENTRY(target_table, selected_index);
        entity = *(u8 **)entry;

        if (ENTITY_KIND(entity) == 1) {
            MARK_ENTITY(ENTITY_LINKED_TARGET(entity));
        } else if (ENTITY_KIND(entity) == 4) {
            iter = g_FieldActorListHead;
            if (iter == 0) {
                return;
            }
            active_entity = D_8009D254;
            while (iter != 0) {
                if (iter != active_entity && *(u8 **)iter != 0 && ENTITY_KIND(iter) == 4) {
                    MARK_ENTITY(iter);
                }
                iter = ENTITY_NEXT(iter);
            }
        }
        return;
    }

    if (mode == 2) {
        entry = TARGET_ENTRY(target_table, selected_index);
        center = ENTITY_TABLE_ANGLE(entry);
        if (center < -0x600) {
            max_angle = center + 0x200;
            min_angle = center + 0xE00;
        } else if (center < 0x600) {
            max_angle = center - 0x200;
            min_angle = center + 0x200;
        } else {
            max_angle = center - 0xE00;
            min_angle = center - 0x200;
        }

        if (*(u8 **)target_table == 0) {
            return;
        }

        slot = 0;
        wraps = center <= -0x600 || center >= 0x600;
        while (*(u8 **)TARGET_ENTRY(target_table, slot) != 0) {
            s16 angle = ENTITY_TABLE_ANGLE(TARGET_ENTRY(target_table, slot));
            int outside;

            if (!wraps) {
                outside = angle < max_angle || min_angle < angle;
            } else {
                outside = angle < min_angle && max_angle < angle;
            }

            if (!outside) {
                entity = *(u8 **)TARGET_ENTRY(target_table, slot);
                if (ENTITY_KIND(entity) == 1) {
                    MARK_ENTITY(ENTITY_LINKED_TARGET(entity));
                } else if (ENTITY_KIND(entity) == 4) {
                    iter = g_FieldActorListHead;
                    if (iter == 0) {
                        slot++;
                        continue;
                    }
                    active_entity = D_8009D254;
                    while (iter != 0) {
                        if (iter != active_entity && *(u8 **)iter != 0 && ENTITY_KIND(iter) == 4) {
                            MARK_ENTITY(iter);
                        }
                        iter = ENTITY_NEXT(iter);
                    }
                }
            }
            slot++;
        }
        return;
    }

    if (mode == 1 || mode == 3) {
        if (*(u8 **)target_table == 0) {
            return;
        }

        slot = 0;
        active_entity = D_8009D254;
        while (*(u8 **)TARGET_ENTRY(target_table, slot) != 0) {
            entity = *(u8 **)TARGET_ENTRY(target_table, slot);
            if (ENTITY_KIND(entity) == 1) {
                MARK_ENTITY(ENTITY_LINKED_TARGET(entity));
            } else if (ENTITY_KIND(entity) == 4) {
                iter = g_FieldActorListHead;
                if (iter == 0) {
                    slot++;
                    continue;
                }
                while (iter != 0) {
                    if (iter != active_entity && *(u8 **)iter != 0 && ENTITY_KIND(iter) == 4) {
                        MARK_ENTITY(iter);
                    }
                    iter = ENTITY_NEXT(iter);
                }
            }
            slot++;
        }
    }
}

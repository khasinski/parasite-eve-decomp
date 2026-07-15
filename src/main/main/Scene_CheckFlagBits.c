typedef unsigned int u32;

typedef struct SceneFlagRecord {
    u32 flags;
    u32 mask_all;
    u32 mask_any;
    void (*callback)(void *, void *);
} SceneFlagRecord;

extern u32 D_8009D26C;
extern u32 D_8009D1F4;
extern u32 D_8009D1E4;

void Scene_CheckFlagBits(void *ctx, SceneFlagRecord **table, int *index_ptr) {
    SceneFlagRecord *record;
    SceneFlagRecord *base;
    u32 flags;
    int triggered;

    base = table[*index_ptr];
    triggered = 0;
    if (base != 0) {
        flags = base->flags;
    } else {
        flags = 0;
    }

    if (flags != 0) {
        record = base;
        do {
            void (*callback)(void *, void *);
            u32 mask;
            u32 value;

            callback = record->callback;
            if (callback == 0) {
                break;
            }

            if (flags & 0x20) {
                callback(ctx, index_ptr);
                triggered = 1;
            } else if (flags & 1) {
                value = D_8009D26C;
                mask = record->mask_all;
                if ((value & mask) == mask) {
                    if (flags & 4) {
                        if (D_8009D1F4 & record->mask_any) {
                            callback(ctx, index_ptr);
                            triggered = 1;
                        }
                    } else if (flags & 8) {
                        if (D_8009D1E4 & record->mask_any) {
                            callback(ctx, index_ptr);
                            triggered = 1;
                        }
                    } else if (flags & 2) {
                        if (value & record->mask_any) {
                            callback(ctx, index_ptr);
                            triggered = 1;
                        }
                    } else {
                        callback(ctx, index_ptr);
                        triggered = 1;
                    }
                }
            } else if (flags & 2) {
                mask = record->mask_any;
                if (D_8009D26C & mask) {
                    if (flags & 4) {
                        if (D_8009D1F4 & mask) {
                            callback(ctx, index_ptr);
                            triggered = 1;
                        }
                    } else if (flags & 8) {
                        if (D_8009D1E4 & mask) {
                            callback(ctx, index_ptr);
                            triggered = 1;
                        }
                    } else {
                        callback(ctx, index_ptr);
                        triggered = 1;
                    }
                }
            } else if (flags & 4) {
                if (D_8009D1F4 & record->mask_any) {
                    callback(ctx, index_ptr);
                    triggered = 1;
                }
            } else if (flags & 8) {
                if (D_8009D1E4 & record->mask_any) {
                    callback(ctx, index_ptr);
                    triggered = 1;
                }
            }

            if ((flags & 0x10) == 0 && triggered != 0) {
                break;
            }

            record++;
            flags = record->flags;
        } while (flags != 0);
    }

    if (base != 0 && base->callback != 0) {
        base->callback(ctx, index_ptr);
    }
}

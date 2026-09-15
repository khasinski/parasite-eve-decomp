#include "pe1/field_anim.h"

int func_800D401C(int id)
{
    /* Reused for the callback's additional payload size after the slot scan. */
    int result;
    FieldAnimTaskContext *ctx = D_800E2368;
    FieldAnimTaskSlot *slot = ctx->slots;
    int size;
    char *payload;
    for (result = 0; result < 8; result++, slot++) {
        if (slot->id == 0xFFFF)
            break;
    }
    if (result == 8)
        return -1;
    ctx->count++;
    slot->id = id;
    slot->age = 0;
    size = ctx->table->sizes[id];
    payload = ctx->cursor;
    ctx->used += size;
    if (ctx->used > 0x97C) {
        payload = ctx->arena;
        ctx->used = size;
        ctx->cursor = payload;
    }
    slot->end = payload + size;
    slot->start = payload;
    D_800F33E0 = slot;
    result = ctx->table->callbacks[slot->id](0, payload, ctx->argument);
    if (!result)
        slot->end = 0;
    ctx->cursor += size + result;
    ctx->used += result;
    return size + result;
}

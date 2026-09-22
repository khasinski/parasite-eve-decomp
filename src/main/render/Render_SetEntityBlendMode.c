#include "pe1/render_object.h"
#include "pe1/render_prim.h"

static inline int primitiveBlendMode(int kind, int forcedKind, s16 mode) {
    if (kind == forcedKind) {
        return 1;
    }
    return mode;
}

static inline void setPrimitiveBlend(u8 *command, int enabled) {
    if (enabled) {
        *command |= 2;
    } else {
        *command &= ~2;
    }
}

void Render_SetEntityBlendMode(RenderObjectEntity *object, int mode) {
    int slot = D_8009CDDC;
    s16 savedMode = mode;
    int i;
    u8 *prim;
    RenderPrimitiveDescriptor *descriptor;

    if (!object->header || !object->draw_count) {
        return;
    }
    descriptor = object->primitive_descriptors;
    prim = object->primitive_buffer;
    for (i = 0; i < object->header->packet34_count;
         prim += 2 * sizeof(RenderPacket34), ++i, ++descriptor) {
        RenderPacket34 *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 11, savedMode);
        packet = (RenderPacket34 *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
    for (i = 0; i < object->header->packet28_count;
         prim += 2 * sizeof(RenderPacket28), ++i, ++descriptor) {
        RenderPacket28 *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 16, savedMode);
        packet = (RenderPacket28 *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
    for (i = 0; i < object->header->packet24_count;
         prim += 2 * sizeof(RenderPacket24), ++i, ++descriptor) {
        RenderPacket24 *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 21, savedMode);
        packet = (RenderPacket24 *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
    for (i = 0; i < object->header->packet1c_count;
         prim += 2 * sizeof(RenderPacket1C), ++i, ++descriptor) {
        RenderPacket1C *packet;
        int enabled = primitiveBlendMode(descriptor->kind, 26, savedMode);
        packet = (RenderPacket1C *)prim + slot;
        setPrimitiveBlend(&packet->values0.bytes.command, enabled);
    }
}

#include "pe1/render_object.h"
#include "pe1/render_prim.h"

void Render_FadeEntityColor(RenderObjectEntity *object, int r, int g, int b) {
    u8 *prim;
    u32 color;
    int i;
    int slot = D_8009CDDC;
    u8 opcode;

    if (!object->header || !object->draw_count) {
        return;
    }
    color = ((b & 255) << 16) | ((g & 255) << 8) | (r & 255);
    object->primitive_red = r;
    object->primitive_green = g;
    object->primitive_blue = b;
    prim = object->primitive_buffer;
    for (i = -1; ++i < object->header->packet34_count;
         prim += 2 * sizeof(RenderPacket34)) {
        RenderPacket34 *packet = (RenderPacket34 *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->value3 = color;
        packet->values0.bytes.command = opcode;
    }
    for (i = -1; ++i < object->header->packet28_count;
         prim += 2 * sizeof(RenderPacket28)) {
        RenderPacket28 *packet = (RenderPacket28 *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->values0.bytes.command = opcode;
    }
    for (i = -1; ++i < object->header->packet24_count;
         prim += 2 * sizeof(RenderPacket24)) {
        RenderPacket24 *packet = (RenderPacket24 *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->value3 = color;
        packet->values0.bytes.command = opcode;
    }
    for (i = -1; ++i < object->header->packet1c_count;
         prim += 2 * sizeof(RenderPacket1C)) {
        RenderPacket1C *packet = (RenderPacket1C *)prim + slot;
        opcode = packet->values0.bytes.command;
        packet->values0.value = color;
        packet->value1 = color;
        packet->value2 = color;
        packet->values0.bytes.command = opcode;
    }
}

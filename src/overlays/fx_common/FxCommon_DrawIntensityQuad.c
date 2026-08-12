#include "fx_common.h"

void func_801941A4(int intensity)
{
    FxCommonPacketCursor *packet;
    FxCommonBuffer *quadBuffer;
    FxCommonBuffer *modeBuffer;
    FxCommonAddress packetAddress;
    u32 addressMask = 0xFFFFFF;
    u32 commandMask;
    u32 tag;

    packet = D_8019C9C0->data;
    func_80077BC4(&packet->quad);
    func_80077B04(&packet->quad, 1);

    packet->quad.color0[0] = intensity;
    packet->quad.color0[1] = intensity;
    packet->quad.color0[2] = intensity;
    packet->quad.color1[0] = intensity;
    packet->quad.color1[1] = intensity;
    packet->quad.color1[2] = intensity;
    packet->quad.color2[0] = intensity;
    packet->quad.color2[1] = intensity;
    packet->quad.color2[2] = intensity;
    packet->quad.color3[0] = intensity;
    packet->quad.color3[1] = intensity;
    packet->quad.color3[2] = intensity;
    packet->quad.x0 = 0;
    packet->quad.y0 = 0;
    packet->quad.x1 = 0x140;
    packet->quad.y1 = 0;
    packet->quad.x2 = 0;
    packet->quad.y2 = 0xF0;
    packet->quad.x3 = 0x140;
    packet->quad.y3 = 0xF0;

    commandMask = 0xFF000000;
    packetAddress.pointer = packet;
    quadBuffer = D_8019C9C0;
    tag = packet->quad.tag;
    tag &= commandMask;
    tag |= *(u32 *)quadBuffer->allocation & addressMask;
    packet->quad.tag = tag;
    *(u32 *)quadBuffer->allocation =
        (*(u32 *)quadBuffer->allocation & commandMask) |
        (packetAddress.word & addressMask);
    packet++;
    quadBuffer->data = packet;

    func_80077C84(&packet->mode.packet, 0, 0, 0x40);
    modeBuffer = D_8019C9C0;
    packet->mode.packet.tag = (packet->mode.packet.tag & commandMask) |
                              (*(u32 *)modeBuffer->allocation & addressMask);
    packetAddress.pointer = packet;
    addressMask = packetAddress.word & addressMask;
    *(u32 *)modeBuffer->allocation =
        (*(u32 *)modeBuffer->allocation & commandMask) | addressMask;
    modeBuffer->data = &packet->mode.packet + 1;
}

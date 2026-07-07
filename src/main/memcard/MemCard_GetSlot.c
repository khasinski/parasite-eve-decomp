/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;

extern u8 D_800A0ED6[];
extern u8 D_800A0EF1[];
extern u8 D_800A0EF0[];

void *MemCard_GetSlot(unsigned int port, int slot) {
    int port_offset;
    int slot_offset;
    u8 *base;

    if (port >= 2U) {
        return 0;
    }
    if (slot < 0) {
        return 0;
    }

    port_offset = ((((port << 5) + port) << 2) - port) << 3;
    if (slot >= D_800A0ED6[port_offset]) {
        return 0;
    }

    slot_offset = ((slot << 4) + slot) << 2;
    if (D_800A0EF1[slot_offset + port_offset] == 0) {
        return 0;
    }

    base = D_800A0EF0 + port_offset;
    return base + slot_offset;
}

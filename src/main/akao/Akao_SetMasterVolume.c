/* CC1_FLAGS: -O1 */

extern unsigned char *_spu_RXX;
extern short g_SpuMasterVolume[];

void Akao_SetMasterVolume(short left, short right) {
    *(short *)(_spu_RXX + 0x184) = left;
    *(short *)(_spu_RXX + 0x186) = right;
    g_SpuMasterVolume[0] = left;
    g_SpuMasterVolume[1] = right;
}

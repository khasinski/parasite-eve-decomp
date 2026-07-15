/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009D13C;
extern int D_8009D140;
extern int D_8009D144;

void func_8005F844(int variant) {
    D_8009D13C = variant != 0 ? 0x3A1C : 0x395D;
    D_8009D140 = variant != 0 ? 0xCC : 0x84;
    D_8009D144 = 0xA4;
}

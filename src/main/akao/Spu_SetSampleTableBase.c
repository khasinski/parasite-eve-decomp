/* CC1_PROFILE: build0001 */
extern int D_8009D240;
extern int D_8009D260;

void Spu_SetSampleTableBase(int arg0) {
    D_8009D240 = arg0;
    D_8009D260 = arg0 + 0x800;
}

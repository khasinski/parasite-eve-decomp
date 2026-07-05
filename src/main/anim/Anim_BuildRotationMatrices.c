/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

void Anim_DecodeBoneRotationsByte(void *arg0, void *arg1, s16 arg2);
void Anim_DecodeBoneRotationsShort(void *arg0, void *arg1, s16 arg2);
void RotMatrixYXZ(void *rot, void *matrix);

#define UH(ptr, off) (*(u16 *)((char *)(ptr) + (off)))
#define UB(ptr, off) (*(u8 *)((char *)(ptr) + (off)))
#define W(ptr, off) (*(s32 *)((char *)(ptr) + (off)))

#define APPLY_COMPONENT(entry, count, scratch, dst_off, value_off, abs_bit, add_bit) \
    do { \
        v1 = UB((entry), 0xA7); \
        v0 = v1 & (abs_bit); \
        if (v0 != 0) { \
            v0 = (count) << 3; \
            v1 = UH((entry), (value_off)); \
            v0p = (char *)(v0 + (s32)(scratch)); \
            *(u16 *)(v0p + (dst_off)) = v1; \
        } else { \
            v0 = v1 & (add_bit); \
            if (v0 != 0) { \
                v0 = (count) << 3; \
                v0p = (char *)(v0 + (s32)(scratch)); \
                v1 = *(u16 *)(v0p + (dst_off)); \
                a0tmp = UH((entry), (value_off)); \
                v1 += a0tmp; \
                *(u16 *)(v0p + (dst_off)) = v1; \
            } \
        } \
    } while (0)

void Anim_BuildRotationMatrices(char *obj, char *data, int arg2) {
    char *s1 = obj;
    register char *scratch asm("$18") = (char *)0x1F800000;
    register char *s0 asm("$16");
    register int count asm("$5");
    int v1;
    register int v0 asm("$2");
    register char *v0p asm("$2");
    int a0tmp;
    char *rot;
    char *mat_base;
    int mode;

    if (data == 0) {
        return;
    }

    UH(s1, 0x7C) = UH(data, 0xA);
    UH(s1, 0x74) = UH(data, 0x4);
    UH(s1, 0x76) = UH(data, 0x6);
    UH(s1, 0x78) = UH(data, 0x8);

    mode = UB(data, 0) & 3;
    if (mode == 2) {
        Anim_DecodeBoneRotationsShort(s1, data, (s16)arg2);
    } else {
        Anim_DecodeBoneRotationsByte(s1, data, (s16)arg2);
    }

    s0 = s1;
    do {
        count = UB(s0, 0xA6);
        if (count > 0) {
            APPLY_COMPONENT(s0, count, scratch, 0, 0xA0, 0x08, 0x01);
            APPLY_COMPONENT(s0, count, scratch, 2, 0xA2, 0x10, 0x02);
            APPLY_COMPONENT(s0, count, scratch, 4, 0xA4, 0x20, 0x04);
            rot = (char *)((u32)scratch | (count << 3));
            mat_base = (char *)W(s1, 0x58);
            RotMatrixYXZ(rot, mat_base + (count << 5));
        }
        s0 += 8;
    } while ((s32)s0 < (s32)(s1 + 0x10));
}

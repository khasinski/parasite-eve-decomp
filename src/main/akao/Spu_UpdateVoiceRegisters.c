typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;

#define U16(base, off) (*(u16 *)((char *)(base) + (off)))
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))

extern void *D_8009D2C8;
extern u32 D_8009D2C4;

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Spu_UpdateVoiceRegisters(void *voice, u32 voice_mask) {
    u32 old_value;
    u32 new_value;
    int value;
    int scaled;
    s16 *table;

    if (U16(voice, 0x72) != 0) {
        old_value = U32(voice, 0x44);
        new_value = old_value + U32(voice, 0x48);
        U16(voice, 0x72)--;
        if ((new_value & 0xFFE00000) != (old_value & 0xFFE00000)) {
            U32(voice, 0xF4) |= 3;
        }
        U32(voice, 0x44) = new_value;
    }

    if (U16(voice, 0x60) != 0) {
        U16(voice, 0x60)--;
        U16(voice, 0x5E) += U16(voice, 0xD6);
        U32(voice, 0xF4) |= 3;
    }

    if (U16(voice, 0x6E) != 0) {
        old_value = U16(voice, 0x6C);
        new_value = old_value + S16(voice, 0xD4);
        U16(voice, 0x6E)--;
        if ((new_value & 0x7F00) != (old_value & 0x7F00)) {
            U32(voice, 0xF4) |= 3;
        }
        U16(voice, 0x6C) = new_value;
    }

    if (U16(voice, 0x74) != 0) {
        old_value = S16(voice, 0xD8);
        new_value = old_value + S16(voice, 0xDA);
        U16(voice, 0x74)--;
        if ((U32(voice, 0x38) & 0x100) != 0 &&
            ((new_value & 0xFF00) != (old_value & 0xFF00))) {
            U32(voice, 0xF4) |= 3;
        }
        U16(voice, 0xD8) = new_value;
    }

    if (U16(voice, 0x78) != 0) {
        old_value = U16(voice, 0x76);
        new_value = old_value + S16(voice, 0xDC);
        U16(voice, 0x78)--;
        if ((new_value & 0xFF00) != (old_value & 0xFF00)) {
            U32(voice, 0xF4) |= 3;
        }
        U16(voice, 0x76) = new_value;
    }

    if (U16(voice, 0x8A) != 0) {
        U16(voice, 0x8A)--;
    }

    if (U16(voice, 0x9E) != 0) {
        U16(voice, 0x9E)--;
    }

    if (U16(voice, 0xBA) != 0) {
        U16(voice, 0xBA)--;
        if (U16(voice, 0xBA) == 0) {
            U32(D_8009D2C8, 0x34) ^= voice_mask;
            D_8009D2C4 |= 0x10;
            Seq_MarkTrack34MaskDirty();
        }
    }

    if (U16(voice, 0xBC) != 0) {
        U16(voice, 0xBC)--;
        if (U16(voice, 0xBC) == 0) {
            U32(D_8009D2C8, 0x3C) ^= voice_mask;
            Seq_MarkTrack3CMaskDirty();
        }
    }

    if (U16(voice, 0x96) != 0) {
        U16(voice, 0x96)--;
        U16(voice, 0x94) += U16(voice, 0x98);
        value = (U16(voice, 0x94) & 0x7F00) >> 8;
        if ((U16(voice, 0x94) & 0x8000) != 0) {
            scaled = (value * U32(voice, 0x30)) >> 7;
        } else {
            scaled = (value * (((U32(voice, 0x30) << 4) - U32(voice, 0x30)) >> 8)) >> 7;
        }
        U16(voice, 0x92) = scaled;

        if (U16(voice, 0x8A) == 0 && U16(voice, 0x8E) != 1) {
            table = (s16 *)U32(voice, 0x1C);
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            value = (S16(voice, 0x92) * table[0]) >> 16;
            if (value != S16(voice, 0xE8)) {
                U16(voice, 0xE8) = value;
                U32(voice, 0xF4) |= 0x10;
                if (value >= 0) {
                    U16(voice, 0xE8) = value << 1;
                }
            }
        }
    }

    if (U16(voice, 0xA8) != 0) {
        U16(voice, 0xA8)--;
        U16(voice, 0xA6) += U16(voice, 0xAA);
        if (U16(voice, 0x9E) == 0 && U16(voice, 0xA2) != 1) {
            table = (s16 *)U32(voice, 0x20);
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            value = (((S16(voice, 0x46) * (U16(voice, 0x6C) >> 8)) >> 7) *
                     (U16(voice, 0xA6) >> 8));
            value = ((value << 9) >> 16) * table[0];
            value >>= 15;
            if (value != S16(voice, 0xEA)) {
                U16(voice, 0xEA) = value;
                U32(voice, 0xF4) |= 3;
            }
        }
    }

    if (U16(voice, 0xB6) != 0) {
        U16(voice, 0xB6)--;
        U16(voice, 0xB4) += U16(voice, 0xB8);
        if (U16(voice, 0xB0) != 1) {
            table = (s16 *)U32(voice, 0x24);
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            value = ((U16(voice, 0xB4) >> 8) * table[0]) >> 15;
            if (value != S16(voice, 0xEC)) {
                U16(voice, 0xEC) = value;
                U32(voice, 0xF4) |= 3;
            }
        }
    }

    if (U16(voice, 0x7A) != 0) {
        old_value = U32(voice, 0x34);
        new_value = old_value + U32(voice, 0x4C);
        U16(voice, 0x7A)--;
        if ((new_value & 0xFFFF0000) != (old_value & 0xFFFF0000)) {
            U32(voice, 0xF4) |= 0x10;
        }
        U32(voice, 0x34) = new_value;
    }
}

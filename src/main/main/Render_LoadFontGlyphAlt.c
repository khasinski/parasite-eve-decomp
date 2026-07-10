typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[0x98]; } D_800B0DD8;
extern u16 D_80093176[];
extern u8 *D_800B0E6C;
extern u8 D_80091A20;
extern u32 D_80091A24;
extern u8 D_8009ECD8[];
extern u8 D_8009EE24[];
extern u8 D_80091A1C;
extern u8 D_80091A1D;
extern u8 D_80091A1E;
extern u8 D_80091A1F;
extern u8 *D_80091A28;

int CdRom_ReadSectorsFromLba(int lba, void *dst, int sectors);
int CdRom_PollReady(void);
int rand(void);
void srand(unsigned int seed);
void Render_LoadFontGlyph(int glyph);

u8 Render_LoadFontGlyphAlt(void) {
    u16 *range;
    int read_status;
    int poll_status;
    int i;
    int forced_slot;
    int unique_count;
    int glyph;
    int limit;
    int duplicate;
    int j;
    int marker_index;
    int sub_index;
    u8 *font_data;
    u8 section_count;
    u8 sub_count;

retry_read:
    range = D_80093176;
    do {
        read_status = CdRom_ReadSectorsFromLba(*(int *)&D_800B0DD8 + range[0],
                                               *(void **)((char *)&D_800B0DD8 + 0x94),
                                               range[1] - range[0]);
    } while (read_status == -1);

    do {
        poll_status = CdRom_PollReady();
        if (poll_status == -1) {
            goto retry_read;
        }
    } while (poll_status != 0);

    D_8009ECD8[0] = D_800B0E6C[0];
    if (D_80091A20 == 0 && D_80091A24 == 0) {
        do {
            D_80091A24 = rand() % 0x7FFF;
        } while (D_80091A24 == 0);
    }

    srand(D_80091A24);

    unique_count = 0;
    forced_slot = 0;
    for (i = 0; i < 0x45; i++) {
        if ((i + 2) % 10 == 0) {
            D_8009EE24[i] = forced_slot;
            forced_slot++;
            continue;
        }

        if (i + 2 < 10) {
            limit = 0x68;
        } else if (i + 2 < 20) {
            limit = 0x6D;
        } else {
            limit = 0x71;
        }

        if (D_8009ECD8[0] - 1 < limit) {
            limit = D_8009ECD8[0] - 1;
        }

        glyph = (rand() % (limit - 7)) + 7;
        D_8009EE24[i] = glyph;

        if (unique_count < D_8009ECD8[0] - 9) {
            do {
                duplicate = 0;
                for (j = 0; (u8)j < (u8)i; j++) {
                    if (D_8009EE24[j] == D_8009EE24[i]) {
                        D_8009EE24[i] = (rand() % (limit - 7)) + 7;
                        duplicate = 1;
                    }
                }
            } while (duplicate != 0);
            unique_count++;
        }
    }

    Render_LoadFontGlyph(D_8009EE24[0]);

    D_80091A1C = 1;
    D_80091A1D = 2;
    D_80091A1E = D_8009EE24[0];
    D_80091A28 = D_8009ECD8;

    marker_index = 0;
    font_data = D_8009ECD8;
    if (font_data[3] > 0) {
        section_count = font_data[3];
        for (i = 0; i < section_count; i++) {
            if (font_data[i + 4] == 2) {
                marker_index = i;
                i = section_count;
            }
        }
    }

    sub_index = 0xFF;
    sub_count = font_data[0x1C];
    if (sub_count > 0) {
        for (i = 0; i < sub_count; i++) {
            if (font_data[0x1D + i] == (u8)marker_index) {
                sub_index = i;
                break;
            }
        }
    }

    D_80091A1F = sub_index;
    return D_80091A28[D_80091A28[D_80091A1F + 0x1D] + 4];
}

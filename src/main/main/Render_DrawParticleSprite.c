typedef unsigned char u8;
typedef unsigned int u32;

typedef struct UnalignedWord {
    int value;
} __attribute__((packed)) UnalignedWord;

extern u8 D_8009B558[];
extern int D_8009B554;
extern void (*D_800A36A4)(int, void *);

void Render_DrawParticleSprite(int kind, void *arg1) {
    int code;
    u8 *state;

    if ((kind & 0xFF) == 2) {
        state = D_8009B558;
        code = state[0];

        if ((u8)code == 0xE) {
            if ((state[0x29] ^ state[1]) & 0x80) {
                *(int *)(state + 0x20) = 0xF;
                *(int *)(state + 0x1C) = 2;
                *(int *)(state + 0x3C) = 3;
            } else {
                *(int *)(state + 0x1C) = 1;
                *(int *)(state + 0x20) = 0xB;
            }
            state[0x29] = state[1];
        } else if ((u8)code == 3) {
            *(int *)(state + 0x20) = 0x10;
            *(int *)(state + 0x1C) = 2;
            state[0x2F] = code;
            *(int *)(state + 0x48) = 0x4B0;
        } else if ((u8)code == 6 || (u8)code == 0x1B) {
            *(int *)(state + 0x20) = 0x11;
            *(int *)(state + 0x1C) = 2;
            state[0x2F] = code;
            *(int *)(state + 0x48) = 0x4B0;
        } else {
            switch ((u8)code) {
            case 2:
                ((UnalignedWord *)(state + 0x2A))->value =
                    ((UnalignedWord *)(state + 1))->value;
                break;
            case 3:
            case 6:
            case 0x1B:
                state[0x2F] = code;
                break;
            case 0x15:
            case 0x16:
                state[0x2E] = code;
                break;
            }
            *(int *)(state + 0x1C) = 1;
            *(int *)(state + 0x20) = 0xB;
        }
    } else {
        state = D_8009B558 + 0x14;
        if (state[0] & 0x10) {
            *(int *)(state + 8) = 2;
            *(int *)(state + 0xC) = 0xC;
        } else {
            *(int *)(state + 8) = 1;
            *(int *)(state + 0xC) = 0xB;
        }
    }

    if (D_800A36A4 != 0 && D_8009B554 != 0) {
        D_800A36A4(kind & 0xFF, arg1);
    }
}

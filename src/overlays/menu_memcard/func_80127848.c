/* CC1_FLAGS: -fno-cse-skip-blocks */
#include "common.h"

typedef struct Rect {
    s16 x, y, w, h;
} Rect;

extern int D_801D11C8;
extern char *D_801D11BC[];
extern char *D_801D11C4;
extern int D_80193258;
extern char D_80193268[];

void func_8007506C(Rect *, char *);
void DrawSync(int);
void VSync(int);
void func_80074A44(int);
void PutDrawEnv(void *);
void func_800755F0(void *);

void func_80127848(void) {
    int i;
    int page;
    int y;
    char *env;
    Rect main_rect;

    for (i = 0; i < 2; i++) {
        page = D_801D11C8 == 0;
        D_801D11C4 = D_801D11BC[page];
        D_801D11C8 = page;
        main_rect.x = 0;
        y = 0x14;
        if (page == 0) y = 0x104;
        main_rect.w = 0x1E0;
        main_rect.h = 0xCC;
        main_rect.y = y;
        func_8007506C(&main_rect, D_80193268 + D_80193258);
        DrawSync(0);

        env = D_801D11C4;
        if (*(s16 *)(env + 0x74) > 0) {
            Rect clipped;
            clipped = *(Rect *)(env + 0x70);
            clipped.x = (clipped.x * 3) >> 1;
            if (D_801D11C8 == 0) clipped.y = (u16)clipped.y + 0xF0;
            clipped.w = (clipped.w * 3) >> 1;
            func_8007506C(&clipped, D_801D11C4 + 0x8080);
        }

        VSync(0);
        func_80074A44(1);
        PutDrawEnv(D_801D11C4);
        func_800755F0(D_801D11C4 + 0x5C);
    }
}

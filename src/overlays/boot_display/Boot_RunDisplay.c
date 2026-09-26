#include "pe1/boot_display.h"
extern BootDisplayCallbacks D_80125B4C, D_80125B68;
extern s32 D_8009D1A0, D_8009D1F4, D_8009D26C;
extern s32 D_80172CE0;
extern u32 g_BootDisplayFrameCounter;
extern void func_8012562C(void);
extern void func_80125744(void);
extern void func_80125894(s32);
extern s32 Boot_DrawTextBlock(void);
extern void func_8003EB04(void);
extern void func_801255A4(void);
void Boot_RunDisplay(void) {
    u32 state;
    s32 fade;
    if (D_8009D1A0 & 0x10000) {
        g_BootDisplayCallbacks = &D_80125B68;
    } else {
        g_BootDisplayCallbacks = &D_80125B4C;
        if (!(D_8009D1A0 & 0x20000)) D_80125B4C.text = 0;
    }
    g_BootDisplayCallbacks->init();
    if (!(D_8009D1A0 & 0x10000)) {
        func_8012562C();
        func_80125744();
        if (D_8009D1A0 & 0x20000) {
            func_800870F0(144);
            func_80125894(1);
        } else func_800870F0(218);
    }
    state = 0;
    fade = 0;
    g_BootDisplayFrameCounter = 420;
    D_80172CE4 = 0;
    D_80172CE0 = 0;
    do {
        if (g_BootDisplayCallbacks->step) g_BootDisplayCallbacks->step();
        if (g_BootDisplayCallbacks->text) g_BootDisplayCallbacks->text();
        switch (state) {
        case 0:
            if (Boot_DrawTextBlock()) {
                state = (D_8009D1A0 & 0x20000) ? 1 : 4;
                fade = 0;
            }
            break;
        case 1:
            g_BootDisplayCallbacks->transition((u8)fade);
            fade += 4;
            if (fade >= 256) { state = 2; fade = 0; }
            break;
        case 2:
            g_BootDisplayCallbacks->transition(255);
            if (D_80172CE4 && (D_8009D26C & 0xF0000004)) { state = 3; fade = 255; }
            break;
        case 3:
            g_BootDisplayCallbacks->transition((u8)fade);
            fade -= 4;
            if (fade < 0) state = 4;
            break;
        case 4: {
            register s32 finished asm("$2");
            finished = D_8009D1A0;
            if (finished & 0x30000) finished = 1;
            else finished = D_80172CE4;
            D_80172CE0 = finished;
            break;
        }
        }
        func_8003EB04();
        if (D_8009D1A0 & 0x10000) {
            if (D_8009D1F4 & 0x4000000) func_80125894(0);
            if (D_8009D1F4 & 0x8000000) func_80125894(1);
        }
        g_BootDisplayCallbacks->present();
        ++g_BootDisplayFrameCounter;
    } while (D_80172CE0 == 0);
    func_801255A4();
    g_BootDisplayCallbacks->shutdown();
    D_8009D1A0 &= ~0x10000;
}

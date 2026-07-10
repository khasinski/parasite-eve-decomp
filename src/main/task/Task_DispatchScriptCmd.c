/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D20C_o __asm__("g_FieldActorListHead");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D2F0_o __asm__("D_8009D2F0");
extern struct { char _[16]; } D_8009CDF8_o __asm__("D_8009CDF8");
extern struct { char _[16]; } D_8009CE00_o __asm__("D_8009CE00");
extern struct { char _[16]; } D_8009D300_o __asm__("D_8009D300");
extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
extern struct { char _[16]; } D_800BD028_o __asm__("D_800BD028");
extern struct { char _[16]; } D_800BD02A_o __asm__("D_800BD02A");
extern struct { char _[16]; } D_800BD025_o __asm__("D_800BD025");
extern struct { char _[16]; } D_800BD026_o __asm__("D_800BD026");
extern struct { char _[16]; } D_800BD027_o __asm__("D_800BD027");

#define g_FieldActorListHead (*(u8 **)&D_8009D20C_o)
#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D2F0 (*(u8 **)&D_8009D2F0_o)
#define D_8009CE00 (*(u32 *)&D_8009CE00_o)
#define D_8009D300 (*(u8 **)&D_8009D300_o)
#define D_800B0CD8 (*(u32 *)&D_800B0CD8_o)
#define D_800BD028 (*(s16 *)&D_800BD028_o)
#define D_800BD02A (*(s16 *)&D_800BD02A_o)
#define D_800BD025 (*(u8 *)&D_800BD025_o)
#define D_800BD026 (*(u8 *)&D_800BD026_o)
#define D_800BD027 (*(u8 *)&D_800BD027_o)
#define CDF8(index) (*(u8 *)((u8 *)&D_8009CDF8_o + (index)))

#define ARG_PTR(task, index) (*(u8 **)((u8 *)(task) + ((index) * 4)))
#define ARG_WORD(task, index) (*(u32 *)ARG_PTR(task, index))
#define ARG_HALF0(task, index) (*(s16 *)ARG_PTR(task, index))
#define ARG_HALF2(task, index) (*(s16 *)(ARG_PTR(task, index) + 2))
#define ARG_BYTE0(task, index) (*(u8 *)ARG_PTR(task, index))

void Render_UpdateScrollPosition(void *pos, int x, int y);
void Task_SetCollisionFlag(int enabled);
void func_800E00CC(void *coords, int mode, int arg2, int arg3, int arg4, int arg5, int arg6);
void *Scene_LoadMap(void *room_id, void *arg1, int arg2);
void Battle_DrawActiveStatus(void);
void Window_SetBoundsByMode(int mode);
int Asset_LoadTimTextures(int mode);

int Task_DispatchScriptCmd(void *task) {
    u32 cmd;
    s16 coords[4];
    u8 room_id[2];
    u8 *loaded;
    u8 *actor;
    u32 value;

    cmd = ARG_WORD(task, 0);

    switch (cmd) {
    case 0x835:
        D_800B0CD8 |= 0x800;
        return 1;

    case 0x898:
        Render_UpdateScrollPosition(D_8009D254 + 0x28, ARG_WORD(task, 1), ARG_WORD(task, 2));
        return 1;

    case 0x899:
        D_800BD028 = ARG_WORD(task, 1);
        D_800BD02A = ARG_WORD(task, 2);
        return 1;

    case 0x8FC:
        Task_SetCollisionFlag(ARG_WORD(task, 1) != 0);
        return 1;

    case 0x960:
        coords[0] = ARG_HALF2(task, 1);
        coords[1] = ARG_HALF2(task, 2);
        coords[2] = ARG_HALF2(task, 3);
        func_800E00CC(
            coords,
            0,
            ARG_HALF0(task, 4),
            ARG_BYTE0(task, 5),
            0,
            0,
            0);
        return 1;

    case 0x961:
        CDF8(0) = ARG_WORD(task, 1);
        CDF8(1) = ARG_WORD(task, 2);
        CDF8(2) = ARG_WORD(task, 3);
        return 1;

    case 0x962:
        coords[0] = ARG_HALF2(task, 1);
        coords[1] = ARG_HALF2(task, 2);
        coords[2] = ARG_HALF2(task, 3);
        func_800E00CC(
            coords,
            1,
            ARG_HALF0(task, 4),
            ARG_BYTE0(task, 5),
            CDF8(0),
            CDF8(1),
            CDF8(2));
        return 1;

    case 0x9C4:
        room_id[0] = ARG_WORD(task, 1);
        room_id[1] = ARG_WORD(task, 2);
        loaded = Scene_LoadMap(room_id, D_8009D2F0, 0);
        *(u32 *)(loaded + 0x28) = ARG_WORD(task, 3);
        *(u32 *)(loaded + 0x2C) = ARG_WORD(task, 4);
        *(u32 *)(loaded + 0x30) = ARG_WORD(task, 5);
        return 1;

    case 0xA28:
        *(u8 *)(D_8009D2F0 + 0x27C) = ARG_WORD(task, 1);
        return 1;

    case 0xA29:
        *(u8 *)(D_8009D2F0 + 0x27D) = ARG_WORD(task, 1);
        return 1;

    case 0xA8C:
        D_800BD025 = ARG_WORD(task, 1);
        D_800BD026 = ARG_WORD(task, 2);
        D_800BD027 = ARG_WORD(task, 3);
        return 1;

    case 0xAF0:
        Battle_DrawActiveStatus();
        return 1;

    case 0xAF1:
        Window_SetBoundsByMode(ARG_BYTE0(task, 1));
        return 1;

    case 0xB54:
        D_800B0CD8 |= 0x400000;
        return 1;

    case 0xBB8:
        value = ARG_WORD(task, 1);
        if (value == 0) {
            return 1;
        }

        actor = g_FieldActorListHead;
        while (actor != 0) {
            if (*(u8 *)(actor + 0xC) == value &&
                *(u8 *)(actor + 0xD) == ARG_WORD(task, 2)) {
                if ((*(u32 *)(actor + 0x98) & 0x10) == 0) {
                    return 1;
                }
            }
            actor = *(u8 **)(actor + 4);
        }
        return 0;

    case 0xC1C:
        if (Asset_LoadTimTextures(1) == 1) {
            D_8009CE00 -= 0x28;
            *(u32 *)(D_8009D300 + 0x10) = 1;
            return 0;
        }
        return 1;

    case 0xC80:
        D_800B0CD8 |= 0x8000000;
        return 1;

    default:
        return 1;
    }
}

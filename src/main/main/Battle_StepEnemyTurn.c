/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[16]; } D_8009E000_o __asm__("g_BattleTargetList");
extern struct { char _[16]; } D_800BE830_o __asm__("D_800BE830");
extern struct { char _[16]; } D_800B0E08_o __asm__("D_800B0E08");

#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define D_800B0E08 (*(void **)&D_800B0E08_o)
#define QUEUE_ENTRY(index) ((u8 *)&D_800BE830_o + ((u8)(index) << 3))
#define TARGET_ENTRY(index) ((u8 *)&D_8009E000_o + ((s8)(index) * 12))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define QUEUE_WRITE(entry, actor, action, slot) \
    do {                                       \
        *(u8 **)(entry) = (actor);             \
        *(s16 *)((entry) + 4) = (action);      \
        *(s16 *)((entry) + 6) = (slot);        \
    } while (0)

extern s8 D_8009CE3C;
extern s8 D_8009CE40;
extern s8 D_8009CE44;
extern s16 D_8009CE50;
extern s8 D_8009CE60;
extern u32 D_8009D1A0;
extern s8 D_8009D25C;
extern s16 D_8009D2A4;
extern s8 D_8009D2B0;
extern s8 D_8009D2D8;
extern u32 D_8009D1F4;

int rand(void);
void Battle_InitFadeVars(void);
void Asset_Find08Alt(int id, int arg1, int x, int y, int z);
void Battle_CopyPadStateToRecord(void);
void Battle_SetupEnemyAnims(void);
void Battle_SyncEnemyAttributes(void);
void Battle_CycleTarget(int mode);
void BattleCmd_UndoPending(void);
int Akao_SendTableCommand(void *slot, int cmd, int arg2, int arg3, int arg4);

int Battle_StepEnemyTurn(int step) {
    int result;
    int action;
    int queue_index;
    int slot;
    int i;
    u8 *entry;
    u8 *entity;
    u8 *target;

    result = step;

    if ((s8)step == 1) {
        action = D_8009D2A4;
        D_8009CE60 = 0;

        if (action > 0) {
            if (action < 0x183) {
                entry = QUEUE_ENTRY(D_8009CE3C);
                QUEUE_WRITE(entry, D_8009D254, action, D_8009D2D8);
                D_8009CE40 = 0;
                D_8009CE3C++;
                D_8009D2D8--;
                result = 0;
            } else if (action < 0x197) {
                switch (action) {
                case 0x189:
                    Battle_InitFadeVars();
                    D_8009CE40 = 4;
                    result++;
                    break;
                case 0x18A:
                    Battle_InitFadeVars();
                    D_8009CE40 = 5;
                    result++;
                    break;
                case 0x18B:
                    Battle_InitFadeVars();
                    D_8009CE40 = 6;
                    result++;
                    break;
                case 0x18D:
                    Battle_InitFadeVars();
                    D_8009CE40 = 7;
                    result++;
                    break;
                case 0x196:
                    Battle_InitFadeVars();
                    result = 0;
                    D_8009D25C = 0;
                    for (i = 1; (u8)i < 8; i++) {
                        queue_index = D_8009CE3C;
                        slot = D_8009D2D8;
                        entry = QUEUE_ENTRY(queue_index);
                        target = *(u8 **)TARGET_ENTRY(rand() % D_8009D2B0);
                        D_8009CE3C++;
                        QUEUE_WRITE(entry, target, D_8009D2A4, slot);
                    }
                    D_8009D2D8--;
                    D_8009CE40 = 0;
                    break;
                default:
                    entry = QUEUE_ENTRY(D_8009CE3C);
                    QUEUE_WRITE(entry, D_8009D254, D_8009D2A4, D_8009D2D8);
                    D_8009CE40 = 0;
                    D_8009D2D8--;
                    D_8009CE3C++;
                    result = 0;
                    break;
                }
            } else if (action < 0x199) {
                result = -1;
                entry = QUEUE_ENTRY(D_8009CE3C);
                entity = D_8009D254;
                QUEUE_WRITE(entry, entity, action, D_8009D2D8);

                if ((D_8009D1A0 & 2) == 0) {
                    D_8009D278 = *(u8 **)entity;
                    Asset_Find08Alt(
                        0x453,
                        1,
                        S16_AT(entity, 0x2A),
                        S16_AT(entity, 0x2E),
                        S16_AT(entity, 0x32));
                }

                if (action == 0x197) {
                    Battle_CopyPadStateToRecord();
                    Battle_SetupEnemyAnims();
                    *(u8 *)(D_8009D278 + 0x12) = 0xD;
                } else if (action == 0x198) {
                    Battle_SyncEnemyAttributes();
                }

                D_8009D2D8 = 0;
                D_8009CE40 = 0;
                D_8009CE3C++;
            } else {
                result = -1;
                entry = QUEUE_ENTRY(D_8009CE3C);
                QUEUE_WRITE(entry, D_8009D254, action, D_8009D2D8);
                D_8009D2D8 = 0;
                D_8009CE40 = 0;
                D_8009CE3C++;
            }

            if (D_8009CE40 == 0 && D_800B0E08 != 0) {
                Akao_SendTableCommand(D_800B0E08, 0x44C, 0, 0x80, 0x7F);
            }
            D_8009CE50 = D_8009D2A4;
        } else if (action == -1) {
            result = 0;
        }
    } else if ((s8)step == 2) {
        Battle_CycleTarget(D_8009CE40);

        if ((D_8009D1F4 & 0x200) != 0) {
            entry = QUEUE_ENTRY(D_8009CE3C);
            target = *(u8 **)TARGET_ENTRY(D_8009CE44);
            QUEUE_WRITE(entry, target, D_8009CE50, D_8009D2D8);
            D_8009CE40 = 0;
            D_8009D2D8--;
            D_8009CE3C++;
            result = 0;
            if (D_800B0E08 != 0) {
                Akao_SendTableCommand(D_800B0E08, 0x44C, 0, 0x80, 0x7F);
            }
        } else if ((D_8009D1F4 & 0x400) != 0) {
            BattleCmd_UndoPending();
            result = 0;
            D_8009CE44 = 0;
            if (D_800B0E08 != 0) {
                Akao_SendTableCommand(D_800B0E08, 0x44D, 0, 0x80, 0x7F);
            }
        }
    }

    return (s8)result;
}

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;

typedef struct {
    int active;
    char pad[0xD8];
} SaveSlotState;

extern SaveSlotState D_800A5D58[];
extern u8 D_8009D2A0;
extern u8 D_8009D2EC;

void func_8002F658(void);
void Save_ResetGlobalFlags(void);

void Save_InitSystem(void)
{
    u8 i;

    func_8002F658();
    for (i = 0; i < 7; i++) {
        D_800A5D58[i].active = 0;
    }
    D_8009D2A0 = 0;
    D_8009D2EC = 0;
    Save_ResetGlobalFlags();
}

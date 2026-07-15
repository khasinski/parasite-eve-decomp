typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *D_8009D278;

void Scene_SetDiscSide(int side);

void Battle_SetupEnemyAnims(void) {
    u8 anim_table[45] = {
        0, 0, 0, 0, 0,
        0, 0, 5, 10, 2,
        5, 0, 0, 10, 4,
        0, 0, 0, 10, 2,
        10, 7, 0, 10, 4,
        0, 0, 5, 15, 3,
        0, 0, 20, 30, 3,
        0, 0, 5, 10, 2,
        0, 0, 0, 0, 5
    };
    u8 scale_table[9] = {
        0, 10, 8, 10, 10, 4, 8, 8, 20
    };
    u8 *combatant;
    u8 *action;
    u8 action_id;
    u8 *row;

    combatant = D_8009D278;
    action = *(u8 **)(combatant + 0x68);
    action_id = action[6];

    combatant[0x12] = 4;
    *(short *)(combatant + 0x24) = (*(u16 *)(combatant + 0x22) * scale_table[action_id]) / 10;
    D_8009D278[0x13] = 5;
    D_8009D278[0x14] = 6;
    D_8009D278[0x17] = 7;
    D_8009D278[0x15] = 8;
    D_8009D278[0x18] = 9;
    D_8009D278[0x16] = 10;
    D_8009D278[0x19] = 11;

    row = &anim_table[action_id * 5];
    action[0x14] = row[0];
    action[0x15] = row[1];
    action[0x16] = row[2];
    action[0x17] = row[3];
    *(int *)(action + 8) = row[4];

    Scene_SetDiscSide(*(short *)(*(u8 **)(D_8009D278 + 0x68) + 6));
}

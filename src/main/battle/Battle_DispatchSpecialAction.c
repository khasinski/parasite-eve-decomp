/* CC1_FLAGS: -g3 */

typedef short s16;

typedef struct ActorLike {
    int firstWord;
    char pad04[0x26];
    s16 x;
    char pad2C[2];
    s16 y;
    char pad30[2];
    s16 z;
} ActorLike;

extern int g_GameStateFlags;
extern ActorLike *g_PlayerEntity;
extern int g_ActiveActor;

void Asset_Find08Alt(int id, int arg1, int x, int y, int z);
void Battle_CopyPadStateToRecord(void);
void Battle_SetupEnemyAnims(void);
void Battle_SyncEnemyAttributes(void);

void Battle_DispatchSpecialAction(int arg0) {
    ActorLike *actor;

    if ((g_GameStateFlags & 2) == 0) {
        actor = g_PlayerEntity;
        g_ActiveActor = actor->firstWord;
        Asset_Find08Alt(0x453, 1, actor->x, actor->y, actor->z);
    }

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        *(char *)(g_ActiveActor + 0x12) = 0xD;
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    }
}

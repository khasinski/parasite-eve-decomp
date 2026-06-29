typedef signed char s8;

extern void *g_ActiveActor;

int rand(void);
int Battle_GetAgilityBonus(void);
void Battle_SetupEntityTarget(void *arg0);

void Battle_RollEnemySpawn(void *arg0)
{
    void *ctx;
    int diff;
    register int threshold asm("$16");
    register int roll asm("$2");
    register int scaled asm("$3");

    ctx = *(void **)arg0;
    if (*(int *)((char *)ctx + 0x10) <= 0) {
        return;
    }

    diff = (s8)(*(unsigned char *)((char *)g_ActiveActor + 4) - *(unsigned char *)((char *)ctx + 4));
    if (diff > 0) {
        threshold = Battle_GetAgilityBonus();
        roll = rand() % 100;
        scaled = roll * 10;
    } else if (diff == 0) {
        threshold = Battle_GetAgilityBonus() * 3;
        roll = rand() % 100;
        scaled = roll * 50;
    } else if (diff == -1) {
        threshold = Battle_GetAgilityBonus() * 3;
        roll = rand() % 100;
        scaled = roll * 100;
    } else if (diff < -1) {
        threshold = Battle_GetAgilityBonus();
        roll = rand() % 100;
        scaled = roll * 50;
    } else {
        return;
    }

    if (scaled < threshold) {
        Battle_SetupEntityTarget(arg0);
    }
}

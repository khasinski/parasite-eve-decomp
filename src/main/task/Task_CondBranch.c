/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct Entity Entity;

struct Entity {
    char pad0[0x9C];
    int field9C;
};

extern Entity *g_CurrentEntity[];
int g_SceneDataTable0;

int Task_CondBranch(int **arg0) {
    int *a;
    int *b;
    int *c;

    a = arg0[0];
    b = arg0[1];
    if (*a != *b) {
        c = arg0[2];
        g_SceneDataTable0 = g_CurrentEntity[0]->field9C + (*c * 2);
    }
    return 1;
}

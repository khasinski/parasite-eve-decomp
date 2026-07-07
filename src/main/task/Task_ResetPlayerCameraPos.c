
typedef struct Entity Entity;

struct Entity {
    char pad0[0x28];
    char field28[1];
};

extern Entity *g_PlayerEntity[];

void Render_UpdateScrollPosition(void *arg0, int arg1, int arg2);

int Task_ResetPlayerCameraPos(void) {
    Render_UpdateScrollPosition(g_PlayerEntity[0]->field28, -1, -1);
    return 1;
}

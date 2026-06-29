/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct {
    char pad00[0x9C];
    int field9C;
    int fieldA0;
    char padA4[0xF8];
    int field19C;
    int field1A0;
} State;

extern State *g_CurrentEntity[];
extern int *g_TaskNodePool;

int Entity_SelectFieldSetter(int **arg0) {
    int value;
    int selector;

    value = *arg0[1];
    if (value < 0) {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity[0]->field1A0 = 0;
            break;
        case 2:
            g_CurrentEntity[0]->field19C = 0;
            break;
        case 3:
            g_TaskNodePool[1] = 0;
            break;
        }
    } else {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity[0]->field1A0 = g_CurrentEntity[0]->field9C + (value << 1);
            break;
        case 2:
            g_CurrentEntity[0]->field19C = g_CurrentEntity[0]->field9C + (value << 1);
            break;
        case 3:
            g_TaskNodePool[1] = g_CurrentEntity[0]->field9C + (value << 1);
            break;
        }
    }

    return 1;
}

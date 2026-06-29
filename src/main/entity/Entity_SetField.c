extern char *g_CurrentEntity;

void Battle_SetContextField(int arg0, int arg1);
void Entity_WriteFieldByCmd(void *arg0, int arg1, int arg2);

int Entity_SetField(char ***arg0) {
    char *ctx = g_CurrentEntity;

    if (*(unsigned char *)(ctx + 0xC) == 0) {
        Battle_SetContextField(*(unsigned char *)arg0[0], *(int *)arg0[1]);
    } else {
        Entity_WriteFieldByCmd(ctx, *(unsigned char *)arg0[0], *(int *)arg0[1]);
    }

    return 1;
}

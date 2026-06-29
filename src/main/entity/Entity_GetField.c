extern char *g_CurrentEntity;

void *Battle_GetContextField(int arg0);
void *Battle_GetEnemyContextField(void *arg0, int arg1);

int Entity_GetField(char ***arg0) {
    register char *value asm("$2");
    char *ctx = g_CurrentEntity;

    if (*(unsigned char *)(ctx + 0xC) == 0) {
        value = Battle_GetContextField(*(unsigned char *)arg0[0]);
    } else {
        value = Battle_GetEnemyContextField(ctx, *(unsigned char *)arg0[0]);
    }

    *arg0[1] = (char *)value;
    return 1;
}

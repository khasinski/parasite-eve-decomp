typedef struct TempPoint {
    int x;
    int y;
} TempPoint;

extern int g_SceneDispatchToken;

int Str_EncodeBase32(char *out, unsigned int value);
int Str_ParseMapNumber(signed char *arg0);

int Save_GetCurrentMapNumber(void) {
    TempPoint point;

    Str_EncodeBase32(&point, g_SceneDispatchToken);
    return Str_ParseMapNumber(&point);
}

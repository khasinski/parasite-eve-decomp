typedef struct TempPoint {
    int x;
    int y;
} TempPoint;

extern int g_SceneDispatchToken;

void Str_EncodeBase32(TempPoint *point, int arg1);
int Str_ParseMapNumber(TempPoint *point);

int Save_GetCurrentMapNumber(void) {
    TempPoint point;

    Str_EncodeBase32(&point, g_SceneDispatchToken);
    return Str_ParseMapNumber(&point);
}

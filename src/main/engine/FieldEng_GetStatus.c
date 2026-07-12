typedef unsigned char u8;

extern char *D_8009D254;

int FieldEng_GetStatus(char *obj) {
    char *data = *(char **)(obj + 8);
    int status = 0;

    if (data != 0) {
        char *inner = *(char **)data;

        if (inner == 0) {
            status = 1;
        } else {
            status = 3;
            if (*(int *)(inner + 0x10) <= 0) {
                status = 2;
            }
        }
    }

    if ((u8)obj[1] == 7) {
        status = 4;
    }
    if ((u8)obj[1] == 0xE) {
        status = 4;
    }
    if (data == D_8009D254) {
        status = 5;
    }

    return status;
}

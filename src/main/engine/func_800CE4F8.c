extern int D_800B0E18;
extern int D_800B0E1C;
extern int D_800B1630;

int func_800CE4F8(int index) {
    int ret;

    if (index == 1) {
        goto ret1;
    }
    if (index < 2) {
        if (index == 0) {
            goto ret0;
        }
        ret = 0;
        goto done;
    }
    if (index == 2) {
        goto ret2;
    }
    ret = 0;
    goto done;

ret0:
    ret = D_800B0E18;
    goto done;
ret1:
    ret = D_800B0E1C;
    goto done;
ret2:
    ret = D_800B1630;
done:
    return ret;
}

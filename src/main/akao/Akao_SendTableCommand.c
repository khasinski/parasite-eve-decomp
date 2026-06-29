int Asset_FindTable2CByU16Key(void *arg0, int arg1);
int Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3);

int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4) {
    int ret = Asset_FindTable2CByU16Key(arg0, arg1);

    if (ret == 0) {
        goto fail;
    }
    ret = Akao_Cmd_24(ret, arg2, arg3, arg4);
    goto done;

fail:
    ret = -1;
done:
    return ret;
}

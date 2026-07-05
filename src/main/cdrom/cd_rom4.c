/* MASPSX_FLAGS: --stack-return-delay */

int Render_AllocParticleNode(int arg0, int arg1, int arg2, int arg3);
int Render_FindParticleEffect(int arg0, int arg1);

int CdRom_ReadSync(unsigned char arg0, int arg1, int arg2) {
    int status;
    int handle;

    handle = Render_AllocParticleNode(arg0, arg1, 0, 0);
    if (handle == 0) {
        return 0;
    }

    do {
        status = Render_FindParticleEffect(handle, arg2) & 0xFF;
    } while (status == 0);

    return status == 2;
}

int CdRom_ReadSyncAlt(unsigned char arg0, int arg1, int arg2) {
    int status;
    int handle;

    handle = Render_AllocParticleNode(arg0, arg1, 0, 0);
    if (handle == 0) {
        return 0;
    }

    do {
        status = Render_FindParticleEffect(handle, arg2) & 0xFF;
    } while (status == 0);

    return status == 2;
}

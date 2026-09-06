/* GCC_VERSION: 2.8.1 */
extern int Render_AllocParticleNode(int, void *, int, int);
extern int Render_FindParticleEffect(int, void *);

int cd_rom4(unsigned char command, void *param, void *result) {
    int request;
    int zero = 0;
    unsigned char status;
    asm("" : "+r"(zero));
    request = Render_AllocParticleNode(command, param, zero, 0);
    if (!request) return 0;
    do {
        status = Render_FindParticleEffect(request, result);
    } while (!status);
    return status == 2;
}

int func_80080DC4(unsigned char command, void *param, void *result) {
    int request;
    int zero = 0;
    unsigned char status;
    asm("" : "+r"(zero));
    request = Render_AllocParticleNode(command, param, zero, 0);
    if (!request) return 0;
    do {
        status = Render_FindParticleEffect(request, result);
    } while (!status);
    return status == 2;
}

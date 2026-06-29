int Render_AllocParticleNode(int arg0, int arg1, int arg2, int arg3);

int DsControlF(unsigned char arg0, int arg1) {
    return Render_AllocParticleNode(arg0, arg1, 0, 0);
}

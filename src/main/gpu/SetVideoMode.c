/* MASPSX_FLAGS: --store-return-delay */
extern int g_VideoMode;

int SetVideoMode(int arg0) {
    int old = g_VideoMode;
    g_VideoMode = arg0;
    return old;
}

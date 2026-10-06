void SetDrawTPage(void *p, int dfe, int dtd, int tpage);
void SetTile(unsigned char *arg0);
int MargePrim(void *arg0, void *arg1);
void exit(int code);

void Gpu_InitDrawModeTilePacket(void *packet, int tpage) {
    void *prim = (char *)packet + 8;

    SetDrawTPage(packet, 0, 1, tpage);
    SetTile(prim);
    if (MargePrim(packet, prim) != 0) {
        exit(-1);
    }
}

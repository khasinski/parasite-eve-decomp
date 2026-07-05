void SetDrawMode(void *p, int dfe, int dtd, int tpage);
void SetTile(unsigned char *arg0);
int Gpu_AppendPacketPrimitive(void *arg0, void *arg1);
void exit(int code);

void Gpu_InitDrawModeTilePacket(void *packet, int tpage) {
    void *prim = (char *)packet + 8;

    SetDrawMode(packet, 0, 1, tpage);
    SetTile(prim);
    if (Gpu_AppendPacketPrimitive(packet, prim) != 0) {
        exit(-1);
    }
}

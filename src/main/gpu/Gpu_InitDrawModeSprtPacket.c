/* CC1_PROFILE: build0001 */
void SetDrawMode(void *p, int dfe, int dtd, int tpage);
void SetSprt(void *p);
int Gpu_AppendPacketPrimitive(void *arg0, void *arg1);
void exit(int code);

void Gpu_InitDrawModeSprtPacket(void *packet, int tpage) {
    void *prim = (char *)packet + 8;

    SetDrawMode(packet, 0, 1, tpage);
    SetSprt(prim);
    if (Gpu_AppendPacketPrimitive(packet, prim) != 0) {
        exit(-1);
    }
}

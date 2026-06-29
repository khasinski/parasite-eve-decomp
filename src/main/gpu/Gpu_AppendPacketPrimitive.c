int Gpu_AppendPacketPrimitive(unsigned char *packet, int *prim) {
    int value;

    value = packet[3] + ((unsigned char *)prim)[3] + 1;
    if (value >= 0x11) {
        return -1;
    }

    packet[3] = value;
    *prim = 0;
    return 0;
}

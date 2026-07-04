/* MASPSX_FLAGS: --load-dest-temp */
typedef unsigned char u8;
typedef unsigned int u32;

extern u32 g_DsReadStatusBlock[];

u32 DsSync(u32 mode) {
    u32 offset = mode << 2;
    return *(u32 *)((u8 *)g_DsReadStatusBlock + offset);
}

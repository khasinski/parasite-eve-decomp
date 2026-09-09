extern void *D_800C0DC8;
extern unsigned int D_800C20C4;
void StClearRing(void);
void StSetRing(void *buffer, unsigned int size) {
 D_800C0DC8 = buffer;
 D_800C20C4 = size;
 StClearRing();
}

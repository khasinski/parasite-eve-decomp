void StCdInterrupt(void);

void StCdInterrupt2(unsigned char event, unsigned char *result) {
    StCdInterrupt();
}

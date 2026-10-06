/* Psy-Q LIBDS DSREAD2.OBJ: StCdInterrupt2. */
void StCdInterrupt(void);

void StCdInterrupt2(unsigned char event, unsigned char *result) {
    StCdInterrupt();
}

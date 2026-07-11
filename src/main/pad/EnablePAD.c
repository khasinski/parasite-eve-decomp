typedef void (*PadToggleFunc)(void);

extern PadToggleFunc jtbl_800A34C8;
extern PadToggleFunc jtbl_800A34CC;

void EnablePAD(void) {
    jtbl_800A34C8();
}

void DisablePAD(void) {
    jtbl_800A34CC();
}

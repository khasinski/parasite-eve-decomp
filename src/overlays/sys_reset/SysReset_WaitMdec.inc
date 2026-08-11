#define SYSRESET_WAIT_MDEC(name, status, mask, message) \
    int name(void) { \
        volatile int timeout = 0x100000; \
        if ((*status & mask) != 0) { \
            do { \
                timeout = timeout - 1; \
                if (timeout == -1) { \
                    func_8010C448(message); \
                    return -1; \
                } \
            } while ((*status & mask) != 0); \
        } \
        return 0; \
    }

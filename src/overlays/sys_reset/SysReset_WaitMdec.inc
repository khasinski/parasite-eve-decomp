#define SYSRESET_WAIT_MDEC(name, status, mask, message_page_size) \
    typedef struct name##_MessagePage { \
        char message[message_page_size]; \
    } name##_MessagePage; \
    register unsigned int g_##name##_MessageArg asm("$4"); \
    int name(void) { \
        volatile int timeout = 0x100000; \
        if ((*status & mask) != 0) { \
            g_##name##_MessageArg = -1; \
            do { \
                timeout = timeout - 1; \
                if (timeout == g_##name##_MessageArg) { \
                    g_##name##_MessageArg = 0x80110000; \
                    func_8010C448((char *)(g_##name##_MessageArg - \
                                           sizeof(name##_MessagePage))); \
                    return -1; \
                } \
            } while ((*status & mask) != 0); \
        } \
        return 0; \
    }

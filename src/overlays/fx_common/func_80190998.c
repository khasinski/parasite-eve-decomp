extern char D_801E4E6C[];
extern char D_8019C340[];
extern char D_801EA5E8[];
extern char D_8019CC58[];
extern short D_8019C39C;
extern short D_8019CBC0;
extern short D_8019C830[];
extern void *D_8019CCBC[];
extern void *D_801EA650[];
extern int D_801EA64C[];
extern int D_8019CCC0[];
extern short D_801E4A88[];
extern short D_8019C9D0;
extern short D_801E4DA8;
extern short D_801E4DAA;
extern short D_801E4DAC;
extern short D_801E4DAE;

extern void func_80191580(void *);

void func_80190998(void)
{
    int i, offset;
    char *slot;

    i = 1;
    slot = D_801E4E6C;
    for (; i < 200; i++, slot += 108)
        func_80191580(slot);

    func_80191580(D_8019C340);
    D_8019C39C = 0;
    D_8019CBC0 = 0;
    for (i = 0; i < 200; i++)
        D_8019C830[i] = i + 1;

    i = 0;
    {
        char *source = D_801EA5E8;
        char *dest = D_8019CC58;

        for (; i < 3;) {
            offset = i * 108;
            *(void **)((char *)D_8019CCBC + offset) = source;
            source += 108;
            *(void **)((char *)D_801EA650 + offset) = dest;
            dest += 108;
            *(int *)((char *)D_801EA64C + offset) = 0;
            *(int *)((char *)D_8019CCC0 + offset) = 0;
            i++;
        }
    }

    {
        short empty = -1;

        offset = 0;
        for (; offset < 800; offset += 4) {
            *(short *)((char *)D_801E4A88 + offset) = empty;
            *(short *)((char *)D_801E4A88 + offset + 2) = empty;
        }
    }

    D_8019C9D0 = 200;
    D_801E4DA8 = -1;
    D_801E4DAA = 201;
    D_801E4DAC = 200;
    D_801E4DAE = -1;
}

extern int g_MemCardEventF000Spec0004;
extern int g_MemCardEventF000Spec8000;
extern int g_MemCardEventF000Spec0100;
extern int g_MemCardEventF000Spec2000;

extern int g_MemCardEventF000Spec0004Flag;
extern int g_MemCardEventF000Spec8000Flag;
extern int g_MemCardEventF000Spec2000Flag;

int TestEvent(int event);

void Evt_ClearEvents(void) {
    TestEvent(g_MemCardEventF000Spec0004);
    TestEvent(g_MemCardEventF000Spec8000);
    TestEvent(g_MemCardEventF000Spec0100);
    TestEvent(g_MemCardEventF000Spec2000);
    g_MemCardEventF000Spec2000Flag = 0;
    g_MemCardEventF000Spec8000Flag = 0;
    g_MemCardEventF000Spec0004Flag = 0;
}

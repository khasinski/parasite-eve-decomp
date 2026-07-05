extern int g_MemCardEventPort1Spec0004;
extern int g_MemCardEventPort1Spec8000;
extern int g_MemCardEventPort1Spec0100;
extern int g_MemCardEventPort1Spec2000;

extern int g_MemCardEventF400Spec0004Flag;
extern int g_MemCardRemovedEventPending;
extern int g_MemCardEventF400Spec2000Flag;

int TestEvent(int event);

extern int g_MemCardEventF000Spec0004;
extern int g_MemCardEventF000Spec8000;
extern int g_MemCardEventF000Spec0100;
extern int g_MemCardEventF000Spec2000;

extern int g_MemCardEventF000Spec0004Flag;
extern int g_MemCardEventF000Spec8000Flag;
extern int g_MemCardEventF000Spec2000Flag;

void MemCard_ClearPort1Events(void) {
    TestEvent(g_MemCardEventPort1Spec0004);
    TestEvent(g_MemCardEventPort1Spec8000);
    TestEvent(g_MemCardEventPort1Spec0100);
    TestEvent(g_MemCardEventPort1Spec2000);
    g_MemCardEventF400Spec2000Flag = 0;
    g_MemCardRemovedEventPending = 0;
    g_MemCardEventF400Spec0004Flag = 0;
}

void Evt_ClearEvents(void) {
    TestEvent(g_MemCardEventF000Spec0004);
    TestEvent(g_MemCardEventF000Spec8000);
    TestEvent(g_MemCardEventF000Spec0100);
    TestEvent(g_MemCardEventF000Spec2000);
    g_MemCardEventF000Spec2000Flag = 0;
    g_MemCardEventF000Spec8000Flag = 0;
    g_MemCardEventF000Spec0004Flag = 0;
}

extern void StopCARD2(void);
extern void _ExitCard(void);

long StopCARD(void) {
    StopCARD2();
    _ExitCard();
}

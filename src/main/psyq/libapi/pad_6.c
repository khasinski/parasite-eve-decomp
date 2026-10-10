/* PSY-Q LIBAPI PAD, part 6 of 6: RemovePatchPad, func_8007E0F8, func_8007E160. */

extern void D_800A34B0;

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void SysDeqIntRP(int index, void *queue);

int RemovePatchPad(void) {
    EnterCriticalSection();
    SysDeqIntRP(1, &D_800A34B0);
    ExitCriticalSection();
    return 1;
}

typedef short s16;

typedef struct PadWork {
    char pad[0xA];
    s16 unk_a;
} PadWork;

extern PadWork *D_8009B4B0;

int func_8007E0F8(void) {
    volatile int delay[4];
    PadWork *work;

    work = D_8009B4B0;
    work->unk_a = 0;

    delay[0] = 10;
    if (--delay[0] != -1) {
        do {
        } while (--delay[0] != -1);
    }

    return 0;
}

extern int *D_8009B4B4;
int func_8007E160(void)
{
  register int *state;
  register int result;
  int new_var;
  state = D_8009B4B4;
  new_var = state[1];
  result = new_var;
  result = result & 1;
  if ((new_var & 1) == 0)
  {
    result = 0;
  }
  else if ((state[0] & 1) != 0)
  {
    new_var = 1;
    result = new_var;
  }
  else
  {
    result = (long) 0;
  }
  return result;

}

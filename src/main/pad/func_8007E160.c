
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
    goto done;
  }
  ;
  if ((state[0] & 1) != 0)
  {
    new_var = 1;
    result = new_var;
    goto done;
  }
  result = (long) 0;
  done:
  return result;

}

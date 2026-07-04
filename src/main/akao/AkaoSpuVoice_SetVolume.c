
void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right)
{
  unsigned int new_var;
  unsigned short *ptr = (unsigned short *) (0x1F801C00 + (((unsigned long long) index) * 0x10));
  new_var = 0x7FFF;
  ptr[0] = left & new_var;
  ptr[1] = right & 0x7FFF;
}

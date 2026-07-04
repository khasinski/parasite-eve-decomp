
extern unsigned char g_MemCardObjStorage[];
/* The inner do { } while (0) is a load-bearing scheduler fence: it keeps the
 * candidate advance out of the value/index update order so the bnez delay
 * slot gets the pointer increment, matching retail. */
int CardObj_GetChannelId(unsigned char *entry)
{
  unsigned char *candidate;
  int index;
  int value;
  index = 0;
  value = 0x10;
  candidate = g_MemCardObjStorage;
  for (; index < 2; index++)
  {
    if (entry == candidate)
    {
      return value;
    }
    value += 0x10;
 do { candidate += 0xF0; } while (0);
  }

  return 0xFF;
}

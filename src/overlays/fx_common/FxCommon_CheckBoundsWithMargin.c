/* MASPSX_FLAGS: --expand-div */

extern int D_8019CBB0;
extern int D_8019CBB4;
extern int D_8019CBB8;
extern int D_8019CBD0;
extern int D_8019CBD4;
extern int D_8019CBD8;
extern int D_8019CB48;
extern int D_8019CB4C;
extern int D_8019CC04;
extern int D_8019CC0C;
extern int D_8019CBC8;
extern int D_8019CC00;
int FxCommon_CheckBoundsWithMargin(int *point, int margin)
{
    int first;
    int second;
    int distance;
    /* Preserve the separate margin scaling and signed-short conversions. */
    register int limit asm("$2");
    int secondVisible = 0;
    int firstVisible = 0;
    int threshold = margin;
    first = D_8019CBB0 * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0 * point[0] + D_8019CBD4 * point[1]
           + D_8019CBD8 * point[2] + D_8019CB4C;
    if (D_8019CC04 > 0 && first >= 0)
    {
      firstVisible = 1;
    }
    if (D_8019CC04 < 0 && first <= 0)
    {
      firstVisible = 1;
    }
    if (D_8019CC0C > 0 && second >= 0)
    {
      secondVisible = 1;
    }
    if (D_8019CC0C < 0 && second <= 0)
    {
      secondVisible = 1;
    }
    limit = threshold * 8;
    threshold = limit + 30;
    if (!firstVisible)
    {
      if (first >= 0)
      {
        distance = first / D_8019CBC8;
      }
      else
      {
        distance = (-first) / D_8019CBC8;
      }
      limit = (short)threshold;
      if (distance < limit)
      {
        firstVisible = 1;
      }
    }
    if (!secondVisible)
    {
      if (second >= 0)
      {
        distance = second / D_8019CC00;
      }
      else
      {
        distance = (-second) / D_8019CC00;
      }
      limit = (short)threshold;
      if (distance < limit)
      {
        secondVisible = 1;
      }
    }
    return firstVisible & secondVisible;
}

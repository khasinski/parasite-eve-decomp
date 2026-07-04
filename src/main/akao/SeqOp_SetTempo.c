
extern char *g_AkaoCurTrack;
void SeqOp_SetTempo(void *ptr)
{
  unsigned char *stream = *((unsigned char **) ptr);
  char *track;
  register unsigned int value;
  *((unsigned char **) ptr) = stream + 1;
  track = g_AkaoCurTrack;
  ;
  *((unsigned int *) (track + 0x20)) = (*stream) << 16;
  stream = *((unsigned char **) ptr);
  *((unsigned char **) ptr) = stream + 1;
  stream = (unsigned char *) ((unsigned int) (*stream));
  value = *((unsigned int *) (track + 0x20));
  *((short *) (track + 0x52)) = 0;
  value |= ((unsigned int) stream) << 24;
  *((unsigned int *) (track + 0x20)) = value;
}

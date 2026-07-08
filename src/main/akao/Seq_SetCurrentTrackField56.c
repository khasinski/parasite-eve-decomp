#include "pe1/akao.h"

extern char *g_AkaoCurTrack;

void Seq_SetCurrentTrackField56(AkaoValueCommand *arg0) {
    ((AkaoTrack *)g_AkaoCurTrack)->field_56 = arg0->field_4;
}

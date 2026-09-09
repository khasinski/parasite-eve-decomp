#include "pe1/psyq_spu_internal.h"

void SpuGetVoiceEnvelope(unsigned long voice, unsigned short *env)
{
    *env = (voice + _spu_RXX->voice)->envelope;
}

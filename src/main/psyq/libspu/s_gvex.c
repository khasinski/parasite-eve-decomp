extern unsigned char *_spu_RXX;

void SpuGetVoiceEnvelope(unsigned long voice, unsigned short *env)
{
    voice <<= 4;
    voice += (unsigned long)_spu_RXX;
    *env = *(unsigned short *)(voice + 0xC);
}

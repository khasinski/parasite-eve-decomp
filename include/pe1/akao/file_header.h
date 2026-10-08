#ifndef PE1_AKAO_FILE_HEADER_H
#define PE1_AKAO_FILE_HEADER_H

/* Shared prefix of the sample and command buffers accepted by the validator.
 * Each buffer kind defines the remaining header and payload independently. */
typedef struct AkaoFilePrefix {
    unsigned int magic;
    unsigned char contents[0];
} AkaoFilePrefix;

typedef char akao_file_prefix_size[(sizeof(AkaoFilePrefix) == 4) ? 1 : -1];
typedef char akao_file_prefix_contents[
    ((unsigned int)&((AkaoFilePrefix *)0)->contents == 4) ? 1 : -1];

int Spu_ValidateSampleHeader(void *header);

#endif

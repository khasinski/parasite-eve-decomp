#ifndef PE1_CDROM_CALLBACK_INTERNAL_H
#define PE1_CDROM_CALLBACK_INTERNAL_H

typedef struct DsCallbackSetPage {
    char reserved00[0x36A0];
    unsigned int poll;
    unsigned int sync;
    unsigned int ready;
    unsigned int dispatch;
} DsCallbackSetPage;

register DsCallbackSetPage *g_DsCallbackSetPage asm("$1");

#endif

#ifndef ROOM_M273_H
#define ROOM_M273_H

#include "pe1/render_object.h"

typedef GteShortVector RoomM273Vector;

typedef struct {
    signed int low : 16;
    signed int high : 16;
} RoomM273TrigEntry;

extern RoomM273TrigEntry D_800966EC[];
extern u16 D_800942EC;
extern char D_8019AB70[];
extern char D_8019ACC8[];
extern char D_8019ACCC[];

#endif

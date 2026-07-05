#include "../room_lib/room_lib.h"

extern RoomBlob8 RoomM272_BlobA;
extern RoomBlob8 RoomM272_BlobB;
extern RoomBlob8 RoomM272_BlobC;
extern char RoomM272_Rec[];
extern int D_801991C8;
extern char D_800BCE80[];
extern int D_8009CDDC;
extern int func_800750CC();
extern int func_8007506C();
extern int func_8007512C();
extern int func_80074DC0();
extern int func_80073A44();
extern int func_800755F0();
extern int func_80074D28();

int RoomM272_Cinematic_8018F014(void) {
    RoomBlob8 b1 = RoomM272_BlobA;
    RoomBlob8 b2 = RoomM272_BlobB;
    RoomBlob8 b3 = RoomM272_BlobC;

    if (D_801991C8 == 0) {
        func_800750CC(&b1, RoomM272_Rec);
        func_8007512C(&b2, 0x300, 0x100);
        func_80074DC0(0);
    }
    if (D_801991C8 == 1) {
        func_8007506C(&b1, RoomM272_Rec);
        func_8007512C(&b3, 0x140, 0x100);
        func_80074DC0(0);
        func_80073A44(2);
        func_800755F0(&D_800BCE80[D_8009CDDC * 20]);
        func_80074D28(1);
    }
    return 0;
}

int RoomM272_SetPhase_8018F178(int a, int b, int c, int d) {
    D_801991C8 = d;
    return 0;
}

int RoomM272_Nop_8018F188(void) {
    return 0;
}

int RoomM272_Nop_8018F190(void) {
    return 0;
}

int RoomM272_Nop_8018F198(void) {
    return 0;
}

int RoomM272_Nop_8018F1A0(void) {
    return 0;
}

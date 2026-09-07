"""Host checks of CPU behavior; mocked GTE transfers do not emulate the GTE."""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

SOURCE = Path(__file__).with_name('RoomEffect_DirectedRings.c')


class DirectedRings(unittest.TestCase):
    def test_cpu_behavior(self):
        source = SOURCE.read_text().replace('#include "pe1/gte.h"', '')
        source = re.sub(r'asm\(""[^;]*;', '', source)
        source = re.sub(r'\s+asm\("\$\d+"\)', '', source)
        source = source.replace('extern unsigned char D_8019AF69,D_8019AF68;', '')
        source = source.replace('extern short D_8019AEFC,D_8019AEFE,D_8019AF00;', '')
        source = source.replace('extern unsigned short D_8019AF60;', '')
        source = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
/* One array represents the evidenced origin/heading/flag address window. */
static unsigned short storage[55];
#define D_8019AEFC (*(short *)&storage[0])
#define D_8019AEFE (*(short *)&storage[1])
#define D_8019AF00 (*(short *)&storage[2])
#define D_8019AF60 storage[50]
#define D_8019AF68 (((unsigned char *)storage)[108])
#define D_8019AF69 (((unsigned char *)storage)[109])
static int transfers;
#define gte_ldrotmatrix(p) assert((p)==currentMatrix)
#define gte_ldtransmatrix(p) assert((p)->t[0]==0 && (p)->t[1]==0 && (p)->t[2]==0)
#define gte_ldv0(p) assert((p)==&D_8019AD60)
#define gte_rtv0tr_mac() (transfers++)
#define gte_mfc2_9(x) ((x)=101)
#define gte_mfc2_10(x) ((x)=-202)
#define gte_mfc2_11(x) ((x)=303)
''' + source.replace('int func_801969D8(int mode)',
                       'static Matrix *currentMatrix;\nint func_801969D8(int mode)') + r'''
static Context context;
static StateContext stateContext;
static State state;
static Owner owner;
static Player player;
static Particle particles[10];
Context *D_800F33E0=&context;
StateContext *D_800F32D0=&stateContext;
Player *g_PlayerEntity=&player;
Vector D_8019AD60;
int D_800966EC[4096];
unsigned short D_800E11E8,D_800E2850[8];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static int allocations,used,sounds,mathCalls,angleResult;
static unsigned int failures,squared;
static int vertical;
static Vector rotations[10];
static int matrices;
int func_8019665C(void) { assert(0); return 0; }
int func_800CE560(void *pool,int stride,int count,int (*callback)()) {
    assert(pool==&context && stride==24 && count==40 && callback==func_8019665C);
    return 960;
}
Particle *func_800CE610(void *pool) {
    assert(pool==&context);
    if(failures & (1u<<allocations++)) return 0;
    assert(used<10);
    return &particles[used++];
}
int func_8005186C(int value) { squared=(unsigned int)value; mathCalls++; return 1234; }
int func_80079FB4(int y,int radius) {
    assert(radius==1234); vertical=y; mathCalls++; return angleResult;
}
void func_80079754(Vector *rotation,Matrix *matrix) {
    rotations[matrices++]=*rotation; currentMatrix=matrix;
}
void func_8006DCE4(int id,void *value,int x,int y,int z) {
    assert(id==0x5D1 && value==&owner);
    assert(x==-11 && y==22 && z==-33); sounds++;
}
static void reset(void) {
    memset(particles,0xA5,sizeof(particles));
    allocations=used=sounds=mathCalls=transfers=matrices=0;
    failures=0; D_8019AF69=0; D_8019AF68=1;
    D_8019AEFC=-11; D_8019AEFE=22; D_8019AF00=-33; D_8019AF60=700;
    player.x=111; player.y=-222; player.z=333;
}
static void check_particle(int n,int i,int ring,int angle) {
    unsigned int index=ring ? i*1024 : i*4096/6;
    short *wave=(short *)&D_800966EC[index];
    int width=(angle-256)/(ring ? 2 : 1);
    Particle *p=&particles[n];
    assert(p->rotation.x==(short)(angle+wave[1]*(ring ? 96 : 160)/4096));
    assert(p->rotation.y==(short)(wave[0]*width/4096+700+2048));
    assert(p->rotation.z==0 && p->rotation.w==1);
    assert(!memcmp(&rotations[n],&p->rotation,sizeof(Vector)));
    assert(p->position.x==-11 && p->position.y==22 && p->position.z==-33);
    assert(p->position.w==0 && p->velocity.w==0);
    assert(p->velocity.x==101 && p->velocity.y==-202 && p->velocity.z==303);
}
int main(void) {
    static int angles[]={-1,0,383,384,500,768,769,4095,4096};
    int a,first,second,i,n,angle;
    context.pool=&context; stateContext.state=&state; state.owner=&owner; owner.value=&owner;
    for(i=0;i<4096;i++) {
        short *wave=(short *)&D_800966EC[i];
        wave[0]=(i&1) ? -2345 : 1234; wave[1]=(i&2) ? -3456 : 4567;
    }
    for(a=0;a<(int)(sizeof(angles)/sizeof(*angles));a++)
    for(first=0;first<=6;first++) for(second=0;second<=4;second++) {
        reset(); angleResult=angles[a];
        if(first<6) failures|=1u<<first;
        if(second<4) failures|=1u<<(first+(first<6)+second);
        assert(func_801969D8(1)==0);
        assert(used==first+second && transfers==used && matrices==used && sounds==1);
        assert(allocations==used+(first<6)+(second<4));
        assert(mathCalls==2 && vertical==-244 && squared==122u*122u+366u*366u);
        angle=angleResult&4095; if(angle>768) angle=768; if(angle<384) angle=384;
        n=0;
        for(i=0;i<first;i++) check_particle(n++,i,0,angle);
        for(i=0;i<second;i++) check_particle(n++,i,1,angle);
    }
    reset(); D_8019AF69=255;
    assert(func_801969D8(1)==2 && mathCalls==0 && allocations==0 && sounds==0);
    reset(); D_8019AF68=0;
    assert(func_801969D8(1)==0 && mathCalls==2 && allocations==0 && sounds==0);
    /* Squared horizontal distance wraps at 32 bits, as in the retail MULTs. */
    reset(); D_8019AF68=0;
    D_8019AEFC=D_8019AF00=-32768; player.x=player.z=32767;
    func_801969D8(1); assert(squared==(uint32_t)(2ull*65535*65535));
    reset(); assert(func_801969D8(0)==960);
    D_800E11E8=3; D_800E2850[3]=0xABCD;
    assert(func_801969D8(2)==0);
    assert(D_800F3368==16 && D_800F336A==1 && D_800F336C==2 && D_800F336E==0);
    assert(D_800F3370==0xABCD && D_800F3372==0 && D_800F3374==0);
    assert(D_800F3376==16 && D_800F3378==16);
    assert(func_801969D8(-1)==0 && func_801969D8(3)==0);
    assert(allocations==0 && sounds==0 && mathCalls==0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='pe-directed-host-') as directory:
            path = Path(directory) / 'test.c'
            path.write_text(source)
            executable = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', '-fno-strict-aliasing',
                            str(path), '-o', str(executable)], check=True)
            subprocess.run([str(executable)], check=True)


if __name__ == '__main__':
    unittest.main()

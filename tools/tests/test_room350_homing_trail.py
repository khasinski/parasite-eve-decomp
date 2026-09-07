import pathlib
import re
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_HomingTrailController.c'


class HomingTrailTests(unittest.TestCase):
    def test_cpu_behavior(self):
        source = SOURCE.read_text().replace('#include "pe1/gte.h"', '')
        source = re.sub(r' asm\("\$\d+"\)', '', source)
        source = source.replace('(unsigned int)count', '(uintptr_t)count')
        source = source.replace('D_8019A7FE,D_8019A800,', '')
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
static union { uint64_t align; unsigned char bytes[256]; } pointerQueue,positionQueue;
#define D_8019A7FE (*(short *)(pointerQueue.bytes+0x56))
#define D_8019A800 (*(short *)(positionQueue.bytes+0x48))
static int gteCalls,draws;
static void check_vector(void *);
#define gte_ldrotmatrix(p) ((void)(p))
#define gte_ldtransmatrix(p) ((void)(p))
#define gte_ldv0(p) check_vector(p)
#define gte_rtv0tr_mac() (gteCalls++)
#define gte_mfc2_9(x) ((x)=11)
#define gte_mfc2_10(x) ((x)=22)
#define gte_mfc2_11(x) ((x)=33)
'''
        harness = prefix + source + r'''
Actor *D_800F32D0;
Instance *g_PlayerEntity;
int D_800E27EC,D_800966EC[4096],D_800F3428;
short D_800966EE[8192],D_8019A802;
unsigned short D_800F336C,D_800E1204[65536],D_800942EC;
char D_8019A3C0[4],D_8019A4E4[4],D_8019A3C8[4];
static Actor actor;
static Instance instance,player;
static Owner owner,playerOwner;
static Particle particle;
static int sqrtResult;
static int steering,expectedStep,expectedYaw,turnCalls;
static void check_vector(void *p) {
    Vector *v=p;
    assert(v->x==0 && v->y==0 && v->z==(short)(0u-(unsigned short)particle.speed));
    /* The fourth halfword is uninitialized in the target. */
}
int FieldEng_VecToAngle(void *a,void *b) { assert(a==player.position && b==instance.position); return 123; }
int FieldEng_TurnToward(int a,int b,int c) {
    assert(b==123); turnCalls++;
    if(steering) { assert(a==-321 && c==expectedStep); D_800E27EC=7; }
    return 456;
}
int Math_IntSqrt(int n) { return sqrtResult; }
void RotMatrixYXZ(Vector *v,Matrix *m) {
    assert(v==&particle.rotation);
    if(steering) assert(v->x==-1 && v->y==(short)expectedYaw && v->z==987);
    memset(m,0,sizeof(*m));
}
int GetClut(int x,int y) { assert(x==32 && y==104); return 0x12345678; }
void func_800CEE20(void *p,int a,int b,int c,int d,int e,int f,int g,int h) {
    assert(draws++==0 && p==&particle && a==0 && b==4096 && c==4096);
    assert(d==108 && e==0x5678 && f==1 && g==77 && h==0);
    particle.position.x=-5; particle.position.z=9; particle.brightness=88;
}
void func_800D004C(void *p,int a,int b,int c,void *d,int e,int f,void *g,void *h,int i,int j) {
    Vector *v=p;
    assert(draws++==1 && v->x==-5 && v->y==-123 && v->z==9);
    assert(a==192 && b==192 && c==8 && d==D_8019A3C0 && e==4096 && f==4096);
    assert(g==D_8019A4E4 && h==D_8019A3C8 && i==88 && j==1);
}
static void reset(void) {
    memset(&particle,0,sizeof(particle)); memset(&owner,0,sizeof(owner));
    memset(&pointerQueue,0,sizeof(pointerQueue)); memset(&positionQueue,0,sizeof(positionQueue));
    actor.instance=&instance; instance.owner=&owner; player.owner=&playerOwner;
    playerOwner.status=0; player.animation=4; D_800F32D0=&actor; g_PlayerEntity=&player;
    D_800E27EC=0; D_8019A802=9; gteCalls=draws=turnCalls=steering=0; sqrtResult=128;
}
int main(void) {
    int n,frame,hit,expected;
    for(n=0;n<65536;n++) {
        reset(); D_800E27EC=48; D_8019A802=n;
        assert(func_801947BC(1,&particle)==1);
        assert((unsigned short)D_8019A802==(unsigned short)(n-1) && gteCalls==0 && D_8019A800==0);
    }
    for(frame=-32768;frame<=32767;frame++) {
        reset(); particle.frame=frame; particle.hit=1; particle.position.pad=321;
        assert(func_801947BC(1,&particle)==0 && gteCalls==0);
        assert(particle.frame==(frame<48 ? (short)(frame+1):frame));
        assert(D_8019A800==(frame<48));
        expected=frame>=36 && frame<48 && !(frame&3);
        assert(D_8019A7FE==expected);
        if(frame<48) assert(!memcmp(positionQueue.bytes,&particle.position,8));
    }
    for(n=0;n<=4;n++) for(hit=0;hit<2;hit++) {
        reset(); particle.frame=0; particle.rotation.x=64; particle.speed=100;
        D_8019A7FE=n; sqrtResult=hit ? 127:128;
        assert(func_801947BC(1,&particle)==0 && gteCalls==1);
        assert(particle.hit==hit && particle.frame==(hit ? 41:1));
        assert(particle.speed==(hit ? 0:100));
        assert(owner.flags==(hit ? 0x80000000u:0) && playerOwner.status==(hit ? 0x4000:0));
        expected=hit ? (n+2<4 ? n+2:4):n;
        assert(D_8019A7FE==expected && D_8019A800==1);
        for(frame=n;frame<expected;frame++) assert(((Particle **)pointerQueue.bytes)[frame]==&particle);
    }
    for(n=-32768;n<=32767;n++) {
        int second=(short)(n*13);
        reset(); steering=1; player.animation=0;
        particle.rotation.x=-1; particle.rotation.y=-321; particle.rotation.z=987;
        particle.speed=n; D_800966EC[0]=(unsigned short)n;
        D_800966EC[2055]=(unsigned short)second;
        expectedStep=n/4; expectedYaw=456+80-(second*160)/4096;
        assert(func_801947BC(1,&particle)==0 && gteCalls==1 && turnCalls==1);
        assert(D_800E27EC==7 && particle.frame==1 && particle.hit==0);
    }
    reset(); particle.brightness=77; D_800F336C=4; D_800F3428=1;
    D_800E1204[4]=100; D_800942EC=(unsigned short)-123;
    assert(func_801947BC(2,&particle)==0 && draws==2);
    reset(); particle.frame=48; assert(func_801947BC(2,&particle)==0 && draws==0);
    for(n=-3;n<=4;n++) if(n!=1 && n!=2) {
        reset(); assert(func_801947BC(n,&particle)==0 && gteCalls==0 && draws==0 && D_8019A802==9);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'homing'
            result = subprocess.run(['cc','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-x','c','-','-o',str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

    def test_psx_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Vector)==8 ? 1:-1];
typedef char b[sizeof(Matrix)==32 ? 1:-1];
typedef char c[sizeof(Particle)==28 ? 1:-1];
typedef char d[OFF(Particle,rotation)==8 ? 1:-1];
typedef char e[OFF(Particle,speed)==16 ? 1:-1];
typedef char f[OFF(Particle,brightness)==20 ? 1:-1];
typedef char g[OFF(Particle,frame)==22 ? 1:-1];
typedef char h[OFF(Particle,hit)==24 ? 1:-1];
typedef char i[OFF(Owner,status)==0x4C ? 1:-1];
typedef char j[OFF(Instance,animation)==14 ? 1:-1];
typedef char k[OFF(Instance,position)==0x1FC ? 1:-1];
typedef char l[OFF(Actor,instance)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run(
                [str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))],
                cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)


if __name__ == '__main__':
    unittest.main()

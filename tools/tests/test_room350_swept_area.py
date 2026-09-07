import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT/'src/overlays/room_m350/RoomEffect_SweptAreaEmitter.c'


class SweptAreaBehavior(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Vector)==8 && OFF(Vector,z)==4 ? 1:-1];
typedef char b[sizeof(Matrix)==32 && OFF(Matrix,position)==20 ? 1:-1];
typedef char c[OFF(Actor,instance)==8 && OFF(Emitter,pool)==8 ? 1:-1];
typedef char d[OFF(Instance,animation)==0xE && OFF(Instance,frame)==0x16 ? 1:-1];
typedef char e[OFF(Instance,transform)==0x1E8 && OFF(Instance,transforms)==0x238 ? 1:-1];
typedef char f[OFF(Owner,flags)==0 && OFF(Owner,status)==0x4C ? 1:-1];
typedef char g[OFF(Particle,y)==0 && OFF(Particle,z)==2 && OFF(Particle,shade)==4 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(path),str(path.with_suffix('.o'))],cwd=ROOT,capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = SOURCE.read_text()
        source = source.replace('#include "pe1/gte.h"', '')
        source = re.sub(r' asm\("\$\d+"\)', '', source)
        source = re.sub(r'extern volatile short (rightX|leftX) asm\([^;]*;', '', source)
        source = re.sub(r'extern volatile short D_8019A89C[^;]*;', '', source)
        source = source.replace('extern short D_8019A8AE;', '')
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
static void screen(int,int),clip(int *);
#define gte_ldsxy0(x) screen(0,x)
#define gte_ldsxy1(x) screen(1,x)
#define gte_ldsxy2(x) screen(2,x)
#define gte_nclip() ((void)0)
#define gte_stmac0(p) clip(p)
#define rightX (*(volatile short *)&D_8019A898[0])
#define leftX (*(volatile short *)&D_8019A8A0[0])
#define D_8019A89C (*(volatile short *)&D_8019A898[2])
#define D_8019A89E (*(volatile short *)&D_8019A898[3])
#define D_8019A8A4 (*(volatile short *)&D_8019A8A0[2])
#define D_8019A8AE D_8019A8A8.pad
'''
        body = r'''
Actor *D_800F32D0;
Emitter *D_800F33E0;
Instance *g_PlayerEntity;
int D_800E27EC;
unsigned char D_8019A8C0;
short D_8019A898[4],D_8019A8A0[4];
Vector D_8019A8A8,D_8019A8B0;
short D_8019A890[3];
short D_800F3368,D_800F336A,D_800F3376,D_800F3378;
short D_800F336C,D_800F336E,D_800F3372,D_800F3374,D_800F3370;
unsigned short D_800E11EA,D_800E2850[65536];
static Actor actors[2];
static Instance instances[2],players[2];
static Owner owners[3];
static Emitter emitter;
static Matrix transforms[16];
static Particle particle;
static Vector oldRight,oldLeft,quad[4];
static int allocations,randoms,matrices,clips,screens,randomValue,failAlloc,outsideAt,mutate;
static int initCalls,packed[3],expectedPoint;
static int word(uint32_t u) { int x; memcpy(&x,&u,4); return x; }
static int half(unsigned u) { return (u&65535)<32768 ? (int)(u&65535):(int)(u&65535)-65536; }
static int xz(int x,int z) { return word(((uint32_t)z<<16)|(uint16_t)x); }
int func_801981D0(int event,Particle *p) { (void)event;(void)p; assert(0); return 0; }
int func_800CE560(void *pool,int stride,int count,int (*callback)(int,Particle *)) {
    assert(++initCalls==1 && pool==&particle && stride==8 && count==4 && callback==func_801981D0);
    return -17;
}
Particle *func_800CE610(void *pool) {
    assert(++allocations==1 && pool==&particle);
    if (mutate) D_800F32D0=&actors[1];
    return failAlloc ? 0:&particle;
}
int Inv_ScrambleGrid(void) { assert(++randoms==1 && allocations==1); return randomValue; }
void ApplyMatrixSV(Matrix *m,Vector *in,Vector *out) {
    assert(++matrices==1 && m==&instances[0].transform && in==out);
    assert(in->x==256 && in->y==0 && in->z==0);
    assert(!memcmp(&D_8019A8A8,&oldRight,8) && !memcmp(&D_8019A8B0,&oldLeft,8));
    in->x=-32768; in->y=123; in->z=-1;
    if(mutate) {
        D_800F32D0=&actors[1];
        instances[0].transform.position[0]=INT_MAX;
        instances[0].transform.position[2]=INT_MIN;
    }
    quad[0]=oldRight; quad[1]=oldLeft; quad[2]=oldLeft; quad[3]=oldRight;
    quad[0].x=half((unsigned)instances[0].transform.position[0]+32768u);
    quad[0].z=half((unsigned)instances[0].transform.position[2]+65535u);
    quad[0].pad=1;
    quad[1].x=half((unsigned)instances[0].transform.position[0]-32768u);
    quad[1].z=half((unsigned)instances[0].transform.position[2]-65535u);
}
static void screen(int slot,int value) {
    static const int order[3]={0,2,1};
    assert(slot==order[screens%3]); screens++;
    packed[slot]=value;
}
static void clip(int *area) {
    int previous=clips ? clips-1:3;
    assert(screens==3*(clips+1));
    assert(packed[0]==expectedPoint);
    assert(packed[1]==xz(quad[clips].x,quad[clips].z));
    assert(packed[2]==xz(quad[previous].x,quad[previous].z));
    *area=clips==outsideAt ? -1:0;
    if(mutate) g_PlayerEntity=&players[1];
    clips++;
}
static void reset(void) {
    int i;
    memset(instances,0,sizeof(instances)); memset(players,0,sizeof(players));
    memset(owners,0,sizeof(owners)); memset(transforms,0,sizeof(transforms));
    for(i=0;i<2;i++) { actors[i].instance=&instances[i]; players[i].owner=&owners[i+1]; }
    instances[0].owner=&owners[0]; instances[0].transforms=transforms;
    instances[0].animation=9; instances[0].frame=10;
    instances[0].transform.position[0]=123456; instances[0].transform.position[2]=-765432;
    players[0].transform.position[0]=123; players[0].transform.position[2]=-456;
    expectedPoint=xz(123,-456);
    emitter.pool=&particle; D_800F33E0=&emitter; D_800F32D0=&actors[0]; g_PlayerEntity=&players[0];
    oldRight=(Vector){100,222,300,1}; oldLeft=(Vector){-100,333,-300,444};
    memcpy((void *)D_8019A898,&oldRight,8); memcpy((void *)D_8019A8A0,&oldLeft,8);
    memset(&D_8019A8A8,0x55,8); memset(&D_8019A8B0,0x66,8);
    D_800E27EC=0; D_8019A8C0=0; particle=(Particle){-7,-8,999};
    initCalls=allocations=randoms=matrices=clips=screens=failAlloc=mutate=0; outsideAt=-1; randomValue=-1;
}
int main(void) {
    int i,j,k;
    reset(); assert(func_80198400(0)==-17 && initCalls==1 && !allocations && !matrices);
    reset(); assert(func_80198400(-1)==0 && !allocations && !matrices);
    for(i=31;i<65536;i++) {
        reset(); instances[0].frame=i;
        assert(func_80198400(1)==2 && !allocations && !matrices && !clips);
    }
    for(i=0;i<256;i++) {
        reset(); randomValue=word(0xFEDC0000u+(unsigned)i); D_8019A8C0=1;
        assert(func_80198400(1)==0 && allocations==1 && randoms==1 && !matrices);
        assert(particle.y==i && particle.z==64+i/4 && particle.shade==999);
    }
    for(i=-1;i<4;i++) for(j=0;j<2;j++) for(k=0;k<2;k++) {
        reset(); outsideAt=i; mutate=j; if(k) instances[0].owner=0;
        assert(func_80198400(1)==0 && allocations==1 && randoms==1 && matrices==1);
        assert(clips==(i<0 ? 4:i+1));
        assert(!memcmp((const void *)D_8019A898,&quad[0],8));
        assert(!memcmp((const void *)D_8019A8A0,&quad[1],8));
        assert(D_8019A8C0==(i<0));
        assert(owners[j+1].status==(i<0 ? 0x4000u:0));
        assert(owners[0].flags==(i<0 && !k ? 0x80000000u:0));
    }
    reset(); failAlloc=1;
    assert(func_80198400(1)==0 && allocations==1 && !randoms && matrices==1 && clips==4);
    reset(); D_800E27EC=-1; instances[0].animation=8;
    assert(func_80198400(1)==0 && !allocations && !matrices);
    reset(); oldRight.pad=0; memcpy((void *)D_8019A898,&oldRight,8);
    assert(func_80198400(1)==0 && matrices==1 && !clips && !D_8019A8C0 && D_8019A89E==1);
    reset(); D_800E11EA=65535; D_800E2850[65535]=0xFEDC;
    for(i=0;i<3;i++) { transforms[11].position[i]=-3-i; transforms[15].position[i]=i; }
    assert(func_80198400(2)==0 && !allocations && !matrices);
    for(i=0;i<3;i++) assert(D_8019A890[i]==-2);
    assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
    assert(D_800F336C==3 && !D_800F336E && !D_800F3372 && !D_800F3374 && (unsigned short)D_800F3370==0xFEDC);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'test.c'
            path.write_text(prefix+source+body)
            executable = path.with_suffix('')
            result = subprocess.run(['cc','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing',str(path),'-o',str(executable)],capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            result = subprocess.run([str(executable)],capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)


if __name__ == '__main__':
    unittest.main()

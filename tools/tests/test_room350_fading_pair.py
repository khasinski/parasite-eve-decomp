import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_FadingPairEmitter.c'


class FadingPairTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(State)==16 && OFF(State,count)==8 && OFF(State,brightness)==10 ? 1:-1];
typedef char b[OFF(State,fade)==12 && OFF(State,size)==14 ? 1:-1];
typedef char c[sizeof(Particle)==8 && OFF(Particle,size)==6 ? 1:-1];
typedef char d[sizeof(Vector)==8 && sizeof(Matrix)==32 && OFF(Matrix,position)==20 ? 1:-1];
typedef char e[OFF(Instance,transforms)==0x238 && OFF(Instance,owner)==0 ? 1:-1];
typedef char f[OFF(Actor,instance)==8 && OFF(Owner,asset)==8 && OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_setup_update_and_draw(self):
        source = re.sub(r' asm\("\$\d+"\)', '', SOURCE.read_text())
        # Remove only MIPS register constraints; matrix helpers are boundaries,
        # not GTE emulation. Never inspect uninitialized vector/matrix padding.
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
''' + source + r'''
Actor *D_800F32D0;
Emitter *D_800F33E0;
unsigned char D_8019A82E;
int D_800E27EC,D_800966EC[4096],D_800F3428;
unsigned short D_800E2850[65536],D_800E1204[65536];
volatile unsigned short D_800E11E8,D_800F3370;
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F336C,D_800F336E,D_800F3372,D_800F3374;
char D_8019A570[8];
static Actor actors[2];
static Instance instances[2];
static Owner owners[2];
static Emitter emitters[2];
static Matrix transforms[2][34],*activeMatrix;
static Vector *activeVector;
static int pools[2],assets[2],allocs,randoms,rotations,applies,sounds,cluts,draws;
static int failAt,mutate,nextKind,special,initialTimer,expectedRotation;
static State state,expected;
static Particle particles[2],expectedParticles[2];
static uint32_t seed;
static int word(uint32_t x) { int r; memcpy(&r,&x,4); return r; }
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
static int shift5(int x) { return x>=0 ? x/32:-((-x+31)/32); }
static int rng(int i) { return word(seed+(uint32_t)i*0x87654321u); }
int func_80195B64(int e,Particle *p) { return 0; }
int Asset_Find08w(int id,void *asset,int x,int y,int z) {
    assert(sounds++==0 && id==0x5C9 && asset==owners[0].asset);
    assert(x==half(transforms[0][33].position[0]));
    assert(y==half(transforms[0][33].position[1]) && z==half(transforms[0][33].position[2]));
    D_800F33E0=&emitters[1]; memset(&state,0x39,sizeof(state));
    return -999;
}
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    State wanted;
    memset(&wanted,0x39,sizeof(wanted)); memset(&wanted.rotation,0,sizeof(wanted.rotation));
    wanted.count=0; wanted.fade=0;
    assert(memcmp(&state,&wanted,sizeof(state))==0);
    assert(sounds==1 && p==&pools[1] && stride==8 && count==18 && cb==func_80195B64);
    return -17;
}
Particle *func_800CE610(void *p) {
    int i=allocs++;
    assert(i<2 && p==&pools[mutate && i ? 1:0]);
    assert(randoms==i*4 && applies==i && rotations==i);
    assert(state.brightness==expected.brightness && state.size==expected.size);
    if(mutate) state.count=expected.count=half(0xFFFFu+i);
    return i==failAt ? 0:&particles[i];
}
int Inv_ScrambleGrid(void) {
    int i=randoms++;
    assert(i<8 && allocs==i/4+1);
    if(i%4==2) assert(activeVector->x==0 && activeVector->y==0);
    return rng(i);
}
void RotMatrixYXZ(Vector *v,Matrix *m) {
    int i=rotations++;
    assert(randoms==i*4+2 && applies==i);
    assert(v->x==half((uint32_t)rng(i*4)<<4));
    assert(v->y==half((uint32_t)rng(i*4+1)<<4) && v->z==0);
    activeMatrix=m; activeVector=v;
}
void ApplyMatrixSV(Matrix *m,Vector *v,void *out) {
    int i=applies++,combined=word(((uint32_t)rng(i*4+2)<<8)|(uint32_t)rng(i*4+3));
    Particle *p=out;
    assert(randoms==i*4+4 && rotations==i+1 && out==&particles[i]);
    assert(m==activeMatrix && v==activeVector && v->x==0 && v->y==0 && v->z==combined%44+320);
    p->x=expectedParticles[i].x=-32768+i;
    p->y=expectedParticles[i].y=32767-i;
    p->z=expectedParticles[i].z=-1234+i;
    if(mutate) { state.count=expected.count=i ? -32768:32767; D_800F33E0=&emitters[1]; }
    expectedParticles[i].size=half((uint32_t)state.count*128+2048);
}
int GetClut(int x,int y) {
    int i=cluts++,kind=i ? nextKind:2;
    assert(i<2 && draws==i && x==32);
    assert(y==D_800E1204[kind]+(kind==4 && special ? 4:0));
    assert(state.rotation.z==expectedRotation);
    state.size=i ? 32767:-32768; state.brightness=i ? -1:1023;
    D_800F336A=i ? -32768:32767;
    return word(0xABCDFFFFu+i);
}
void func_800CEE20(void *pos,void *rotation,int sx,int sy,int scale,int clut,int flag,int brightness,void *colors) {
    Vector *v=pos;
    int i=draws++;
    assert(i<2 && cluts==i+1 && rotation==&state && flag==1 && colors==D_8019A570);
    assert(v->x==half(0xABCDEF01u) && v->y==half(0x76543210u) && v->z==-32768);
    assert(sx==(i ? 32767:-32768) && sy==sx && brightness==(i ? -1:1023));
    assert(scale==(i ? -32768:32767)+216 && clut==(i ? 0:65535));
    assert(state.rotation.z==expectedRotation);
    state.rotation.z=i ? -32768:12345;
    expectedRotation=half(0u-(uint16_t)state.rotation.z);
    D_800F336C=nextKind; D_800F3428=special; D_800E1204[nextKind]=65535;
    D_800F32D0=&actors[1]; transforms[0][33].position[0]=55; D_800E27EC=999;
}
static void reset(void) {
    allocs=randoms=rotations=applies=sounds=cluts=draws=0;
    D_800F32D0=&actors[0]; D_800F33E0=&emitters[0];
    memset(&state,0x5A,sizeof(state)); memset(particles,0x5A,sizeof(particles));
    memcpy(expectedParticles,particles,sizeof(particles));
    transforms[0][33].position[0]=word(0xABCDEF01u);
    transforms[0][33].position[1]=0x76543210;
    transforms[0][33].position[2]=word(0xFFFF8000u);
}
static void update(int count,int fade,int timer,int stop,int sample,int failure,int mutation) {
    int phase,expired,emits,index;
    reset(); state.count=count; state.fade=fade; expected=state;
    D_800E27EC=timer; D_8019A82E=stop; failAt=failure; mutate=mutation;
    phase=stop ? fade*64+1024:timer<32 ? word((uint32_t)timer<<5):1024;
    expired=stop && phase>=2048; emits=!expired && count<16;
    if(stop) expected.fade=half((uint16_t)fade+1);
    index=(uint32_t)phase&4095; D_800966EC[index]=sample;
    if(!expired) { expected.brightness=shift5(half(sample)); expected.size=half((uint32_t)sample<<2); }
    assert(func_80195CE0(1,&state)==(expired ? 2:0));
    if(emits) expected.count=half((uint16_t)expected.count+1);
    assert(memcmp(&state,&expected,sizeof(state))==0);
    assert(allocs==(emits ? (failure==0 ? 1:2):0));
    assert(applies==(emits ? (failure<0 ? 2:failure):0));
    assert(rotations==applies && randoms==applies*4 && sounds==0 && draws==0);
    assert(memcmp(particles,expectedParticles,sizeof(particles))==0);
}
int main(void) {
    int n,j,k;
    const int timers[]={INT_MIN,-65537,-1,0,1,31,32,33,INT_MAX};
    const int kinds[]={0,2,4,32768,65535};
    for(n=0;n<2;n++) {
        actors[n].instance=&instances[n]; instances[n].owner=&owners[n];
        instances[n].transforms=transforms[n]; owners[n].asset=&assets[n]; emitters[n].pool=&pools[n];
    }
    reset(); assert(func_80195CE0(0,&state)==-17);
    for(n=0;n<65536;n++) {
        seed=(uint32_t)n*65537;
        update(16,half(n),0,1,word(seed),-1,0);
        update(16,7,timers[n%9],0,word(seed),-1,0);
        for(j=-1;j<2;j++) update(half(n),-1,-123,0,word(seed),j,0);
        update(0,-3,31,0,word(seed),n%3-1,1);
    }
    for(n=1;n<256;n++) update(0,15,0,n,INT_MIN,-1,0);
    for(n=0;n<65536;n++) {
        reset(); D_800E11E8=n; D_800E2850[n]=n^0xA537;
        initialTimer=word((uint32_t)n*65537); D_800E27EC=initialTimer;
        expectedRotation=half((uint32_t)initialTimer<<8);
        nextKind=kinds[n%5]; special=n&1; D_800F3428=special;
        assert(func_80195CE0(2,&state)==0 && draws==2 && cluts==2);
        assert(state.rotation.z==expectedRotation && state.rotation.x==0x5A5A && state.rotation.y==0x5A5A);
        assert(D_800F3370==(unsigned short)(n^0xA537));
        assert(D_800F3368==16 && D_800F3376==16 && D_800F3378==16);
        assert(D_800F336E==0 && D_800F3372==0 && D_800F3374==0);
        assert(allocs==0 && sounds==0);
    }
    reset(); expected=state;
    assert(func_80195CE0(-1,&state)==0 && func_80195CE0(3,&state)==0);
    assert(func_80195CE0(INT_MIN,0)==0 && func_80195CE0(INT_MAX,0)==0);
    assert(memcmp(&state,&expected,sizeof(state))==0 && sounds==0 && allocs==0 && draws==0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'test.c'
            path.write_text(harness)
            binary = path.with_suffix('')
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', str(path), '-o', str(binary)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
            subprocess.run([str(binary)], check=True)

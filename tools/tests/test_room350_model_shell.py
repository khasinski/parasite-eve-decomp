import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_ModelShellLayers.c'


def spawner_source():
    source = SOURCE.read_text()
    callback = source.index('int func_8019404C')
    spawner = source.index('int func_8019421C')
    return source[:callback] + 'extern int func_8019404C(int, Particle *);\n' + source[spawner:]


class ModelShellTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Emitter *D_800F33E0', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Vector)==8 ? 1:-1];
typedef char b[sizeof(Matrix)==32 ? 1:-1];
typedef char c[OFF(Matrix,position)==20 ? 1:-1];
typedef char d[sizeof(Particle)==12 ? 1:-1];
typedef char e[OFF(Particle,x)==4 ? 1:-1];
typedef char f[OFF(Particle,size)==10 ? 1:-1];
typedef char g[OFF(Instance,transforms)==0x238 ? 1:-1];
typedef char h[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True, errors='replace')
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_rng_transform_boundaries_and_reload(self):
        source = spawner_source().replace(' asm("$16")', '')
        # Helper stubs check CPU behavior, not rotation/GTE arithmetic.
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
Emitter *D_800F33E0;
Instance *D_8019A7F8;
int D_800E27EC,D_800966EC[4096];
volatile short D_800F3368,D_800F3376,D_800F3378,D_800F336E,D_800F3372,D_800F3374;
short D_800F336A;
unsigned short D_800F336C;
volatile unsigned short D_800E11E8,D_800F3370;
unsigned short D_800E2850[65536];
static Emitter emitters[2];
static Instance instances[2];
static Matrix transforms[2][16],*rotationMatrix;
static Particle particles[2],expected[2];
static int pools[2],allocs,randoms,rotations,applications,setups,setupResult,failAt;
static uint32_t seed;
static int signed_word(uint32_t word) { int result; memcpy(&result,&word,4); return result; }
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
static int asr1(uint32_t x) { return (int)((int64_t)signed_word(x)/2-(signed_word(x)<0 && (x&1))); }
static uint32_t rng(int call) { return seed+(uint32_t)call*0x1234567u; }
int func_8019404C(int e,Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==12 && count==64 && cb==func_8019404C);
    setups++; return setupResult;
}
Particle *func_800CE610(void *p) {
    int i=allocs++;
    assert(i<2 && p==&pools[i] && randoms==i*7 && applications==i);
    if(i==failAt) return 0;
    return &particles[i];
}
int Inv_ScrambleGrid(void) {
    int call=randoms++,i=call/7,part=call%7,j;
    assert(allocs==i+1);
    if(part>=4) {
        assert(applications==i+1 && rotations==i+1);
        for(j=0;j<3;j++) assert((&particles[i].x)[j]==(&expected[i].x)[j]);
        assert(particles[i].size==expected[i].size);
        D_800E27EC=INT_MAX;
        if(part==6) expected[i].color=signed_word((uint32_t)asr1(rng(i*7+4)) |
            ((uint32_t)asr1(rng(i*7+5))<<8) | ((uint32_t)asr1(rng(i*7+6))<<16));
    }
    return signed_word(rng(call));
}
void RotMatrixYXZ(Vector *v,Matrix *m) {
    int i=rotations++;
    assert(randoms==i*7+2 && applications==i);
    assert(v->x==half(rng(i*7)<<4) && v->y==half(rng(i*7+1)<<4) && v->z==0);
    rotationMatrix=m; memset(m,0xB7,sizeof(*m));
}
void ApplyMatrixSV(Matrix *m,Vector *v,Vector *out) {
    int i=applications++,j,timer,index;
    int combined=signed_word((rng(i*7+2)<<8)|rng(i*7+3));
    uint32_t tableWord=0x7FFFFFF0u+(seed&65535);
    assert(randoms==i*7+4 && rotations==i+1 && m==rotationMatrix);
    assert(v->x==0 && v->y==0 && v->z==combined%192+128);
    out->x=half(seed); out->y=half(seed*7); out->z=half(seed*31); out->pad=123;
    D_8019A7F8=&instances[1]; D_800F33E0=&emitters[1];
    D_800E27EC=signed_word(seed+(uint32_t)i*0x13579u);
    timer=signed_word((uint32_t)D_800E27EC<<11); index=(timer/40)&4095;
    D_800966EC[index]=signed_word(tableWord); expected[i].size=half(tableWord+4096);
    for(j=0;j<3;j++) (&expected[i].x)[j]=half((uint32_t)((short *)out)[j]+
        (uint32_t)transforms[1][i ? 11:15].position[j]);
}
static void reset(void) {
    allocs=randoms=rotations=applications=setups=0;
    D_800F33E0=&emitters[0]; D_8019A7F8=&instances[0]; D_800E27EC=40;
    memset(particles,0x5A,sizeof(particles)); memcpy(expected,particles,sizeof(particles));
}
int main(void) {
    int i,j,k,f,event,edge,timers[]={INT_MIN,-1,0,40,41,INT_MAX};
    emitters[0].pool=&pools[0]; emitters[1].pool=&pools[1];
    instances[0].transforms=transforms[0]; instances[1].transforms=transforms[1];
    for(i=0;i<2;i++) for(j=0;j<16;j++) for(k=0;k<3;k++)
        transforms[i][j].position[k]=signed_word(0x7FFF8000u+i*0x3456u+j*123u+k*17u);
    for(i=0;i<65536;i++) for(f=0;f<3;f++) {
        reset(); failAt=f; seed=0xFEDC0000u|i;
        assert(func_8019421C(1)==0 && setups==0);
        assert(allocs==(f<2 ? f+1:2) && randoms==f*7 && rotations==f && applications==f);
        assert(memcmp(particles,expected,sizeof(particles))==0);
    }
    for(i=0;i<65536;i++) {
        reset(); failAt=0; D_800E27EC=i-32768;
        assert(func_8019421C(1)==(D_800E27EC>=41 ? 2:0));
        assert(allocs==(D_800E27EC<41) && !randoms);
    }
    for(edge=0;edge<6;edge++) {
        reset(); failAt=2; D_800E27EC=timers[edge]; seed=(uint32_t)timers[edge];
        assert(func_8019421C(1)==(timers[edge]>=41 ? 2:0));
        assert(allocs==(timers[edge]>=41 ? 0:2));
        assert(memcmp(particles,expected,sizeof(particles))==0);
    }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); assert(func_8019421C(0)==setupResult && setups==1 && !allocs);
    }
    for(i=0;i<65536;i++) {
        reset(); D_800E11E8=i; D_800E2850[i]=65535-i;
        assert(func_8019421C(2)==0 && !allocs && !setups);
        assert(D_800F3368==16 && D_800F336A==1 && D_800F3376==16 && D_800F3378==16);
        assert(D_800F336C==2 && D_800F336E==0 && D_800F3372==0 && D_800F3374==0 && D_800F3370==65535-i);
    }
    for(event=-3;event<=5;event++) if(event<0 || event>2) {
        reset(); assert(func_8019421C(event)==0 && !allocs && !setups && !randoms);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'shell'
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', '-x', 'c', '-', '-o', str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

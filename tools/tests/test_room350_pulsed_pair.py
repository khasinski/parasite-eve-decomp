import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_PulsedPairSpawner.c'


class PulsedPairTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Emitter', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Vector)==8 ? 1:-1];
typedef char b[sizeof(Matrix)==32 && OFF(Matrix,position)==20 ? 1:-1];
typedef char c[sizeof(Particle)==8 && OFF(Particle,reserved)==6 ? 1:-1];
typedef char d[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_emission_count_rng_and_reload(self):
        source = SOURCE.read_text().replace(' asm("$16")', '')
        # Stubs validate CPU-side helper contracts, not rotation/GTE arithmetic.
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
Emitter *D_800F33E0;
Vector D_8019A778[2];
int D_800E27EC;
short D_800966EE[8192];
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F336C,D_800F336E,D_800F3372,D_800F3374;
volatile unsigned short D_800E11E8,D_800F3370;
unsigned short D_800E2850[65536];
static Emitter emitters[2];
static int pools[2],allocs,randoms,rotations,applications,failAt;
static Particle particles[36],expected[36];
static Matrix *rotation;
static uint32_t seed;
static int word(uint32_t x) { int r; memcpy(&r,&x,4); return r; }
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
static uint32_t rng(int n) { return seed+(uint32_t)n*0x87654321u; }
int func_80199B94(int e,Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==8 && count==64 && cb==func_80199B94); return -123;
}
Particle *func_800CE610(void *p) {
    int i=allocs++;
    assert(i<36 && p==&pools[i&1] && randoms==i*4 && applications==i);
    D_800E27EC=INT_MAX;
    return i==failAt ? 0:&particles[i];
}
int Inv_ScrambleGrid(void) { return word(rng(randoms++)); }
void RotMatrixYXZ(Vector *v,Matrix *m) {
    int i=rotations++;
    assert(randoms==i*4+2 && applications==i && allocs==i+1);
    assert(v->x==half(rng(i*4)<<4) && v->y==half(rng(i*4+1)<<4) && v->z==0);
    rotation=m;
}
void ApplyMatrixSV(Matrix *m,Vector *v,Vector *out) {
    int i=applications++,j;
    int combined=word((rng(i*4+2)<<8)|rng(i*4+3));
    assert(m==rotation && randoms==i*4+4 && rotations==i+1);
    assert(v->x==0 && v->y==0 && v->z==combined%192+64);
    out->x=half(seed+i); out->y=half(seed*7+i); out->z=half(seed*31+i);
    for(j=0;j<3;j++) {
        ((short *)&D_8019A778[i&1])[j]=half(seed*13+i*257+j);
        expected[i].position[j]=half((uint16_t)((short *)out)[j]+(uint16_t)((short *)&D_8019A778[i&1])[j]);
    }
    D_800F33E0=&emitters[(i+1)&1];
    memset(D_800966EE,0,sizeof(D_800966EE));
}
static void reset(void) {
    allocs=randoms=rotations=applications=0; D_800F33E0=&emitters[0];
    memset(particles,0x5A,sizeof(particles)); memcpy(expected,particles,sizeof(particles));
}
static void run(int timer,int sample,int failure) {
    int count=((sample+4096)/2048+1)*2,success,wanted;
    reset(); failAt=failure; D_800E27EC=timer;
    D_800966EE[((((uint32_t)timer<<7)+2048)&4095)*2]=sample;
    assert(func_80192508(1)==(timer>=33 ? 2:0));
    if(timer>=33 || count<=0) wanted=success=0;
    else { success=(failure>=0 && failure<count) ? failure:count; wanted=success+(success<count); }
    assert(allocs==wanted && randoms==success*4 && applications==success && rotations==success);
    assert(memcmp(particles,expected,sizeof(particles))==0);
}
int main(void) {
    int n,j;
    static const int timers[]={INT_MIN,-65537,-32768,-33,-1,0,1,31,32,33,INT_MAX};
    for(n=0;n<2;n++) emitters[n].pool=&pools[n];
    reset(); assert(func_80192508(0)==-123);
    for(n=0;n<65536;n++) for(j=0;j<3;j++) {
        seed=(uint32_t)n*65537; run(-1,half(n),j-1);
    }
    for(n=0;n<36;n++) for(j=0;j<sizeof(timers)/sizeof(timers[0]);j++) {
        seed=(uint32_t)n*0x81234567u; run(timers[j],32767,n);
    }
    for(n=0;n<65536;n++) {
        seed=(uint32_t)n*0x81234567u; run(half(n),4096,-1);
        D_800E11E8=n; D_800E2850[n]=n^0xA357;
        assert(func_80192508(2)==0 && D_800F3370==(unsigned short)(n^0xA357));
        assert(D_800F3368==16 && D_800F336A==1 && D_800F3376==16 && D_800F3378==16);
        assert(D_800F336C==2 && D_800F336E==0 && D_800F3372==0 && D_800F3374==0);
    }
    reset(); assert(func_80192508(-1)==0 && func_80192508(3)==0 && allocs==0);
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

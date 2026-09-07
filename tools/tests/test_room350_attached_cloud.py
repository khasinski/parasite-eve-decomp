import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_AttachedCloudSpawner.c'


class AttachedCloudTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Vector)==8 ? 1:-1];
typedef char b[sizeof(Matrix)==32 ? 1:-1];
typedef char c[OFF(Matrix,position)==20 ? 1:-1];
typedef char d[sizeof(Particle)==20 ? 1:-1];
typedef char e[OFF(Particle,position)==4 ? 1:-1];
typedef char f[OFF(Particle,size)==10 ? 1:-1];
typedef char g[OFF(Instance,transforms)==0x238 ? 1:-1];
typedef char h[OFF(Actor,instance)==8 ? 1:-1];
typedef char i[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_helper_order_and_boundaries(self):
        source = SOURCE.read_text().replace(' asm("$16")', '').replace(' asm("$4")', '')
        # Transform stubs verify CPU-side contracts, not GTE arithmetic.
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n' + source + r'''
Actor *D_800F32D0;
Emitter *D_800F33E0;
unsigned char D_8019A8BE;
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F336C,D_800F336E,D_800F3372,D_800F3374;
volatile unsigned short D_800E11E8,D_800F3370;
unsigned short D_800E2850[65536];
static Actor actors[2];
static Instance instances[2];
static Emitter emitters[2];
static Matrix transforms[2][16],*rotation;
static Particle particles[2];
static int pools[2],allocs,randoms,rotations,applications,failAt;
static uint32_t seed;
static int word(uint32_t x) { int result; memcpy(&result,&x,4); return result; }
static int half(uint32_t x) { x &= 65535; return x<32768 ? (int)x:(int)x-65536; }
static uint32_t rng(int n) { return seed+(uint32_t)n*0x87654321u; }
int func_80198860(int e,Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==20 && count==16 && cb==func_80198860);
    return -123;
}
Particle *func_800CE610(void *p) {
    int i=allocs++;
    assert(i<2 && p==&pools[i] && randoms==i*5 && applications==i);
    D_8019A8BE=255;
    return i==failAt ? 0:&particles[i];
}
int Inv_ScrambleGrid(void) {
    int n=randoms++,i=n/5;
    assert(allocs==i+1);
    if(n%5==4) {
        assert(applications==i+1);
        D_800F32D0=&actors[1];
        D_800F33E0=&emitters[1];
    }
    return word(rng(n));
}
void RotMatrixYXZ(Vector *v,Matrix *m) {
    int i=rotations++;
    assert(randoms==i*5+2 && applications==i);
    assert(v->x==half(rng(i*5)<<4) && v->y==half(rng(i*5+1)<<4) && v->z==0);
    rotation=m;
}
void ApplyMatrixSV(Matrix *m,Vector *v,Vector *out) {
    int i=applications++;
    assert(m==rotation && rotations==i+1 && randoms==i*5+4);
    assert(v->x==0 && v->y==0 && v->z==word((rng(i*5+2)<<8)|rng(i*5+3))%128);
    assert(out==(Vector *)particles[i].position);
    out->x=half(seed); out->y=half(seed*7); out->z=half(seed*31);
}
static void reset(void) {
    allocs=randoms=rotations=applications=0;
    D_8019A8BE=0; D_800F32D0=&actors[0]; D_800F33E0=&emitters[0];
    memset(particles,0x5A,sizeof(particles));
}
int main(void) {
    int n,f,i,k,success;
    for(i=0;i<2;i++) {
        actors[i].instance=&instances[i]; instances[i].transforms=transforms[i];
        emitters[i].pool=&pools[i];
    }
    reset(); assert(func_80198ABC(0)==-123);
    for(n=1;n<256;n++) {
        reset(); D_8019A8BE=n;
        assert(func_80198ABC(1)==2 && allocs==0 && D_8019A8BE==n);
    }
    for(n=0;n<65536;n++) for(f=-1;f<2;f++) {
        reset(); seed=(uint32_t)n*65537u; failAt=f;
        assert(func_80198ABC(1)==0);
        success=f<0 ? 2:f;
        assert(allocs==(f<0 ? 2:f+1) && randoms==success*5);
        assert(rotations==success && applications==success && D_8019A8BE==255);
        for(i=0;i<2;i++) {
            if(i<success) {
                assert(particles[i].anchor==transforms[1][i ? 11:15].position);
                assert(particles[i].position[0]==half(seed));
                assert(particles[i].position[1]==half((uint32_t)(uint16_t)half(seed*7)*2+128));
                assert(particles[i].position[2]==half(seed*31));
                assert(particles[i].size==half((rng(i*5+4)<<4)+4096));
                for(k=0;k<2;k++) assert(particles[i].reserved[k]==0x5A5A5A5A);
            } else {
                unsigned char *p=(unsigned char *)&particles[i];
                for(k=0;k<sizeof(Particle);k++) assert(p[k]==0x5A);
            }
        }
    }
    for(n=0;n<65536;n++) {
        D_800E11E8=n; D_800E2850[n]=n^0xA35C;
        assert(func_80198ABC(2)==0 && D_800F3370==(unsigned short)(n^0xA35C));
        assert(D_800F3368==16 && D_800F336A==1 && D_800F3376==16 && D_800F3378==16);
        assert(D_800F336C==2 && D_800F336E==0 && D_800F3372==0 && D_800F3374==16);
    }
    reset(); assert(func_80198ABC(-1)==0 && func_80198ABC(3)==0 && allocs==0);
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

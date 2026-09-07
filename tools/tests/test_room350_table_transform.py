import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/overlays/room_m350/RoomEffect_TableTransformSpawner.c"


class TableTransformTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"), "PSX tools unavailable")
    def test_target_layout(self):
        source = SOURCE.read_text().split("extern Actor", 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==8 ? 1:-1];
typedef char b[sizeof(Entry)==2 ? 1:-1];
typedef char c[OFF(Instance,frame)==22 ? 1:-1];
typedef char d[OFF(Instance,transforms)==0x238 ? 1:-1];
typedef char e[sizeof(Transform)==32 ? 1:-1];
typedef char f[OFF(Transform,position)==20 ? 1:-1];
typedef char g[OFF(Actor,instance)==8 ? 1:-1];
typedef char h[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "layout.c"
            path.write_text(source)
            result = subprocess.run([str(ROOT / "tools/scripts/cc.sh"), str(path), str(path.with_suffix(".o"))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_table_and_post_allocation_reads(self):
        source = SOURCE.read_text().replace(' asm("$3")', '').replace(' asm("$2")', '')
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n' + source + r'''
Actor *D_800F32D0;
Instance *D_8019A7F8;
Emitter *D_800F33E0;
unsigned char D_8019A804;
Entry D_8019A4D4[26];
int D_800966EC[4096];
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336C,D_800F336E;
volatile unsigned short D_800E11FA,D_800F3370;
unsigned short D_800E2850[65536];
static Actor actors[2];
static Instance instances[3];
static Transform transforms[3][256];
static Emitter emitters[2];
static Particle particles[2],expected[2];
static int pools[2],calls,setups,setupResult,failAt,frame,indexAfter,alternate,sizeWord;
static int half(uint32_t x) { x &=65535; return x<32768 ? (int)x:(int)x-65536; }
int func_80195564(int e,Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==8 && count==10 && cb==func_80195564);
    setups++; return setupResult;
}
Particle *func_800CE610(void *p) {
    int i=calls++;
    assert(i<2 && p==&pools[i ? 1:0]);
    D_800F33E0=&emitters[1]; D_800F32D0=&actors[1]; D_8019A7F8=&instances[2];
    D_8019A804=255; instances[1].frame=65535;
    D_8019A4D4[frame*2+i].index=indexAfter;
    D_8019A4D4[frame*2+i].alternate=alternate;
    D_800966EC[(frame-7)*128]=sizeWord;
    return i==failAt ? 0:&particles[i];
}
static void reset(void) {
    calls=setups=0; D_800F32D0=&actors[0]; D_8019A7F8=&instances[0];
    D_800F33E0=&emitters[0]; D_8019A804=0;
    memset(particles,0x5A,sizeof(particles)); memcpy(expected,particles,sizeof(particles));
    memset(D_8019A4D4,0,sizeof(D_8019A4D4));
}
static void run(int f,int stop,int first,int second,int failure) {
    int active=!stop && f>=7 && f<=12;
    int expectedCalls=0, successes=0,i,j;
    reset(); frame=f; instances[0].frame=f; D_8019A804=stop; failAt=failure;
    if(active) {
        D_8019A4D4[f*2].index=first; D_8019A4D4[f*2+1].index=second;
        D_8019A4D4[f*2].alternate=!alternate; D_8019A4D4[f*2+1].alternate=!alternate;
        if(first) { expectedCalls=1; if(failure!=0) { successes=1;
            if(second) { expectedCalls=2; if(failure!=1) successes=2; }
        } }
    }
    assert(func_8019569C(1)==(stop ? 2:0));
    assert(calls==expectedCalls && setups==0);
    for(i=0;i<successes;i++) {
        for(j=0;j<3;j++) expected[i].position[j]=half(transforms[alternate ? 2:1][indexAfter].position[j]);
        expected[i].size=half((uint32_t)sizeWord+2048);
    }
    assert(memcmp(expected,particles,sizeof(particles))==0);
    assert(D_8019A804==(calls ? 255:stop));
}
int main(void) {
    int i,j,k,f,a,b,failure;
    const int words[]={0,-2048,32767,-32768,INT32_MAX,INT32_MIN,-1};
    for(i=0;i<3;i++) {
        instances[i].transforms=transforms[i];
        for(j=0;j<256;j++) for(k=0;k<3;k++) transforms[i][j].position[k]=(i+1)*100000+j*333+k*17777;
    }
    actors[0].instance=&instances[0]; actors[1].instance=&instances[1];
    emitters[0].pool=&pools[0]; emitters[1].pool=&pools[1];
    indexAfter=1; alternate=1; sizeWord=123;
    for(f=0;f<65536;f++) { run(f,0,1,1,2); run(f,255,1,1,2); }
    for(f=7;f<=12;f++) for(a=0;a<2;a++) for(b=0;b<2;b++)
        for(failure=0;failure<3;failure++) run(f,0,a,b,failure);
    /* Includes index becoming zero AFTER the pre-allocation sentinel check. */
    for(indexAfter=0;indexAfter<256;indexAfter++) for(alternate=0;alternate<2;alternate++)
        for(j=0;j<7;j++) for(f=7;f<=12;f++) { sizeWord=words[j]; run(f,0,1,1,2); }
    for(i=0;i<65536;i++) { sizeWord=i; indexAfter=i&255; alternate=i&1; run(7,0,1,1,2); }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); assert(func_8019569C(0)==setupResult && setups==1 && calls==0);
    }
    for(i=0;i<65536;i++) {
        reset(); D_800E11FA=i; D_800E2850[i]=65535-i;
        assert(func_8019569C(2)==0 && calls==0 && setups==0);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F336C==3 && D_800F336E==1 && D_800F3372==0 && D_800F3374==0);
        assert(D_800F3370==65535-i);
    }
    for(i=-3;i<=5;i++) if(i<0 || i>2) {
        reset(); assert(func_8019569C(i)==0 && calls==0 && setups==0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "transforms"
            result = subprocess.run(["cc", "-std=gnu11", "-O2", "-fwrapv", "-fno-strict-aliasing", "-x", "c", "-", "-o", str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

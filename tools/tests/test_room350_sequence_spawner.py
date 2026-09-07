import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/overlays/room_m350/RoomEffect_SequenceSpawner.c"


class SequenceSpawnerTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"), "PSX tools unavailable")
    def test_target_layout(self):
        source = SOURCE.read_text().split("extern Actor", 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==28 ? 1:-1];
typedef char b[sizeof(Vector)==8 ? 1:-1];
typedef char c[OFF(Instance,animation)==14 ? 1:-1];
typedef char d[OFF(Instance,frame)==22 ? 1:-1];
typedef char e[OFF(Instance,previousFrame)==26 ? 1:-1];
typedef char f[OFF(Instance,yaw)==58 ? 1:-1];
typedef char g[OFF(Actor,instance)==8 ? 1:-1];
typedef char h[OFF(Emitter,pool)==8 ? 1:-1];
typedef char i[OFF(Particle,phase)==12 ? 1:-1];
typedef char j[OFF(Particle,size)==16 ? 1:-1];
typedef char k[OFF(Particle,state)==24 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "layout.c"
            path.write_text(source)
            result = subprocess.run([str(ROOT / "tools/scripts/cc.sh"), str(path), str(path.with_suffix(".o"))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_initialization_and_crossing(self):
        harness = '#include <assert.h>\n#include <string.h>\n' + SOURCE.read_text() + r'''
Actor *D_800F32D0;
Emitter *D_800F33E0;
Vector D_8019A7A0;
unsigned char D_8019A804;
short D_8019A802;
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336C,D_800F336E;
volatile unsigned short D_800E11EA,D_800F3370;
unsigned short D_800E2850[65536];
static Actor actors[2];
static Instance instances[2];
static Emitter emitter;
static int pool, calls, setups, setupResult, countAfterAllocation;
static Particle particle, expected;
static int half(int value) {
    unsigned int bits=(unsigned int)value & 65535;
    return bits<32768 ? (int)bits:(int)bits-65536;
}
int func_801947BC(int e, Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pool && stride==28 && count==3 && cb==func_801947BC);
    setups++;
    return setupResult;
}
Particle *func_800CE610(void *p) {
    assert(p==&pool && calls++==0);
    /* Allocation is assumed successful by the target. Probe helper reloads. */
    D_800F32D0=&actors[1]; D_8019A802=countAfterAllocation; D_8019A804=255;
    D_8019A7A0.x=-32768; D_8019A7A0.y=32767; D_8019A7A0.z=-123; D_8019A7A0.pad=99;
    return &particle;
}
static void reset(void) {
    calls=setups=0;
    D_800F32D0=&actors[0]; D_800F33E0=&emitter;
    D_8019A802=123; D_8019A804=0;
    memset(&particle,0x5A,sizeof(particle)); expected=particle;
    memset(&D_8019A7A0,0,sizeof(D_8019A7A0));
}
static void run(int animation,int frame,int previous,int stop) {
    int active=!stop && animation==13 && frame>=15 && previous<15;
    reset(); D_8019A804=stop;
    instances[0].animation=animation; instances[0].frame=frame; instances[0].previousFrame=previous;
    assert(func_80194CFC(1)==(stop ? 2:0));
    assert(calls==active && setups==0);
    if(active) {
        expected.position=D_8019A7A0; expected.position.pad=16;
        expected.speed=512; expected.yaw=half(instances[1].yaw); expected.phase=0;
        expected.duration=16; expected.frame=0; expected.state=0;
        expected.size=half(100-countAfterAllocation*20);
        assert(D_8019A802==half(countAfterAllocation+1) && D_8019A804==255);
    } else assert(D_8019A802==123 && D_8019A804==stop);
    assert(memcmp(&expected,&particle,sizeof(particle))==0);
}
int main(void) {
    int i,j;
    int edges[]={0,14,15,32767,32768,65535};
    emitter.pool=&pool;
    actors[0].instance=&instances[0]; actors[1].instance=&instances[1];
    instances[0].yaw=1; instances[1].yaw=65535;
    countAfterAllocation=5;
    for(i=0;i<65536;i++) { run(13,i,0,0); run(13,15,i,0); }
    for(i=0;i<256;i++) for(j=0;j<6;j++) {
        run(i,edges[j],0,0); run(13,edges[j],0,i);
    }
    for(i=-32768;i<=32767;i++) {
        countAfterAllocation=i; instances[1].yaw=(unsigned short)i;
        run(13,15,0,0);
    }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); assert(func_80194CFC(0)==setupResult && setups==1 && calls==0);
        assert(D_8019A802==123);
    }
    for(i=0;i<65536;i++) {
        reset(); D_800E11EA=i; D_800E2850[i]=65535-i;
        assert(func_80194CFC(2)==0 && calls==0 && setups==0);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F3372==0 && D_800F3374==64 && D_800F336C==3 && D_800F336E==0);
        assert(D_800F3370==65535-i && D_8019A802==123);
    }
    for(i=-3;i<=5;i++) if(i<0 || i>2) {
        reset(); assert(func_80194CFC(i)==0 && calls==0 && setups==0);
        assert(memcmp(&expected,&particle,sizeof(particle))==0 && D_8019A802==123);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "sequence"
            result = subprocess.run(["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing", "-x", "c", "-", "-o", str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

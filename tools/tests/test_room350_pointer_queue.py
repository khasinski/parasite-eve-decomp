import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/overlays/room_m350/RoomEffect_PointerQueueController.c"


class PointerQueueTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"), "PSX tools unavailable")
    def test_target_layout(self):
        source = SOURCE.read_text().split("extern Emitter", 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==8 ? 1:-1];
typedef char b[OFF(Particle,frame)==4 ? 1:-1];
typedef char c[OFF(Queue,count)==0x56 ? 1:-1];
typedef char d[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "layout.c"
            path.write_text(source)
            result = subprocess.run([str(ROOT / "tools/scripts/cc.sh"), str(path), str(path.with_suffix(".o"))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_queue_mutation_and_failure(self):
        source = SOURCE.read_text().replace(' asm("$16")', '')
        declarations, body = source.split("extern Emitter", 1)
        body = ("extern Emitter" + body).replace("extern short D_8019A7FE, D_8019A802;", "extern short D_8019A802;")
        # Bind the interior count symbol to real storage, preserving aliasing.
        harness = '#include <assert.h>\n#include <string.h>\n' + declarations + '\nstatic Queue pending;\n#define D_8019A7FE pending.count\n' + body + r'''
Emitter *D_800F33E0;
short D_8019A802;
unsigned char D_8019A804;
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336C,D_800F336E;
volatile unsigned short D_800E11FA,D_800F3370;
unsigned short D_800E2850[65536];
static Emitter emitters[2];
static int pools[2], calls, setups, setupResult, failAt, nextCount;
static Particle particles[4], expected[4];
static Vector positions[8];
int func_80194F04(int e, Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==8 && count==16 && cb==func_80194F04);
    setups++; return setupResult;
}
Particle *func_800CE610(void *p) {
    int index=calls++;
    assert(index<4 && p==&pools[index ? 1:0]);
    D_800F33E0=&emitters[1];
    pending.count=nextCount;
    pending.positions[index]=&positions[index+4];
    return index==failAt ? 0:&particles[index];
}
static void reset(void) {
    int i;
    calls=setups=0; D_800F33E0=&emitters[0];
    memset(particles,0x5A,sizeof(particles)); memcpy(expected,particles,sizeof(particles));
    for(i=0;i<4;i++) pending.positions[i]=&positions[i];
}
static void run(int count,int stop,int active,int replacement,int failure) {
    int expectedCalls=0, successes=0, i;
    int stopped=stop && active==0;
    reset(); pending.count=count; D_8019A804=stop; D_8019A802=active;
    nextCount=replacement; failAt=failure;
    if(!stopped && count>0) {
        do {
            expectedCalls++;
            if(successes==failure) break;
            successes++;
        } while(successes<replacement);
    }
    assert(func_80195064(1)==(stopped ? 2:0));
    assert(calls==expectedCalls && setups==0);
    assert(pending.count==(stopped ? count:0));
    for(i=0;i<successes;i++) {
        expected[i].position=&positions[i+4]; expected[i].frame=0;
    }
    assert(memcmp(expected,particles,sizeof(particles))==0);
    assert(D_8019A804==stop && D_8019A802==active);
}
int main(void) {
    int count,stop,active,replacement,failure,i;
    emitters[0].pool=&pools[0]; emitters[1].pool=&pools[1];
    for(count=-2;count<=4;count++) for(stop=0;stop<2;stop++)
        for(active=-1;active<=1;active++) for(replacement=-2;replacement<=4;replacement++)
            for(failure=0;failure<=4;failure++) run(count,stop,active,replacement,failure);
    /* Full signed count range, with immediate allocation failure for positive values. */
    for(count=-32768;count<=32767;count++) { run(count,0,1,0,0); run(count,255,0,0,0); }
    for(active=-32768;active<=32767;active++) run(1,255,active,0,0);
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); pending.count=3;
        assert(func_80195064(0)==setupResult && setups==1 && calls==0 && pending.count==3);
    }
    for(i=0;i<65536;i++) {
        reset(); pending.count=3; D_800E11FA=i; D_800E2850[i]=65535-i;
        assert(func_80195064(2)==0 && calls==0 && setups==0 && pending.count==3);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F3372==0 && D_800F3374==0 && D_800F336C==3 && D_800F336E==1);
        assert(D_800F3370==65535-i);
    }
    for(i=-3;i<=5;i++) if(i<0 || i>2) {
        reset(); pending.count=3;
        assert(func_80195064(i)==0 && calls==0 && setups==0 && pending.count==3);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "queue"
            result = subprocess.run(["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing", "-x", "c", "-", "-o", str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

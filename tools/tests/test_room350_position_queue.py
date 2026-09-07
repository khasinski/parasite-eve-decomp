import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/overlays/room_m350/RoomEffect_PositionQueue.c"


def controller_source():
    source = SOURCE.read_text()
    callback = source.index('int func_80195218')
    controller = source.index('int func_80195378')
    declarations = source[:callback]
    for line in (
        'extern int D_800E27EC, D_800F3428, D_8019A3D0[];\n',
        'extern unsigned short D_800F336C, D_800E1204[];\n',
        'extern short D_800F336A, D_800966EE[];\n',
        'extern int GetClut(int, int);\n',
        'extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);\n',
    ):
        declarations = declarations.replace(line, '')
    return declarations + 'extern int D_800E27EC;\nextern volatile short D_800F336A,D_800F336C;\nextern int func_80195218(int, Particle *);\n' + source[controller:]


class PositionQueueTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"), "PSX tools unavailable")
    def test_target_layout(self):
        source = controller_source().split("extern Emitter", 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==12 ? 1:-1];
typedef char b[OFF(Particle,delay)==8 ? 1:-1];
typedef char c[OFF(Queue,count)==0x48 ? 1:-1];
typedef char d[OFF(Emitter,pool)==8 ? 1:-1];
typedef char e[OFF(Instance,yaw)==0x3A ? 1:-1];
typedef char f[OFF(Actor,instance)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "layout.c"
            path.write_text(source)
            result = subprocess.run([str(ROOT / "tools/scripts/cc.sh"), str(path), str(path.with_suffix(".o"))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_queue_copy_and_helper_reloads(self):
        source = controller_source().replace(' asm("$16")', '')
        declarations, body = source.split("extern Emitter", 1)
        body = ("extern Emitter" + body).replace("extern short D_8019A800, D_8019A802;", "extern short D_8019A802;")
        harness = '#include <assert.h>\n#include <string.h>\n' + declarations + '\nstatic Queue pending;\n#define D_8019A800 pending.count\n' + body + r'''
Emitter *D_800F33E0;
Actor *D_800F32D0;
int D_800E27EC;
short D_8019A802;
unsigned char D_8019A804;
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336C,D_800F336E;
volatile unsigned short D_800E11EA,D_800F3370;
unsigned short D_800E2850[65536];
static Emitter emitters[2];
static Actor actors[2];
static Instance instances[2];
static int pools[2], calls, setups, setupResult, failAt, nextCount, yaw, timer;
static Particle particles[8], expected[8];
static int half(int x) { unsigned int b=(unsigned int)x&65535; return b<32768 ? (int)b:(int)b-65536; }
int func_80195218(int e, Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==12 && count==50 && cb==func_80195218);
    setups++; return setupResult;
}
Particle *func_800CE610(void *p) {
    int index=calls++;
    assert(index<8 && p==&pools[index ? 1:0]);
    D_800F33E0=&emitters[1]; D_800F32D0=&actors[1];
    instances[1].yaw=yaw; D_800E27EC=timer;
    pending.count=nextCount;
    pending.positions[index].x=-32768+index;
    pending.positions[index].y=32767-index;
    pending.positions[index].z=-123-index;
    pending.positions[index].pad=999;
    return index==failAt ? 0:&particles[index];
}
static void reset(void) {
    calls=setups=0; D_800F33E0=&emitters[0]; D_800F32D0=&actors[0];
    D_800E27EC=123; instances[0].yaw=123;
    memset(particles,0x5A,sizeof(particles)); memcpy(expected,particles,sizeof(particles));
    memset(pending.positions,0,sizeof(pending.positions));
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
    assert(func_80195378(1)==(stopped ? 2:0));
    assert(calls==expectedCalls && setups==0);
    assert(pending.count==(stopped ? count:0));
    for(i=0;i<successes;i++) {
        expected[i].position=pending.positions[i]; expected[i].position.pad=half(yaw);
        expected[i].delay=(unsigned int)timer%8;
    }
    assert(memcmp(expected,particles,sizeof(particles))==0);
    assert(D_8019A804==stop && D_8019A802==active);
}
int main(void) {
    int count,stop,active,replacement,failure,i,j;
    const int timers[]={0,1,7,8,-1,-8,0x7FFFFFFF,(-2147483647-1)};
    emitters[0].pool=&pools[0]; emitters[1].pool=&pools[1];
    actors[0].instance=&instances[0]; actors[1].instance=&instances[1];
    yaw=32768; timer=-1;
    for(count=-2;count<=8;count++) for(stop=0;stop<2;stop++)
        for(active=-1;active<=1;active++) for(replacement=-2;replacement<=8;replacement++)
            for(failure=0;failure<=8;failure++) run(count,stop,active,replacement,failure);
    for(count=-32768;count<=32767;count++) { run(count,0,1,0,0); run(count,255,0,0,0); }
    for(active=-32768;active<=32767;active++) run(1,255,active,0,0);
    for(i=0;i<65536;i++) for(j=0;j<8;j++) { yaw=i; timer=timers[j]; run(1,0,1,1,8); }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); pending.count=3;
        assert(func_80195378(0)==setupResult && setups==1 && calls==0 && pending.count==3);
    }
    for(i=0;i<65536;i++) {
        reset(); pending.count=3; D_800E11EA=i; D_800E2850[i]=65535-i;
        assert(func_80195378(2)==0 && calls==0 && setups==0 && pending.count==3);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F3372==0 && D_800F3374==0 && D_800F336C==3 && D_800F336E==0);
        assert(D_800F3370==65535-i);
    }
    for(i=-3;i<=5;i++) if(i<0 || i>2) {
        reset(); pending.count=3;
        assert(func_80195378(i)==0 && calls==0 && setups==0 && pending.count==3);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "queue"
            result = subprocess.run(["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing", "-x", "c", "-", "-o", str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

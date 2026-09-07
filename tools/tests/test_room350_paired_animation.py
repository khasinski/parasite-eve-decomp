import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/overlays/room_m350/RoomEffect_PairedAnimationController.c"


class PairedAnimationTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"), "PSX tools unavailable")
    def test_target_layout(self):
        source = SOURCE.read_text().split("extern Actor", 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==8 ? 1:-1];
typedef char b[OFF(Particle,frame)==4 ? 1:-1];
typedef char c[OFF(Instance,animation)==14 ? 1:-1];
typedef char d[OFF(Instance,frame)==22 ? 1:-1];
typedef char e[OFF(Instance,previousFrame)==26 ? 1:-1];
typedef char f[OFF(Instance,transforms)==0x238 ? 1:-1];
typedef char g[OFF(Transform,position)==20 ? 1:-1];
typedef char h[sizeof(Transform)==32 ? 1:-1];
typedef char i[OFF(Actor,instance)==8 ? 1:-1];
typedef char j[OFF(Emitter,pool)==8 ? 1:-1];
typedef char k[OFF(Owner,soundMode)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "layout.c"
            path.write_text(source)
            result = subprocess.run([str(ROOT / "tools/scripts/cc.sh"), str(path), str(path.with_suffix(".o"))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_animation_allocation_and_sound(self):
        source = SOURCE.read_text().replace(' asm("$16")', '')
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
Actor *D_800F32D0;
Emitter *D_800F33E0;
Vector D_8019A778[2];
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374;
static Actor actors[2];
static Instance instances[2];
static Owner owners[2];
static Transform transforms[2];
static Emitter emitters[2];
static int pools[2], calls, sounds, setups, failAt, setupResult;
static Particle particles[2], expected[2];
int func_80192E4C(int e, Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==8 && count==2 && cb==func_80192E4C);
    setups++;
    return setupResult;
}
Particle *func_800CE610(void *pool) {
    int index=calls++;
    assert(index<2 && pool==&pools[index ? 1:0]);
    /* Exercise reloads after allocation, including the failure path. */
    D_800F33E0=&emitters[1];
    D_800F32D0=&actors[1];
    return index==failAt ? 0:&particles[index];
}
int Asset_Find08w(int sound,int mode,int x,int y,int z) {
    assert(sound==0x5C6 && mode==-17);
    assert(x==-32768 && y==32767 && z==-1);
    assert(calls==(failAt==0 ? 1:2));
    sounds++;
    return 123;
}
static void reset(void) {
    calls=sounds=setups=0;
    D_800F32D0=&actors[0]; D_800F33E0=&emitters[0];
    memset(particles,0x5A,sizeof(particles));
    memcpy(expected,particles,sizeof(particles));
}
static void run(int animation,int frame,int previous,int failure) {
    int active=animation==11 && frame>=8 && frame<50 && previous<8;
    int i, successes=active ? (failure<2 ? failure:2):0;
    reset(); failAt=failure;
    instances[0].animation=animation; instances[0].frame=frame;
    instances[0].previousFrame=previous;
    assert(func_8019360C(1)==(animation==11 && frame>=50 ? 2:0));
    assert(calls==(active ? (failure==0 ? 1:2):0));
    assert(sounds==active && setups==0);
    for(i=0;i<successes;i++) {
        expected[i].position=&D_8019A778[i]; expected[i].frame=0;
    }
    assert(memcmp(expected,particles,sizeof(particles))==0);
}
int main(void) {
    int i,j,k;
    int edges[]={0,7,8,49,50,32767,32768,65535};
    for(i=0;i<2;i++) {
        actors[i].instance=&instances[i]; instances[i].owner=&owners[i];
        instances[i].transforms=&transforms[i]; emitters[i].pool=&pools[i];
    }
    owners[1].soundMode=-17;
    transforms[1].position[0]=0x12348000;
    transforms[1].position[1]=0x56787FFF;
    transforms[1].position[2]=0x1357FFFF;
    for(i=0;i<65536;i++) for(k=0;k<3;k++) {
        run(11,i,0,k);
        run(11,8,i,k);
    }
    for(i=0;i<256;i++) for(j=0;j<8;j++) for(k=0;k<3;k++) run(i,edges[j],0,k);
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); assert(func_8019360C(0)==setupResult && setups==1 && calls==0 && sounds==0);
    }
    reset();
    D_800F3368=D_800F336A=D_800F3376=D_800F3378=D_800F3372=D_800F3374=-99;
    assert(func_8019360C(2)==0 && calls==0 && sounds==0 && setups==0);
    assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
    assert(D_800F3372==0 && D_800F3374==0);
    for(i=-3;i<=5;i++) if(i<0 || i>2) {
        reset(); assert(func_8019360C(i)==0 && calls==0 && sounds==0 && setups==0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "paired"
            result = subprocess.run(["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing", "-x", "c", "-", "-o", str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

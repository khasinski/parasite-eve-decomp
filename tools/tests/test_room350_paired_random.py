import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_PairedRandomSpawner.c'


class PairedRandomTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Emitter', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==16 ? 1:-1];
typedef char b[OFF(Particle,position)==8 ? 1:-1];
typedef char c[OFF(Particle,color)==12 ? 1:-1];
typedef char d[sizeof(State)==4 ? 1:-1];
typedef char e[OFF(State,count)==2 ? 1:-1];
typedef char f[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_allocation_rng_and_counter_wrap(self):
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n#include <limits.h>\n' + SOURCE.read_text() + r'''
Emitter *D_800F33E0;
int D_800E27EC,D_8019A414[8];
Vector D_8019A778[2];
volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F336C,D_800F336E,D_800F3372,D_800F3374;
volatile unsigned short D_800E11EA,D_800F3370;
unsigned short D_800E2850[65536];
static Emitter emitters[2];
static int pools[2],allocs,randoms,setups,setupResult,failAt,mutate,lastCount;
static uint32_t seed;
static State state;
static Particle particles[2],expected[2];
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
static void change_count(unsigned n) {
    if(mutate) state.count=half((unsigned short)state.count+n);
    lastCount=(unsigned short)state.count;
}
int func_801927A4(int e,Particle *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)(int,Particle *)) {
    assert(p==&pools[0] && stride==16 && count==18 && cb==func_801927A4);
    assert(state.delay==0 && state.count==0); setups++; return setupResult;
}
Particle *func_800CE610(void *p) {
    int index=allocs++;
    assert(index<2 && p==&pools[mutate && index>0]);
    change_count(13);
    if(index==failAt) return 0;
    return &particles[index];
}
int Inv_ScrambleGrid(void) {
    int call=randoms++,index=call/2,result;
    uint32_t value=seed+(uint32_t)call*0x7654321u;
    assert(allocs==index+1);
    change_count(37);
    if(mutate) { D_800F33E0=&emitters[1]; D_800E27EC=INT_MAX; }
    if(!(call&1)) expected[index].rotation.x=half(value<<4);
    else {
        expected[index].rotation.y=half(value<<4);
        expected[index].rotation.z=0; expected[index].rotation.pad=1;
        expected[index].position=&D_8019A778[index];
        expected[index].color=D_8019A414[(state.count+index)&7];
    }
    memcpy(&result,&value,4); return result;
}
static void reset(unsigned count) {
    allocs=randoms=setups=0; D_800F33E0=&emitters[0]; D_800E27EC=16;
    state.delay=-123; state.count=half(count); lastCount=count&65535;
    memset(particles,0x5A,sizeof(particles)); memcpy(expected,particles,sizeof(particles));
}
int main(void) {
    int i,failure,m,event,edge,timers[]={INT_MIN,-1,0,16,17,INT_MAX};
    emitters[0].pool=&pools[0]; emitters[1].pool=&pools[1];
    for(i=0;i<8;i++) D_8019A414[i]=i*0x10203+123;
    for(i=0;i<65536;i++) for(failure=0;failure<3;failure++) for(m=0;m<2;m++) {
        int delay;
        reset(i); failAt=failure; mutate=m; seed=0xFEDC0000u|i;
        assert(func_801928D4(1,&state)==0);
        assert(allocs==(failure<2 ? failure+1:2) && randoms==failure*2 && setups==0);
        assert(memcmp(particles,expected,sizeof(particles))==0);
        delay=half(4u-lastCount); if(delay<=0) delay=1;
        assert(state.count==half(lastCount+1) && state.delay==delay);
    }
    mutate=0; failAt=0;
    for(edge=0;edge<6;edge++) {
        reset(65535); D_800E27EC=timers[edge];
        assert(func_801928D4(1,&state)==(timers[edge]>=17 ? 2:0));
        if(timers[edge]>=17) assert(allocs==0 && state.count==-1 && state.delay==-123);
        else assert(allocs==1 && state.count==0 && state.delay==5);
    }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(123); assert(func_801928D4(0,&state)==setupResult && setups==1 && allocs==0 && randoms==0);
    }
    for(i=0;i<65536;i++) {
        reset(i); D_800E11EA=i; D_800E2850[i]=65535-i;
        assert(func_801928D4(2,&state)==0 && !allocs && !randoms && !setups);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F336C==3 && D_800F336E==0 && D_800F3372==0 && D_800F3374==0 && D_800F3370==65535-i);
        assert(state.count==half(i) && state.delay==-123);
    }
    for(event=-3;event<=5;event++) if(event<0 || event>2) {
        reset(456); assert(func_801928D4(event,&state)==0 && !allocs && !randoms && !setups);
        assert(state.count==456 && state.delay==-123);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'paired'
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', '-x', 'c', '-', '-o', str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

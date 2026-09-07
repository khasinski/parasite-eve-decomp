import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_RampedPalettePair.c'


def emitter_source():
    source = SOURCE.read_text()
    callback = source.index('int func_80192ADC')
    emitter = source.index('int func_80192C34')
    return source[:callback] + 'extern int func_80192ADC(int, Particle *);\n' + source[emitter:]


class RampedPairTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = emitter_source() + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==8 ? 1:-1];
typedef char b[OFF(Particle,scale)==4 ? 1:-1];
typedef char c[sizeof(State)==4 ? 1:-1];
typedef char d[OFF(State,current)==2 ? 1:-1];
typedef char e[OFF(Instance,frame)==22 ? 1:-1];
typedef char f[OFF(Actor,instance)==8 ? 1:-1];
typedef char g[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_signed_ramp_and_allocation_reloads(self):
        source = emitter_source().replace(' asm("$16")', '')
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
Emitter *D_800F33E0;
Actor *D_800F32D0;
int D_800E27EC;
Vector D_8019A778[2];
volatile short D_800F3368,D_800F3376,D_800F3378,D_800F336E,D_800F3372,D_800F3374;
unsigned short D_800F336C;
short D_800F336A;
volatile unsigned short D_800E11E8,D_800F3370;
unsigned short D_800E2850[65536];
static Emitter emitters[2];
static Actor actor;
static Instance instance;
static State state,reference;
static Particle particles[2],expected[2];
static int pools[2],calls,setups,setupResult,failAt,mutate;
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
int func_80192ADC(int e,Particle *p) { return 0; }
int func_800CE560(void *pool,int size,int count,int (*cb)(int,Particle *)) {
    assert(pool==&pools[0] && size==8 && count==6 && cb==func_80192ADC);
    assert(state.target==16384 && state.current==0); setups++; return setupResult;
}
Particle *func_800CE610(void *pool) {
    int i=calls++;
    assert(i<2 && pool==&pools[mutate && i>0]);
    assert(memcmp(&state,&reference,sizeof(state))==0);
    if(mutate) {
        state.current=half((unsigned short)state.current+0x8001u);
        state.target=-777; reference=state;
        D_800F33E0=&emitters[1]; D_800E27EC=7; instance.frame=65535;
    }
    if(i==failAt) return 0;
    expected[i].position=&D_8019A778[i]; expected[i].scale=state.current;
    return &particles[i];
}
static void reset(int target,int current) {
    calls=setups=0; state.target=target; state.current=current; reference=state;
    D_800F33E0=&emitters[0]; D_800F32D0=&actor;
    memset(particles,0x5A,sizeof(particles)); memcpy(expected,particles,sizeof(particles));
}
static void run(int target,int current,unsigned frame,int timer,int failure,int mutation) {
    int result=0,emitting=0;
    reset(target,current); instance.frame=frame; D_800E27EC=timer; failAt=failure; mutate=mutation;
    if(!target && current<=0) result=2;
    else {
        if(target && frame>=41) reference.target=0;
        if(!(timer&7)) {
            int delta=reference.current<reference.target ? 4096 : reference.current>reference.target ? -4096:0;
            reference.current=half((uint32_t)reference.current+delta);
            emitting=reference.current!=0;
        }
    }
    /* A zero target must not dereference the actor, even while fading. */
    if(!target) D_800F32D0=0;
    assert(func_80192C34(1,&state)==result && setups==0);
    assert(calls==(emitting ? (failure<2 ? failure+1:2):0));
    assert(memcmp(&state,&reference,sizeof(state))==0);
    assert(memcmp(particles,expected,sizeof(particles))==0);
}
int main(void) {
    int i,k,f,m,phase,event,targets[]={0,1,-1,16384,32767,-32768};
    emitters[0].pool=&pools[0]; emitters[1].pool=&pools[1]; actor.instance=&instance;
    for(i=0;i<65536;i++) for(k=0;k<6;k++) for(f=0;f<3;f++) for(m=0;m<2;m++)
        run(targets[k],half(i),40,8,f,m);
    for(i=0;i<65536;i++) {
        run(16384,4096,i,8,2,0);
        run(half(i),half(i),40,8,2,0);
        run(half(i),-32768,40,0,2,0);
    }
    for(phase=0;phase<8;phase++) {
        run(16384,0,40,phase,2,0); run(16384,4096,41,phase,2,0);
        run(0,0,65535,phase,2,0); run(0,-1,65535,phase,2,0);
        run(16384,32767,40,INT_MIN+phase,2,0);
    }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(-123,456); assert(func_80192C34(0,&state)==setupResult && setups==1 && calls==0);
    }
    for(i=0;i<65536;i++) {
        reset(-123,456); D_800E11E8=i; D_800E2850[i]=65535-i;
        assert(func_80192C34(2,&state)==0 && !calls && !setups);
        assert(D_800F336C==2 && D_800F336E==0 && D_800F3368==16 && D_800F336A==1);
        assert(D_800F3376==16 && D_800F3378==16 && D_800F3372==0 && D_800F3374==0 && D_800F3370==65535-i);
        assert(state.target==-123 && state.current==456);
    }
    for(event=-3;event<=5;event++) if(event<0 || event>2) {
        reset(-123,456); assert(func_80192C34(event,&state)==0 && !calls && !setups);
        assert(state.target==-123 && state.current==456);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'ramp'
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', '-x', 'c', '-', '-o', str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)

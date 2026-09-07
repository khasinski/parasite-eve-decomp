import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_TimedRising.c'


def emitter_source():
    source = SOURCE.read_text()
    callback = source.index('int func_801974DC')
    emitter = source.index('int func_80197648')
    return source[:callback] + 'extern int func_801974DC(int, void *);\n' + source[emitter:]


class TimedRisingEmitter(unittest.TestCase):
    def test_target_layout(self):
        source = emitter_source() + r'''
typedef char layout[sizeof(Particle)==8 && __alignof__(Particle)==2 &&
    (unsigned long)&((Particle *)0)->y==2 &&
    (unsigned long)&((Particle *)0)->z==4 &&
    (unsigned long)&((Particle *)0)->speed==6 &&
    (unsigned long)&((Context *)0)->pool==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT / 'tools/scripts/cc.sh'), str(path),
                                     str(path.with_suffix('.o'))], cwd=ROOT,
                                    capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n#include <limits.h>\n' + emitter_source() + r'''
Context *D_800F33E0;
unsigned char D_8019AF6A,D_8019AF69;
short D_8019AE84;
unsigned short D_8019AEFC;
volatile unsigned short D_8019AEFE,D_8019AF00;
unsigned short D_800E11E8,D_800E2850[65536];
unsigned short D_800F3368,D_800F336C,D_800F336E;
short D_800F336A;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static Context context;
static Particle output;
static int pool,initCalls,initResult,allocs,fail;
static short expectedCounter;
static unsigned short newY;
int func_801974DC(int mode,void *p) { assert(0); return 0; }
int func_800CE560(void *p,int size,int count,int (*callback)(int,void *)) {
    assert(p==&pool && size==8 && count==12 && callback==func_801974DC);
    assert(D_8019AE84==0);
    ++initCalls;
    return initResult;
}
Particle *func_800CE610(void *p) {
    assert(p==&pool && allocs++==0 && D_8019AE84==expectedCounter);
    if(fail) return 0;
    D_8019AEFC=65535; D_8019AEFE=newY; D_8019AF00=32768;
    return &output;
}
int main(void) {
    int stop,emit,i,j,c,calls,success;
    unsigned char flags[]={0,1,255};
    short counters[]={-32768,-1,0,1,2,32767};
    unsigned short values[]={0,255,256,32768,65535};
    unsigned short indices[]={0,1,32768,65535};
    D_800F33E0=&context; context.pool=&pool;
    for(initResult=-1;initResult<=1;++initResult) {
        D_8019AE84=99;
        assert(func_80197648(0)==initResult);
    }
    for(stop=0;stop<3;++stop) for(emit=0;emit<3;++emit)
        for(c=0;c<6;++c) for(j=0;j<5;++j) for(fail=0;fail<2;++fail) {
            output.x=11; output.y=22; output.z=33; output.speed=44;
            D_8019AEFC=1; D_8019AEFE=2; D_8019AF00=3; newY=values[j];
            D_8019AE84=counters[c];
            expectedCounter=(short)(counters[c]-1);
            D_8019AF6A=flags[emit]; D_8019AF69=flags[stop]; allocs=0;
            assert(func_80197648(1)==(stop ?2:0));
            calls=!stop && emit && counters[c]<=0;
            success=calls && !fail;
            assert(allocs==calls);
            assert(D_8019AE84==(!stop && emit ? (success ?2:expectedCounter):counters[c]));
            assert((unsigned short)output.x==(success ?65535:11));
            assert(output.y==(success ?(unsigned short)(newY-256):22));
            assert((unsigned short)output.z==(success ?32768:33) && output.speed==(success ?0:44));
            assert(D_8019AF6A==flags[emit] && D_8019AF69==flags[stop]);
        }
    calls=allocs; D_8019AE84=91;
    for(i=0;i<4;++i) {
        D_800E11E8=indices[i]; D_800E2850[indices[i]]=65535-i;
        D_800F3368=D_800F336A=D_800F336C=D_800F336E=0xAAAA;
        D_800F3370=D_800F3372=D_800F3374=0xAAAA;
        D_800F3376=D_800F3378=0xAAAA;
        assert(func_80197648(2)==0 && allocs==calls && D_8019AE84==91);
        assert(D_800F3368==16 && D_800F336A==1 && D_800F336C==2);
        assert(D_800F336E==0 && D_800F3370==65535-i && D_800F3372==0);
        assert(D_800F3374==0 && D_800F3376==16 && D_800F3378==16);
    }
    assert(func_80197648(-1)==0 && func_80197648(3)==0);
    assert(func_80197648(INT_MIN)==0 && func_80197648(INT_MAX)==0);
    assert(initCalls==3 && allocs==calls && D_8019AE84==91);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)],
                           check=True)
            subprocess.run([str(exe)], check=True)

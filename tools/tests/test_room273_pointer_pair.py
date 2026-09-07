import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_PeriodicPointers.c'


def emitter_source():
    source = SOURCE.read_text()
    callback = source.index('int func_80199950')
    emitter = source.index('int func_80199A90')
    return source[:callback] + 'extern int func_80199950(int, void *);\n' + source[emitter:]


class PeriodicPointerPair(unittest.TestCase):
    def test_target_layout(self):
        source = emitter_source() + r'''
typedef char layout[sizeof(Position)==8 && sizeof(Effect)==4 &&
    (unsigned long)&((Effect *)0)->position==0 &&
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
        source = re.sub(r'asm\(""[^;]*;', '', emitter_source())
        source = '#include <assert.h>\n#include <limits.h>\n' + source + r'''
Context *D_800F33E0;
Position D_8019AF74[2];
short D_8019AE88,D_8019AF94,D_8019AF96;
int D_800E27EC;
unsigned short D_800E11FA,D_800E2850[65536];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static Context context;
static Effect outputs[2];
static Position sentinel;
static int pool,initCalls,initResult,allocs,failAt;
int func_80199950(int mode,void *p) { assert(0); return 0; }
int func_800CE560(void *p,int size,int count,int (*callback)(int,void *)) {
    assert(p==&pool && size==4 && count==6 && callback==func_80199950);
    assert(D_8019AE88==0); ++initCalls; return initResult;
}
Effect *func_800CE610(void *p) {
    int i=allocs++;
    assert(p==&pool && i<2);
    if(i==failAt) return 0;
    return &outputs[i];
}
int main(void) {
    short stops[]={-32768,-1,0,1,32767};
    short states[]={-32768,14,15,16,32767};
    short counts[]={-32768,24,25,32767};
    int frames[]={INT_MIN,-9,-8,-1,0,1,7,8,16,INT_MAX};
    unsigned short indices[]={0,1,32768,65535};
    int s,a,b,f,i,stopped,eligible,success,calls;
    D_800F33E0=&context; context.pool=&pool;
    for(initResult=-1;initResult<=1;++initResult) {
        D_8019AE88=99; assert(func_80199A90(0)==initResult);
    }
    for(s=0;s<5;++s) for(a=0;a<5;++a) for(b=0;b<4;++b)
        for(f=0;f<10;++f) for(failAt=-1;failAt<2;++failAt) {
            D_8019AE88=stops[s]; D_8019AF94=states[a]; D_8019AF96=counts[b];
            D_800E27EC=frames[f]; allocs=0;
            outputs[0].position=outputs[1].position=&sentinel;
            stopped=stops[s]!=0 || (states[a]==15 && counts[b]>=25);
            assert(func_80199A90(1)==(stopped ?2:0));
            assert(D_8019AE88==(stops[s] ?stops[s]:(stopped ?1:0)));
            eligible=!stopped && !(frames[f]&7);
            calls=eligible ?(failAt==0 ?1:2):0;
            success=eligible ?(failAt<0 ?2:failAt):0;
            assert(allocs==calls);
            for(i=0;i<2;++i) {
                assert(outputs[i].position==(i<success ?&D_8019AF74[i]:&sentinel));
                if(i<success) {
                    D_8019AF74[i].value[0]=0xA500+i;
                    assert(outputs[i].position->value[0]==0xA500+i);
                }
            }
        }
    calls=allocs; D_8019AE88=91;
    for(i=0;i<4;++i) {
        D_800E11FA=indices[i]; D_800E2850[indices[i]]=65535-i;
        D_800F3368=D_800F336A=D_800F336C=D_800F336E=0xAAAA;
        D_800F3370=D_800F3372=D_800F3374=0xAAAA;
        D_800F3376=D_800F3378=0xAAAA;
        assert(func_80199A90(2)==0 && allocs==calls && D_8019AE88==91);
        assert(D_800F3368==64 && D_800F336A==4 && D_800F336C==3);
        assert(D_800F336E==1 && D_800F3370==65535-i && D_800F3372==0);
        assert(D_800F3374==32 && D_800F3376==64 && D_800F3378==64);
    }
    assert(func_80199A90(-1)==0 && func_80199A90(3)==0);
    assert(func_80199A90(INT_MIN)==0 && func_80199A90(INT_MAX)==0);
    assert(initCalls==3 && allocs==calls && D_8019AE88==91);
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

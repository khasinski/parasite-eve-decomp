import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_BatchRising.c'


def emitter_source():
    source = SOURCE.read_text()
    callback = source.index('int func_80196F2C')
    emitter = source.index('int func_8019706C')
    return source[:callback] + 'extern int func_80196F2C(int, void *);\n' + source[emitter:]


class BatchRisingEmitter(unittest.TestCase):
    def test_target_layout(self):
        source = emitter_source() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((Batch *)0)->y==24 &&
    (unsigned long)&((Batch *)0)->z==48 &&
    (unsigned long)&((Batch *)0)->count==72 &&
    (unsigned long)&((Batch *)0)->stopped==93 &&
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
        source = '#include <assert.h>\n#include <string.h>\n' + source + r'''
Batch D_8019AF0C;
Context *D_800F33E0;
unsigned short D_800E11EA,D_800E2850[65536];
unsigned short D_800F3368,D_800F336C,D_800F336E;
short D_800F336A;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static Context context;
static Vector outputs[12];
static int pool,allocs,failAt,initCalls,initResult,mutate;
int func_80196F2C(int mode,void *p) { assert(0); return 0; }
int func_800CE560(void *p,int size,int count,int (*callback)(int,void *)) {
    assert(p==&pool && size==8 && count==40 && callback==func_80196F2C);
    ++initCalls; return initResult;
}
Vector *func_800CE610(void *p) {
    int i=allocs++;
    assert(p==&pool && i<12);
    if(i==failAt) return 0;
    D_8019AF0C.x[i]=65535-i;
    D_8019AF0C.y[i]=i;
    D_8019AF0C.z[i]=32768+i;
    if(mutate) D_8019AF0C.count=1;
    return &outputs[i];
}
int main(void) {
    short counts[]={-32768,-1,0,1,2,12};
    unsigned short indices[]={0,1,32768,65535};
    int stop,c,i,calls,success,eligible;
    D_800F33E0=&context; context.pool=&pool;
    for(initResult=-1;initResult<=1;++initResult)
        assert(func_8019706C(0)==initResult);
    for(stop=0;stop<2;++stop) for(c=0;c<6;++c)
        for(failAt=-1;failAt<12;++failAt) {
            memset(&D_8019AF0C,0xA5,sizeof(D_8019AF0C));
            memset(outputs,0x55,sizeof(outputs));
            D_8019AF0C.count=counts[c]; D_8019AF0C.stopped=stop ?255:0;
            allocs=0; mutate=0;
            assert(func_8019706C(1)==(stop ?2:0));
            eligible=!stop && counts[c]>0;
            success=eligible ? (failAt>=0 && failAt<counts[c] ?failAt:counts[c]):0;
            calls=success+(eligible && failAt==success && failAt<counts[c]);
            assert(allocs==calls && D_8019AF0C.count==(stop ?counts[c]:0));
            for(i=0;i<12;++i) {
                if(i<success) {
                    assert((unsigned short)outputs[i].x==65535-i && (unsigned short)outputs[i].y==(unsigned short)(i-128));
                    assert((unsigned short)outputs[i].z==32768+i && outputs[i].pad==0);
                } else assert((unsigned short)outputs[i].x==0x5555 && (unsigned short)outputs[i].pad==0x5555);
            }
            assert(D_8019AF0C.stopped==(stop ?255:0));
            for(i=0;i<19;++i) assert(D_8019AF0C.unknown[i]==0xA5);
        }
    D_8019AF0C.stopped=0; D_8019AF0C.count=12;
    allocs=0; failAt=-1; mutate=1;
    assert(func_8019706C(1)==0 && allocs==1 && D_8019AF0C.count==0);
    for(i=0;i<4;++i) {
        D_800E11EA=indices[i]; D_800E2850[indices[i]]=65535-i;
        D_800F336E=D_800F3372=D_800F3374=123;
        assert(func_8019706C(2)==0);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F336C==3);
        assert(D_800F336E==0 && D_800F3372==0 && D_800F3374==0);
        assert(D_800F3370==65535-i && D_800F3376==32 && D_800F3378==32);
    }
    assert(func_8019706C(-1)==0 && func_8019706C(3)==0 && initCalls==3 && allocs==1);
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

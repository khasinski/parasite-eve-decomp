import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_GatedPairEmitter.c'


class GatedPairEmitter(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Record)==8 && __alignof__(Record)==2 &&
    (unsigned long)&((State *)0)->stopped==46 &&
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
        source = re.sub(r'asm\(""[^;]*;', '', SOURCE.read_text())
        source = '#include <assert.h>\n#include <string.h>\n' + source + r'''
Context *D_800F33E0;
State D_8019AF74;
short D_8019AF94,D_8019AF96,D_8019AF98;
unsigned short D_800E11FA,D_800E2850[65536];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static Context context;
static int pool,initCalls,initResult,allocs,failAt;
static Record outputs[2];
int func_80198B1C(int mode,void *p) { assert(0); return 0; }
int func_800CE560(void *p,int size,int count,int (*callback)(int,void *)) {
    assert(p==&pool && size==8 && count==4 && callback==func_80198B1C);
    ++initCalls;
    return initResult;
}
Record *func_800CE610(void *p) {
    int index=allocs++;
    assert(p==&pool && index<2);
    if(index==failAt) return 0;
    /* The record must be captured after allocation, not before it. */
    D_8019AF74.records[index].value[3]=0xFE00+index;
    return &outputs[index];
}
int main(void) {
    int a,b,c,stop,fail,i,eligible,calls,success;
    short states[]={-32768,14,15,16,32767};
    short counts[]={-32768,3,4,32767};
    unsigned short indices[]={0,1,32768,65535};
    D_800F33E0=&context; context.pool=&pool;
    for(initResult=-1;initResult<=1;++initResult)
        assert(func_80198CD4(0)==initResult);
    for(a=0;a<5;++a) for(b=0;b<4;++b) for(c=0;c<4;++c)
        for(stop=0;stop<2;++stop) for(fail=-1;fail<2;++fail) {
            memset(&D_8019AF74,0xA6,sizeof(D_8019AF74));
            memset(outputs,0x55,sizeof(outputs));
            D_8019AF74.stopped=stop ?255:0;
            D_8019AF94=states[a]; D_8019AF96=counts[b]; D_8019AF98=counts[c];
            for(i=0;i<2;++i) {
                D_8019AF74.records[i].value[0]=65535-i;
                D_8019AF74.records[i].value[1]=32768+i;
                D_8019AF74.records[i].value[2]=123+i;
            }
            allocs=0; failAt=fail;
            assert(func_80198CD4(1)==(stop ?2:0));
            eligible=!stop && states[a]==15 && counts[b]>=4 && counts[c]<4;
            calls=eligible ? (fail==0 ?1:2):0;
            success=eligible ? (fail<0 ?2:fail):0;
            assert(allocs==calls);
            for(i=0;i<success;++i) {
                assert(memcmp(&outputs[i],&D_8019AF74.records[i],8)==0);
                assert(outputs[i].value[3]==0xFE00+i);
            }
            for(i=success;i<2;++i) assert(outputs[i].value[0]==0x5555);
            for(i=0;i<30;++i) assert(D_8019AF74.unknown[i]==0xA6);
        }
    for(i=0;i<4;++i) {
        D_800E11FA=indices[i]; D_800E2850[indices[i]]=65535-i;
        assert(func_80198CD4(2)==0);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F336C==3);
        assert(D_800F336E==1 && D_800F3370==65535-i && D_800F3372==0);
        assert(D_800F3374==32 && D_800F3376==32 && D_800F3378==32);
    }
    assert(func_80198CD4(-1)==0 && func_80198CD4(3)==0 && initCalls==3);
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

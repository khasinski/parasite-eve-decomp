import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ScaledLayerEmitter.c'


class ScaledLayerEmitter(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Effect)==12 &&
    (unsigned long)&((Effect *)0)->x==0 &&
    (unsigned long)&((Effect *)0)->y==2 &&
    (unsigned long)&((Effect *)0)->z==4 &&
    (unsigned long)&((Effect *)0)->scale==6 &&
    (unsigned long)&((Effect *)0)->parameter==8 &&
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((StateContext *)0)->owner==8 &&
    (unsigned long)&((Owner *)0)->transform==0x238 &&
    (unsigned long)&((Transform *)0)->x==0x594 &&
    (unsigned long)&((Transform *)0)->y==0x598 &&
    (unsigned long)&((Transform *)0)->z==0x59C ? 1:-1];
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
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
static Context context;
static StateContext state,replacement;
static Owner owner;
static Transform transform;
static Effect output;
static unsigned char pool[128];
Context *D_800F33E0=&context;
StateContext *D_800F32D0=&state;
int D_800E27EC;
short D_800966EC[8192],D_800966EE[8192];
static int init,allocations,fail,newCounter;
int func_8019353C(void) { assert(0); return 0; }
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(init++==0 && p==pool && stride==12 && count==9 && callback==func_8019353C);
    return 140;
}
Effect *func_800CE610(void *p) {
    assert(p==pool && allocations++==0);
    D_800F32D0=&replacement; D_800E27EC=newCounter;
    return fail ? 0 : &output;
}
int main(void) {
    int counters[]={-12345,-1,0,1,7,11,12,12345};
    int samples[]={-32768,-4096,-103,-102,-1,0,1,102,103,4096,32767};
    int i,j,k,f;
    context.pool=pool; replacement.owner=&owner; owner.transform=&transform;
    transform.x=65535; transform.y=-65537; transform.z=32768;
    assert(func_801936F0(0)==140 && init==1);
    for(i=0;i<8;++i) for(j=0;j<11;++j) for(k=0;k<11;++k) for(f=0;f<2;++f) {
        int sampleIndex=(int)(((long long)counters[i]*1024/12)&0xFFF)*2;
        Effect before;
        newCounter=counters[i]; D_800E27EC=0; D_800F32D0=&state; state.owner=0;
        memset(D_800966EC,0x55,sizeof(D_800966EC));
        memset(D_800966EE,0xAA,sizeof(D_800966EE));
        D_800966EC[sampleIndex]=samples[j]; D_800966EE[sampleIndex]=samples[k];
        memset(&output,0xA5,sizeof(output)); before=output; fail=f; allocations=0;
        assert(func_801936F0(1)==0 && allocations==1);
        if(f) assert(memcmp(&output,&before,sizeof(output))==0);
        else {
            assert(output.x==-1 && output.y==-1 && output.z==-32768);
            assert(output.scale==samples[k] && output.parameter[0]==40);
            assert(output.parameter[1]==(unsigned char)((long long)samples[j]*40/4096));
            assert(output.parameter[2]==0 && output.parameter[3]==0xA5);
        }
    }
    fail=1;
    for(i=-2;i<=13;++i) {
        allocations=0; D_800E27EC=i;
        assert(func_801936F0(1)==(i>=12 ? 2:0) && allocations==(i<12));
    }
    allocations=0; D_800E27EC=INT_MAX;
    assert(func_801936F0(1)==2 && allocations==0);
    assert(func_801936F0(2)==0 && func_801936F0(3)==0);
    assert(func_801936F0(INT_MIN)==0 && func_801936F0(INT_MAX)==0 && allocations==0);
    assert(init==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ThresholdPools.c'


class ThresholdPools(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((StateContext *)0)->state==8 &&
    (unsigned long)&((State *)0)->kind==14 &&
    (unsigned long)&((State *)0)->value22==22 &&
    (unsigned long)&((State *)0)->value26==26 &&
    (unsigned long)&((Scene *)0)->object==0x238 && sizeof(Effect)==4 ? 1:-1];
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
static StateContext stateContext;
static State state;
static Scene scene, replacementScene;
static Effect effect;
static unsigned char pool[512], object[64], replacementObject[64];
Context *D_800F33E0=&context;
StateContext *D_800F32D0=&stateContext;
Scene *g_PlayerEntity=&scene;
void *D_8019AE94;
unsigned char D_8019AE9A;
static int calls,allocations,updates,renders,failAllocation,firstSize,secondSize;
int func_801947CC(void) { assert(0); return 0; }
int func_80199F84(void) { assert(0); return 0; }
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(calls++==0 && p==pool && stride==8 && count==2 && callback==func_801947CC);
    return firstSize;
}
int func_800CE5AC(void *out,int offset,int stride,int count,int (*callback)()) {
    assert(calls++==1 && out==&D_8019AE94 && offset==firstSize);
    assert(stride==16 && count==9 && callback==func_80199F84);
    *(void **)out=pool+offset;
    return secondSize;
}
Effect *func_800CE610(void *p) {
    assert(p==pool && !updates && !renders && allocations++==0);
    /* The caller must resolve the scene pointer after allocation. */
    g_PlayerEntity=&replacementScene;
    D_8019AE94=pool+48;
    return failAllocation ? 0 : &effect;
}
int func_800CE688(void *p) {
    assert(p==D_8019AE94 && updates++==0 && !renders);
    return 97;
}
int func_800CE78C(void *p) {
    assert(p==D_8019AE94 && renders++==0 && !updates && !allocations);
    return 98;
}
int main(void) {
    int values[]={-32768,-1,0,3,4,5,32767};
    int stop,kind,i,j,fail;
    context.pool=pool; stateContext.state=&state;
    scene.object=object; replacementScene.object=replacementObject;
    firstSize=36; secondSize=192;
    assert(func_801949EC(0)==228 && calls==2 && D_8019AE94==pool+36);
    calls=0; firstSize=40; secondSize=73;
    assert(func_801949EC(0)==113 && calls==2 && D_8019AE94==pool+40);
    for(stop=0;stop<3;++stop) for(kind=8;kind<=10;++kind)
        for(i=0;i<7;++i) for(j=0;j<7;++j) for(fail=0;fail<2;++fail) {
            State before;
            int wanted=!stop && kind==9 && values[i]>=4 && values[j]<4;
            memset(&state,0xA5,sizeof(state));
            state.kind=kind; state.value22=values[i]; state.value26=values[j];
            before=state; effect.position=object;
            D_8019AE9A=stop==2 ? 255:stop;
            D_8019AE94=pool+36; g_PlayerEntity=&scene;
            failAllocation=fail; allocations=updates=renders=0;
            assert(func_801949EC(1)==(stop ? 2:0));
            assert(allocations==wanted && updates==!stop && renders==0);
            assert(effect.position==(wanted && !fail ? replacementObject+20:object));
            assert(memcmp(&state,&before,sizeof(state))==0);
        }
    allocations=updates=renders=0;
    D_8019AE9A=255;
    assert(func_801949EC(2)==0 && renders==1);
    assert(func_801949EC(-1)==0 && func_801949EC(3)==0);
    assert(func_801949EC(INT_MIN)==0 && func_801949EC(INT_MAX)==0);
    assert(renders==1 && !updates && !allocations && calls==2);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

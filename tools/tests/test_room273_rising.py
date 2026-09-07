import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_BatchRising.c'


def callback_source():
    source = SOURCE.read_text()
    callback = source.index('int func_80196F2C')
    emitter = source.index('int func_8019706C')
    return source[:callback] + source[callback:emitter]


class RisingSprite(unittest.TestCase):
    def test_target_layout(self):
        source = callback_source() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((Vector *)0)->y==2 ? 1:-1];
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
        source = '#include <assert.h>\n#include <limits.h>\n' + callback_source() + r'''
int D_800E27EC, D_800F3428;
unsigned short D_800F336C, D_800E1204[8];
short D_800F336A, D_8019AD68[8];
int D_800966EC[4096];
static Vector position;
static int cluts, draws, expectedPalette, expectedTile, expectedShade, mutate;
unsigned short GetClut(int x, int y) {
    assert(cluts++==0 && draws==0 && x==64 && y==expectedPalette);
    if (mutate) { D_800E27EC=999; D_800F336A=-2; }
    return 0xF123;
}
void func_800CEE20(Vector *p, void *rotation, int w, int h, int tile,
                  int clut, int flag, int shade, void *color) {
    assert(cluts==1 && draws++==0 && p==&position && rotation==0);
    assert(w==4096 && h==4096 && tile==expectedTile);
    assert(clut==0xF123 && flag==1 && shade==expectedShade && color==0);
}
int main(void) {
    int i, kind, variant, frame, index;
    int timers[]={INT_MIN,0,15,16,INT_MAX};
    for (i=0;i<5;++i) {
        D_800E27EC=timers[i]; position.x=12; position.y=-32768;
        position.z=34; position.pad=56;
        assert(func_80196F2C(1,&position)==(timers[i]>=16));
        assert(position.y==(timers[i]>=16 ? -32768:32762));
        assert(position.x==12 && position.z==34 && position.pad==56);
    }
    assert(!cluts && !draws);
    for (i=0;i<8;++i) D_8019AD68[i]=i+3;
    for (kind=0;kind<8;++kind) for (variant=0;variant<2;++variant)
        for (i=0;i<=16;++i) {
            D_800E27EC=i; frame=i-1;
            D_800F336C=kind; D_800F3428=variant;
            D_800E1204[kind]=65535;
            expectedPalette=65535+(kind==4 && variant ? 4:0);
            D_800F336A=2; mutate=variant;
            expectedTile=44+(variant ? -2:2)*D_8019AD68[frame/4];
            index=(((unsigned int)frame<<9)&0x3E00)/4;
            D_800966EC[index]=variant ? 0x1234FFE0:0x56780040;
            expectedShade=variant ? -1:2;
            cluts=draws=0;
            assert(func_80196F2C(2,&position)==0 && cluts==1 && draws==1);
        }
    assert(func_80196F2C(0,&position)==0);
    assert(func_80196F2C(-1,&position)==0);
    assert(func_80196F2C(INT_MAX,&position)==0);
    assert(cluts==1 && draws==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

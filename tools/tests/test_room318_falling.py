import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m318/RoomEffect_FallingPaletteSprite.c'


class FallingSprite(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==8 && OFF(Particle,speed)==6 ? 1:-1];
typedef char b[sizeof(Vector)==8 && OFF(Vector,z)==4 ? 1:-1];
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
        source = re.sub(r' asm\("\$\d+"\)', '', SOURCE.read_text())
        source = re.sub(r'asm\(""[^;]*;', '', source)
        source = '#include <assert.h>\n#include <limits.h>\n' + source + r'''
int D_800E27EC, D_800F3428;
unsigned short D_800F336C, D_800E1204[8];
char D_8019948C[1];
static Particle particle;
static int generated, cluts, draws, expectedPalette, mutate;
void func_800CF3AC(void *table, void *out, int frame) {
    int *color = out;
    assert(table == D_8019948C && frame == D_800E27EC);
    assert(generated++ == 0 && cluts == 0 && draws == 0);
    color[0] = 123; color[1] = 456;
    if (mutate) { particle.x = 65535; particle.y = 32768; particle.z = 32767; }
}
unsigned short GetClut(int x, int y) {
    assert(generated == 1 && cluts++ == 0 && draws == 0);
    assert(x == 0 && y == expectedPalette);
    return 0xCAFE;
}
void func_800CEE20(Vector *p, void *rotation, int w, int h, int tile,
                  int clut, int flag, int shade, void *data) {
    int *color = data;
    assert(generated == 1 && cluts == 1 && draws++ == 0);
    assert((unsigned short)p->x == particle.x);
    assert((unsigned short)p->y == particle.y && (unsigned short)p->z == particle.z);
    assert(rotation == 0 && w == 2048 && h == 2048 && tile == 6);
    assert(clut == 0xCAFE && flag == 1 && shade == 128);
    assert(color[0] == 123 && color[1] == 456);
}
int main(void) {
    int frames[] = {INT_MIN, -1, 0, 23, 24, INT_MAX};
    unsigned short speeds[] = {0, 1, 32768, 65535};
    unsigned int i, j;
    int kind, variant;
    for (i=0; i<sizeof(frames)/sizeof(frames[0]); ++i)
        for (j=0; j<sizeof(speeds)/sizeof(speeds[0]); ++j) {
            particle.x=12; particle.y=3; particle.z=34; particle.speed=speeds[j];
            D_800E27EC=frames[i];
            assert(func_80192DA0(1, &particle) == (frames[i]>=24));
            assert(particle.y == (unsigned short)(3-speeds[j]));
            assert(particle.speed == (unsigned short)(speeds[j]-1));
            assert(particle.x==12 && particle.z==34);
        }
    assert(!generated && !cluts && !draws);
    for (kind=0; kind<8; ++kind) for (variant=0; variant<2; ++variant) {
        generated=cluts=draws=0; mutate=variant;
        D_800F336C=kind; D_800F3428=variant;
        D_800E1204[kind]=65535;
        expectedPalette=65535+(kind==4 && variant ? 10:6);
        assert(func_80192DA0(2, &particle)==0);
        assert(generated==1 && cluts==1 && draws==1);
    }
    assert(func_80192DA0(0, &particle)==0);
    assert(func_80192DA0(-1, &particle)==0);
    assert(func_80192DA0(INT_MAX, &particle)==0);
    assert(generated==1 && cluts==1 && draws==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

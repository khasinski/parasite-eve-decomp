import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class ResetGraphTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_reset_modes_order_layout_and_dimensions(self):
        source = (ROOT / "src/main/main/Render_InitEntityPool.c").read_text()
        for pin in ('$4', '$2', '$3'):
            self.assertEqual(source.count('asm("%s")' % pin), 1)
            source = source.replace('asm("%s")' % pin, '')
        source = source.replace('::: "$6"', '::: "memory"')
        source = source.replace(': "$3");', ': "memory");')
        # Preserve the PSX state layout and map only its fixed-address page.
        source = source.replace('void (*done)(void);', 'unsigned int done;')
        source = source.replace('(unsigned char *)0x80090000', 'testPage')
        source = source.replace('printf', 'testPrintf')
        harness = 'static unsigned char testPage[0x6000];\n' + source + r'''
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
GpuState D_8009574C;
unsigned char D_8009574E, D_80095704[4];
char D_800117E0[] = "reset", D_80011800[] = "soft";
unsigned short D_800957D8[3][2] = {{241, 999}, {481, 999}, {513, 999}};
static GpuCallbacks callbacks;
GpuCallbacks *D_80095744 = &callbacks;
static int stage, prints, modeExpected, variantExpected, softCalls;
int testPrintf(char *format, ...) {
    va_list ap;
    assert(stage == 0 && format == D_800117E0);
    va_start(ap, format);
    assert(va_arg(ap, unsigned char *) == D_80095704);
    assert(va_arg(ap, unsigned char *) == testPage + 0x574C);
    va_end(ap);
    ++prints;
    return 0;
}
static void debug(char *format, int mode) {
    assert(stage == 0 && format == D_80011800 && mode == modeExpected);
    ++prints;
}
void (*D_80095748)(char *, int) = debug;
void GPU_memset(void *ptr, int value, int size) {
    if (stage == 0) {
        assert(ptr == &D_8009574C && value == 0 && size == 128);
        stage = 1;
    } else if (stage == 4) {
        assert(ptr == D_8009574C.drawCache && value == -1 && size == 0x5C);
        assert(D_8009574C.variant == variantExpected && D_8009574C.queue == 1);
        assert(D_8009574C.width == 321 + 100 * variantExpected);
        assert(D_8009574C.height == D_800957D8[variantExpected][0]);
        stage = 5;
    } else {
        assert(stage == 5 && ptr == D_8009574C.displayCache && value == -1 && size == 0x14);
        stage = 6;
    }
    memset(ptr, value, size);
}
int ResetCallback(void) { assert(stage == 1); stage = 2; return 0; }
void GPU_cw(unsigned int command) {
    assert(stage == 2 && command == ((uintptr_t)&callbacks & 0xFFFFFF));
    stage = 3;
}
int Gpu_InitDmaQueue(int mode) {
    assert(stage == 3 && mode == modeExpected);
    stage = 4;
    return variantExpected;
}
static int softReset(int mode) {
    assert(stage == 0 && mode == 1);
    ++softCalls;
    return -77;
}
int main(void) {
    static const int levels[] = {0, 1, 2, 255};
    int mode, level, variant;
    assert(sizeof(GpuState) == 128 && offsetof(GpuState, drawCache) == 16);
    assert(offsetof(GpuState, displayCache) == 0x6C);
    callbacks.reset = softReset;
    for (variant = 0; variant < 3; ++variant)
        *(unsigned short *)(testPage + 0x57CC + 4 * variant) = 321 + 100 * variant;
    for (mode = -8; mode < 24; ++mode) for (level = 0; level < 4; ++level)
    for (variant = 0; variant < 3; ++variant) {
        unsigned char before[128], expected[128] = {0};
        int full = (mode & 7) == 0 || (mode & 7) == 3 || (mode & 7) == 5;
        int result;
        memset(&D_8009574C, 0xA5, sizeof(D_8009574C));
        D_8009574C.debug = D_8009574E = levels[level];
        memcpy(before, &D_8009574C, 128);
        stage = prints = softCalls = 0;
        modeExpected = mode; variantExpected = variant;
        result = Render_InitEntityPool(mode);
        if (full) {
            unsigned short width = 321 + 100 * variant;
            unsigned short height = D_800957D8[variant][0];
            expected[0] = variant; expected[1] = 1;
            memcpy(expected + 4, &width, 2); memcpy(expected + 6, &height, 2);
            memset(expected + 16, 255, 112);
            assert(result == variant && stage == 6 && softCalls == 0);
            assert(prints == ((mode & 7) != 5));
            assert(memcmp(&D_8009574C, expected, 128) == 0);
        } else {
            assert(result == -77 && stage == 0 && softCalls == 1);
            assert(prints == (levels[level] >= 2));
            assert(memcmp(&D_8009574C, before, 128) == 0);
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "reset-graph-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-Wno-pointer-to-int-cast",
                 "-I", str(ROOT / "include"), "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()

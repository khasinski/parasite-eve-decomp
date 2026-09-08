import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class CheckRectTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_diagnostics_boundaries_and_no_mutation(self):
        source = (ROOT / "src/main/psyq/libgpu/sys_clear.c").read_text()
        source = source[:source.find("/* GCC_VERSION:", len("/* GCC_VERSION:"))]
        self.assertEqual(source.count('asm("$3")'), 1)
        source = source.replace('asm("$3")', "")
        # The retail MIPS callback is intentionally unprototyped.  Route the
        # host build through a variadic shim so arm64 passes its arguments in
        # the ABI locations inspected below.
        source = source.replace('#include "pe1/gpu_state.h"',
                                '#include "pe1/gpu_state.h"\n'
                                'extern void test_debug(char *, ...);')
        source = source.replace('D_80095748(format, name);',
                                'test_debug(format, name);')
        source = source.replace('D_80095748(D_800118A4, x, y, width, height);',
                                'test_debug(D_800118A4, x, y, width, height);')
        harness = source + r'''
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
GpuDebugState D_8009574C;
char D_800118B8[] = "all", D_800118A4[] = "fields";
static char name[] = "test";
static RECT before;
static int calls;
void test_debug(char *format, ...) {
    va_list ap;
    va_start(ap, format);
    if (calls == 0) {
        assert(format == (D_8009574C.queueState.debugLevel == 1
            ? (char *)(uintptr_t)0x80011898 : D_800118B8));
        assert(va_arg(ap, char *) == name);
    } else {
        assert(calls == 1 && format == D_800118A4);
        assert(va_arg(ap, int) == before.x);
        assert(va_arg(ap, int) == before.y);
        assert(va_arg(ap, int) == before.w);
        assert(va_arg(ap, int) == before.h);
    }
    va_end(ap);
    ++calls;
}
int main(void) {
    static const short values[] = {-32768, -1, 0, 1, 239, 240, 241,
                                    319, 320, 321, 32767};
    int a, b, c, d, level;
    D_8009574C.width = 320;
    D_8009574C.height = 240;
    for (level = 0; level < 5; ++level) {
        D_8009574C.queueState.debugLevel = level == 4 ? 255 : level;
        for (a = 0; a < 11; ++a) for (b = 0; b < 11; ++b)
        for (c = 0; c < 11; ++c) for (d = 0; d < 11; ++d) {
            RECT rect = {values[a], values[b], values[c], values[d]};
            int invalid = rect.w > 320 || rect.w + rect.x > 320 ||
                rect.y > 240 || rect.y + rect.h > 240 || rect.w <= 0 ||
                rect.x < 0 || rect.y < 0 || rect.h <= 0;
            before = rect;
            calls = 0;
            checkRECT(name, &rect);
            assert(calls == ((level == 2 || (level == 1 && invalid)) ? 2 : 0));
            assert(memcmp(&rect, &before, sizeof(rect)) == 0);
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "check-rect-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-I", str(ROOT / "include"),
                 "-I", str(ROOT / "tools/m2c"), "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()

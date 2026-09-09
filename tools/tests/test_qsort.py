import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class QsortTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_sort_and_record_preservation(self):
        source = (ROOT / "src/main/psyq/libc/qsort.c").read_text()
        # Host execution tests the C algorithm; retail SHA tests MIPS constraints.
        for constraint in (' asm("$20")', ' asm("$22")', ' asm("$8")',
                           'asm volatile("" : "=r"(left) : "0"(left));',
                           'asm volatile("" : "+r"(pivot) : "r"(half));'):
            self.assertEqual(source.count(constraint), 1)
            source = source.replace(constraint, "")
        source = source.replace('#include "common.h"', "")
        source = source[:source.index("/* Trailing word")]
        harness = r'''
#include <assert.h>
#include <string.h>
#define qsort pe_qsort
typedef unsigned char u8;
typedef unsigned int u32;
'''
        harness += source
        harness += r'''
static int compare_ints(const void *a, const void *b) {
    int left = *(const int *)a, right = *(const int *)b;
    return (left > right) - (left < right);
}
static int compare_records(const void *a, const void *b) {
    return *(const unsigned char *)a - *(const unsigned char *)b;
}
int main(void) {
    unsigned int n, i, seed = 7;
    int values[64];
    for (n = 0; n <= 64; n++) {
        int before[17] = {0}, after[17] = {0};
        for (i = 0; i < n; i++) {
            seed = seed * 1664525u + 1013904223u;
            values[i] = (int)(seed % 17) - 8;
            before[values[i] + 8]++;
        }
        pe_qsort(values, n, sizeof(int), compare_ints);
        for (i = 0; i < n; i++) {
            after[values[i] + 8]++;
            if (i) assert(values[i - 1] <= values[i]);
        }
        assert(memcmp(before, after, sizeof(before)) == 0);
        pe_qsort(values, n, sizeof(int), compare_ints);
        for (i = 1; i < n; i++) assert(values[i - 1] <= values[i]);
    }
    {
        unsigned char records[][3] = {{3, 30, 31}, {1, 10, 11}, {2, 20, 21}};
        pe_qsort(records, 3, 3, compare_records);
        for (i = 0; i < 3; i++) {
            assert(records[i][0] == i + 1);
            assert(records[i][1] == (i + 1) * 10);
            assert(records[i][2] == (i + 1) * 10 + 1);
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "qsort-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()

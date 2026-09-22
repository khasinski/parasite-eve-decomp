"""Retail versus stock-compiled behavior for Render_SetFontGlyphByCode.

The harness links the shipped C with the stock cc.sh pipeline, then runs that
object and the retail bytes. A loader stub clobbers caller-saved registers,
HI/LO, and the table/state the function is required to reload.
"""
import pathlib
import random
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
ENTRY = 0x8003944C
EXIT = 0x80010000
LOAD = 0x800389DC
POOL = 0x80110000
STATE = 0x80091A1D
GLYPH = STATE + 1
INDEX = STATE + 2
FAILED = STATE + 3
TABLE = 0x80091A28
CODES = 0x8009EE22
RETAIL_OFF = 0x29C4C
RETAIL_END = 0x29E78
REGIONS = [(POOL, 0x1000), (0x80091800, 0x800), (CODES, 256)]


def physical(address):
    return address & 0x1FFFFFFF


class RenderSetFontGlyphByCodeTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    @unittest.skipUnless((ROOT / "tools/scripts/cc.sh").is_file(), "stock compiler unavailable")
    @unittest.skipUnless((ROOT / "assets/USA/main.exe").is_file(), "retail image unavailable")
    def test_linked_object_matches_retail_behavior(self):
        try:
            from unicorn import Uc, UcError, UC_ARCH_MIPS, UC_HOOK_CODE, UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS32
            from unicorn.mips_const import (
                UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3,
                UC_MIPS_REG_GP, UC_MIPS_REG_HI, UC_MIPS_REG_LO, UC_MIPS_REG_PC,
                UC_MIPS_REG_RA, UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2,
                UC_MIPS_REG_S3, UC_MIPS_REG_S4, UC_MIPS_REG_S5, UC_MIPS_REG_S6,
                UC_MIPS_REG_S7, UC_MIPS_REG_FP, UC_MIPS_REG_SP, UC_MIPS_REG_T0,
                UC_MIPS_REG_T1, UC_MIPS_REG_T2, UC_MIPS_REG_T3, UC_MIPS_REG_T4,
                UC_MIPS_REG_T5, UC_MIPS_REG_T6, UC_MIPS_REG_T7, UC_MIPS_REG_T8,
                UC_MIPS_REG_T9, UC_MIPS_REG_V0, UC_MIPS_REG_V1,
            )
        except ImportError:
            self.skipTest("unicorn unavailable")

        saved_regs = [
            UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2, UC_MIPS_REG_S3,
            UC_MIPS_REG_S4, UC_MIPS_REG_S5, UC_MIPS_REG_S6, UC_MIPS_REG_S7,
            UC_MIPS_REG_FP,
        ]
        clobber = [
            UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3,
            UC_MIPS_REG_V0, UC_MIPS_REG_V1, UC_MIPS_REG_T0, UC_MIPS_REG_T1,
            UC_MIPS_REG_T2, UC_MIPS_REG_T3, UC_MIPS_REG_T4, UC_MIPS_REG_T5,
            UC_MIPS_REG_T6, UC_MIPS_REG_T7, UC_MIPS_REG_T8, UC_MIPS_REG_T9,
        ]
        retail = (ROOT / "assets/USA/main.exe").read_bytes()[RETAIL_OFF:RETAIL_END]
        with tempfile.TemporaryDirectory() as directory:
            work = pathlib.Path(directory)
            obj, elf, data = work / "glyph.o", work / "glyph.elf", work / "glyph.bin"
            subprocess.run(
                ["tools/scripts/cc.sh", "src/main/main/Render_SetFontGlyphByCode.c", str(obj)],
                cwd=ROOT, check=True, capture_output=True,
            )
            (work / "glyph.ld").write_text(
                "D_80091A1D = 0x80091A1D;\n"
                "D_80091A1E = 0x80091A1E;\n"
                "D_80091A1F = 0x80091A1F;\n"
                "D_80091A20 = 0x80091A20;\n"
                "D_80091A28 = 0x80091A28;\n"
                "D_8009EE22 = 0x8009EE22;\n"
                "Render_LoadFontGlyph = 0x800389DC;\n"
                "_gp = 0x8009CD70;\n"
                "SECTIONS { .text 0x8003944C : SUBALIGN(4) { *(.text) } "
                "/DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) } }\n"
            )
            subprocess.run(
                ["mipsel-none-elf-ld", "-EL", "-T", str(work / "glyph.ld"), str(obj), "-o", str(elf)],
                check=True, capture_output=True,
            )
            subprocess.run(
                ["mipsel-none-elf-objcopy", "-O", "binary", "-j", ".text", str(elf), str(data)],
                check=True, capture_output=True,
            )
            compiled = data.read_bytes()
        self.assertEqual(len(compiled), len(retail))
        self.assertEqual(compiled, retail)

        rng = random.Random(0x3944C)
        for case in range(4096):
            memory = bytearray(0x200000)
            for address, size in REGIONS:
                memory[physical(address):physical(address) + size] = rng.randbytes(size)

            def read(address, size=1, memory=memory):
                return int.from_bytes(memory[physical(address):physical(address) + size], "little")

            def write(address, value, size=1, memory=memory):
                memory[physical(address):physical(address) + size] = (
                    value & ((1 << (size * 8)) - 1)
                ).to_bytes(size, "little")

            def snapshot(memory=memory):
                return tuple(bytes(memory[physical(address):physical(address) + size])
                             for address, size in REGIONS)

            code = case % 256 if case < 2048 else [0, 1, 2, 9, 10, 11, 69, 70, 71, 255, 3, 20, 30, 40, 50, 60][case % 16]
            argument = code | (rng.getrandbits(24) << 8)
            table = [POOL, POOL + 0x400, 0x80091A1C, 0x80091A03][(case // 16) % 4] if case >= 2048 else POOL
            for base in (POOL, POOL + 0x400, table):
                for index in range(256):
                    write(base + 4 + index, [0, 1, 2, 3, 255, 17, 23, 254][(index + case // 32) % 8])
                    write(base + 0x1D + index, [0, 1, 2, 3, 23, 24, 254, 255][(index + case // 64) % 8])
                write(base + 3, [0, 1, 2, 24, 255][(case // 16) % 5])
                write(base + 0x1C, [0, 1, 2, 99, 100, 255][(case // 32) % 6])
            write(TABLE, POOL, 4)
            write(FAILED, [0, 1, 2, 255][(case // 256) % 4])
            initial = bytes(memory)
            calls = []
            mutation = rng.getrandbits(32)

            def mutate(store, case=case, table=table, mutation=mutation):
                store(TABLE, table, 4)
                store(FAILED, [0, 1, 2, 255][(case // 16) % 4])
                store(STATE, [0, 1, 9, 10, 11, 20, 70, 255][(case // 64) % 8])
                store(INDEX, mutation)
                if case & 512:
                    store(table + 3, [0, 1, 24, 255][case % 4])
                    store(table + 4, [0, 1, 3, 255][case % 4])
                    store(table + 0x1C, [0, 1, 100, 255][case % 4])

            def load(value):
                calls.append((value, snapshot()))
                mutate(write)

            if code - 2 >= 69:
                write(FAILED, 1)
                result = 255
            elif code < 2:
                write(STATE, 1)
                result = 255
            else:
                write(STATE, code)
                value = read(CODES + code)
                write(GLYPH, value)
                load(value)
                wanted = int(read(STATE) % 10 != 0) if read(FAILED) == 1 else 3
                live = read(TABLE, 4)
                found = 0
                count = read(live + 3)
                for index in range(count):
                    if read(live + 4 + index) == wanted:
                        found = index
                        break
                selected = 255
                for index in range(read(live + 0x1C)):
                    if read(live + 0x1D + index) == (found & 255):
                        selected = index
                        break
                write(INDEX, selected)
                live = read(TABLE, 4)
                index = read(INDEX)
                write(FAILED, 0)
                result = read(live + 4 + read(live + 0x1D + index))
            expected = snapshot()
            saved = [rng.getrandbits(32) for _ in saved_regs]
            for name, body in (("retail", retail), ("C", compiled)):
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                machine.mem_write(0, initial)
                machine.mem_write(physical(ENTRY), body)
                observed = []

                def store(address, value, size=1, machine=machine):
                    machine.mem_write(
                        physical(address),
                        (value & ((1 << (size * 8)) - 1)).to_bytes(size, "little"),
                    )

                def hook(machine, address, size, _data):
                    if address == LOAD:
                        machine.emu_stop()

                machine.hook_add(UC_HOOK_CODE, hook)
                machine.reg_write(UC_MIPS_REG_SP, 0x801F0000)
                machine.reg_write(UC_MIPS_REG_RA, EXIT)
                machine.reg_write(UC_MIPS_REG_GP, 0x8009CD70)
                machine.reg_write(UC_MIPS_REG_A0, argument)
                for reg, value in zip(saved_regs, saved):
                    machine.reg_write(reg, value)
                pc = ENTRY
                try:
                    while True:
                        machine.emu_start(pc, EXIT, count=20000)
                        pc = machine.reg_read(UC_MIPS_REG_PC)
                        if pc != LOAD:
                            break
                        call_index = len(observed)
                        actual = (
                            machine.reg_read(UC_MIPS_REG_A0),
                            tuple(bytes(machine.mem_read(physical(address), size))
                                  for address, size in REGIONS),
                        )
                        observed.append(actual)
                        self.assertLess(call_index, len(calls))
                        self.assertEqual(actual, calls[call_index], (case, name, "call"))
                        mutate(store)
                        pc = machine.reg_read(UC_MIPS_REG_RA)
                        for reg in clobber:
                            machine.reg_write(reg, 0xD00D0000 + reg)
                        machine.reg_write(UC_MIPS_REG_HI, 0x12345678)
                        machine.reg_write(UC_MIPS_REG_LO, 0x87654321)
                        machine.reg_write(UC_MIPS_REG_V0, mutation)
                except UcError as error:
                    self.fail(f"case {case} {name}: {error}")
                self.assertEqual(pc, EXIT, (case, name, "exit"))
                self.assertEqual(len(observed), len(calls), (case, name, "calls"))
                self.assertEqual(
                    tuple(bytes(machine.mem_read(physical(address), size))
                          for address, size in REGIONS),
                    expected,
                    (case, name, "memory"),
                )
                self.assertEqual(machine.reg_read(UC_MIPS_REG_V0), result, (case, name, "result"))
                self.assertEqual(machine.reg_read(UC_MIPS_REG_SP), 0x801F0000, (case, name, "stack"))
                self.assertEqual([machine.reg_read(reg) for reg in saved_regs], saved,
                                 (case, name, "saved"))


if __name__ == "__main__":
    unittest.main()

"""Check the linked glyph loader against retail bytes and CD retry/copy behavior."""
import pathlib
import random
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
ENTRY = 0x800389DC
EXIT = 0x80010000
READ = 0x8006E6A8
POLL = 0x8006E7E8
BUFFER = 0x80100000
TABLE = 0x8009ECD8
RANGE = 0x80093176
STATE = 0x800B0DD8
RETAIL_OFF = 0x291DC
RETAIL_END = 0x293C0


def physical(address):
    return address & 0x1FFFFFFF


class RenderLoadFontGlyphTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    @unittest.skipUnless((ROOT / "assets/USA/main.exe").is_file(), "retail image unavailable")
    def test_byte_match_and_loader_behavior(self):
        try:
            from unicorn import Uc, UcError, UC_ARCH_MIPS, UC_HOOK_CODE, UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS32
            from unicorn.mips_const import (
                UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3,
                UC_MIPS_REG_GP, UC_MIPS_REG_HI, UC_MIPS_REG_LO, UC_MIPS_REG_PC,
                UC_MIPS_REG_RA, UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2,
                UC_MIPS_REG_S3, UC_MIPS_REG_SP, UC_MIPS_REG_T0, UC_MIPS_REG_T1,
                UC_MIPS_REG_T2, UC_MIPS_REG_T3, UC_MIPS_REG_T4, UC_MIPS_REG_T5,
                UC_MIPS_REG_T6, UC_MIPS_REG_T7, UC_MIPS_REG_T8, UC_MIPS_REG_T9,
                UC_MIPS_REG_V0, UC_MIPS_REG_V1,
            )
        except ImportError:
            self.skipTest("unicorn unavailable")

        retail = (ROOT / "assets/USA/main.exe").read_bytes()[RETAIL_OFF:RETAIL_END]
        with tempfile.TemporaryDirectory() as directory:
            work = pathlib.Path(directory)
            obj, elf, binary = (work / name for name in ("glyph.o", "glyph.elf", "glyph.bin"))
            subprocess.run(["tools/scripts/cc.sh", "src/main/main/Render_LoadFontGlyph.c", str(obj)],
                           cwd=ROOT, check=True, capture_output=True)
            (work / "glyph.ld").write_text(
                "D_80093176 = 0x80093176;\n"
                "D_8009ECD8 = 0x8009ECD8;\n"
                "D_800B0DD8 = 0x800B0DD8;\n"
                "D_800B0E6C = 0x800B0E6C;\n"
                "CdRom_ReadSectorsFromLba = 0x8006E6A8;\n"
                "CdRom_PollReady = 0x8006E7E8;\n"
                "_gp = 0x8009CD70;\n"
                "SECTIONS { .text 0x800389DC : SUBALIGN(4) { *(.text) } "
                "/DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) } }\n"
            )
            subprocess.run(["mipsel-none-elf-ld", "-EL", "-T", str(work / "glyph.ld"),
                            str(obj), "-o", str(elf)], check=True, capture_output=True)
            subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary", "-j", ".text",
                            str(elf), str(binary)], check=True, capture_output=True)
            compiled = binary.read_bytes()
        self.assertEqual(compiled, retail)

        rng = random.Random(0x389DC)
        saved_regs = [UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2, UC_MIPS_REG_S3]
        caller_regs = [UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3,
                       UC_MIPS_REG_V1, UC_MIPS_REG_T0, UC_MIPS_REG_T1, UC_MIPS_REG_T2,
                       UC_MIPS_REG_T3, UC_MIPS_REG_T4, UC_MIPS_REG_T5, UC_MIPS_REG_T6,
                       UC_MIPS_REG_T7, UC_MIPS_REG_T8, UC_MIPS_REG_T9]
        patterns = [
            ([0], [0]),
            ([-1, 0], [1, 0]),
            ([0, 0], [1, -1, 2, 0]),
            ([-1, 0, 0], [-1, 0]),
        ]
        for case in range(128):
            code = [0, 1, 2, 24, 69, 127, 128, 255][case % 8]
            read_results, poll_results = patterns[(case // 8) % len(patterns)]
            base = 1000 + case
            start = 2 + case
            end = start + 3
            memory = bytearray(0x200000)
            memory[physical(BUFFER):physical(BUFFER) + 256 * 328 + 328] = rng.randbytes(256 * 328 + 328)
            memory[physical(TABLE):physical(TABLE) + 329] = rng.randbytes(329)
            memory[physical(RANGE):physical(RANGE) + 4] = start.to_bytes(2, "little") + end.to_bytes(2, "little")
            memory[physical(STATE):physical(STATE) + 4] = base.to_bytes(4, "little")
            memory[physical(STATE + 0x94):physical(STATE + 0x98)] = BUFFER.to_bytes(4, "little")
            before = bytes(memory[physical(TABLE):physical(TABLE) + 329])
            expected = bytes(memory[physical(BUFFER + code * 328 + 1):physical(BUFFER + code * 328 + 329)])
            saved = [rng.getrandbits(32) for _ in saved_regs]
            for name, body in (("retail", retail), ("C", compiled)):
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                machine.mem_write(0, bytes(memory))
                machine.mem_write(physical(ENTRY), body)
                machine.reg_write(UC_MIPS_REG_SP, 0x801F0000)
                machine.reg_write(UC_MIPS_REG_RA, EXIT)
                machine.reg_write(UC_MIPS_REG_GP, 0x8009CD70)
                machine.reg_write(UC_MIPS_REG_A0, code | (rng.getrandbits(24) << 8))
                for reg, value in zip(saved_regs, saved):
                    machine.reg_write(reg, value)
                calls = []

                def hook(_machine, address, _size, _data):
                    if address in (READ, POLL):
                        _machine.emu_stop()

                machine.hook_add(UC_HOOK_CODE, hook)
                pc = ENTRY
                try:
                    while True:
                        machine.emu_start(pc, EXIT, count=20000)
                        pc = machine.reg_read(UC_MIPS_REG_PC)
                        if pc == EXIT:
                            break
                        self.assertIn(pc, (READ, POLL), (case, name, "unexpected stop"))
                        if pc == READ:
                            self.assertEqual((machine.reg_read(UC_MIPS_REG_A0),
                                              machine.reg_read(UC_MIPS_REG_A1),
                                              machine.reg_read(UC_MIPS_REG_A2)),
                                             (base + start, BUFFER, end - start), (case, name, "read args"))
                            result = read_results[sum(call == READ for call in calls)]
                        else:
                            result = poll_results[sum(call == POLL for call in calls)]
                        calls.append(pc)
                        for reg in caller_regs:
                            machine.reg_write(reg, 0xD00D0000 + reg)
                        machine.reg_write(UC_MIPS_REG_HI, 0x12345678)
                        machine.reg_write(UC_MIPS_REG_LO, 0x87654321)
                        machine.reg_write(UC_MIPS_REG_V0, result & 0xFFFFFFFF)
                        pc = machine.reg_read(UC_MIPS_REG_RA)
                except (UcError, IndexError) as error:
                    self.fail(f"case {case} {name}: {error}")
                self.assertEqual(calls.count(READ), len(read_results), (case, name, "read count"))
                self.assertEqual(calls.count(POLL), len(poll_results), (case, name, "poll count"))
                actual = bytes(machine.mem_read(physical(TABLE), 329))
                self.assertEqual(actual, before[:1] + expected, (case, name, "table"))
                self.assertEqual(machine.reg_read(UC_MIPS_REG_V0), 0, (case, name, "return"))
                self.assertEqual(machine.reg_read(UC_MIPS_REG_SP), 0x801F0000, (case, name, "stack"))
                self.assertEqual([machine.reg_read(reg) for reg in saved_regs], saved,
                                 (case, name, "saved registers"))


if __name__ == "__main__":
    unittest.main()

"""Retail behavior of the byte-matched AKAO voice-bank state reset."""
import pathlib
import random
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
ENTRY = 0x8006A674
EXIT = 0x80010000
STATE = 0x800B0CD8
PAIRS = 0x80094488
RETAIL_OFF = 0x5AE74
RETAIL_END = 0x5B0D4


def physical(address):
    return address & 0x1FFFFFFF


class AkaoClearVoiceBankTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    @unittest.skipUnless((ROOT / "assets/USA/main.exe").is_file(), "retail image unavailable")
    def test_retail_bytes_and_state_reset(self):
        try:
            from unicorn import Uc, UcError, UC_ARCH_MIPS, UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS32
            from unicorn.mips_const import (
                UC_MIPS_REG_A0, UC_MIPS_REG_GP, UC_MIPS_REG_PC, UC_MIPS_REG_RA,
                UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2, UC_MIPS_REG_S3,
                UC_MIPS_REG_S4, UC_MIPS_REG_S5, UC_MIPS_REG_S6, UC_MIPS_REG_S7,
                UC_MIPS_REG_SP,
            )
        except ImportError:
            self.skipTest("unicorn unavailable")

        retail = (ROOT / "assets/USA/main.exe").read_bytes()[RETAIL_OFF:RETAIL_END]
        with tempfile.TemporaryDirectory() as directory:
            work = pathlib.Path(directory)
            obj, elf, binary = (work / name for name in ("akao.o", "akao.elf", "akao.bin"))
            subprocess.run(["tools/scripts/cc.sh", "src/main/akao/Akao_ClearVoiceBank.c", str(obj)],
                           cwd=ROOT, check=True, capture_output=True)
            (work / "akao.ld").write_text(
                "g_GameState = 0x800B0CD8;\n"
                "D_80094488 = 0x80094488;\n"
                "_gp = 0x8009CD70;\n"
                "SECTIONS { .text 0x8006A674 : SUBALIGN(4) { *(.text) } "
                "/DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) } }\n"
            )
            subprocess.run(["mipsel-none-elf-ld", "-EL", "-T", str(work / "akao.ld"),
                            str(obj), "-o", str(elf)], check=True, capture_output=True)
            subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary", "-j", ".text",
                            str(elf), str(binary)], check=True, capture_output=True)
            compiled = binary.read_bytes()
        self.assertEqual(compiled, retail)

        saved_regs = [UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2,
                      UC_MIPS_REG_S3, UC_MIPS_REG_S4, UC_MIPS_REG_S5,
                      UC_MIPS_REG_S6, UC_MIPS_REG_S7]
        rng = random.Random(0x6A674)
        for case in range(128):
            memory = bytearray(0x200000)
            state = bytearray(rng.randbytes(0x200))
            pairs = bytearray(rng.randbytes(0x20))
            base = [0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF,
                    rng.getrandbits(32)][case % 5]
            state[0x150:0x154] = base.to_bytes(4, "little")
            memory[physical(STATE):physical(STATE) + len(state)] = state
            memory[physical(PAIRS):physical(PAIRS) + len(pairs)] = pairs
            initial = bytes(memory)
            expected = bytearray(memory)

            def write(address, value, width=1):
                expected[physical(address):physical(address) + width] = (
                    value & ((1 << (8 * width)) - 1)
                ).to_bytes(width, "little")

            write(STATE, 3, 4)
            write(STATE + 4, 10, 2)
            write(STATE + 6, -1, 2)
            for offset, value in {8: 2, 9: -1, 10: 11, 11: 0, 12: -1, 13: 1}.items():
                write(STATE + offset, value)
            for offset in range(14, 20):
                write(STATE + offset, 0)
            for index in range(49):
                write(STATE + 0x14 + 4 * index, 0, 4)
            for offset, value in {
                0xD8: 0, 0xD9: 8, 0xDA: -1, 0xDB: -1,
                0xDC: -1, 0xDD: -1, 0xDE: -1, 0xDF: -1,
                0xFF: 0x7F, 0xFE: 0x7F, 0xE0: 0x27, 0xE1: 0xD,
                0xE2: 0, 0xE3: 1, 0xE6: 0x98, 0xE7: -1,
                0xEA: 0, 0xEB: 0, 0xF6: 0x30, 0xF7: 0x7F,
                0x107: 3, 0x10B: 0x62, 0x117: 1,
                0xED: 0x20, 0x108: 0x20, 0x109: 0x20, 0x10A: 0x20,
                0xEC: 0, 0xEE: 0, 0xEF: 0, 0xF0: 0, 0xF1: 0,
                0xF2: 0, 0xF4: 0,
            }.items():
                write(STATE + offset, value)
            for offset, value in {
                0xE4: -1, 0xE8: -1, 0xF8: 0x100, 0xFA: 0x800,
                0x110: 0x140, 0x112: 0xE0, 0x10C: 0, 0x10E: 0,
            }.items():
                write(STATE + offset, value, 2)
            for index in range(4):
                write(PAIRS + index * 8 + 4, 0, 2)
                write(PAIRS + index * 8 + 6, 0, 2)
            for offset, value in {
                0x118: 0xE1000440, 0x11C: 0, 0x120: 0, 0x124: 0,
                0x128: base, 0x12C: base + 0x1400, 0x130: base + 0x2800,
                0x134: 0, 0x138: 0, 0x13C: 0,
                0x140: 0, 0x144: 0, 0x148: 0,
            }.items():
                write(STATE + offset, value, 4)

            saved = [rng.getrandbits(32) for _ in saved_regs]
            for name, body in (("retail", retail), ("C", compiled)):
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                machine.mem_write(0, initial)
                machine.mem_write(physical(ENTRY), body)
                machine.reg_write(UC_MIPS_REG_SP, 0x801F0000)
                machine.reg_write(UC_MIPS_REG_RA, EXIT)
                machine.reg_write(UC_MIPS_REG_GP, 0x8009CD70)
                machine.reg_write(UC_MIPS_REG_A0, rng.getrandbits(32))
                for reg, value in zip(saved_regs, saved):
                    machine.reg_write(reg, value)
                try:
                    machine.emu_start(ENTRY, EXIT, count=10000)
                except UcError as error:
                    self.fail(f"case {case} {name}: {error}")
                self.assertEqual(machine.reg_read(UC_MIPS_REG_PC), EXIT, (case, name, "exit"))
                for address, width in ((STATE, 0x200), (PAIRS, 0x20)):
                    self.assertEqual(bytes(machine.mem_read(physical(address), width)),
                                     bytes(expected[physical(address):physical(address) + width]),
                                     (case, name, hex(address)))
                self.assertEqual(machine.reg_read(UC_MIPS_REG_SP), 0x801F0000, (case, name, "stack"))
                self.assertEqual([machine.reg_read(reg) for reg in saved_regs], saved,
                                 (case, name, "saved registers"))


if __name__ == "__main__":
    unittest.main()

"""Byte identity and deterministic CD/font-selection behavior for the alternate loader."""
import pathlib
import random
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
ENTRY, EXIT = 0x80038D74, 0x80010000
READ, POLL = 0x8006E6A8, 0x8006E7E8
RAND, SRAND, LOAD = 0x80071A54, 0x80071A64, 0x800389DC
TABLE, CODES, BUFFER = 0x8009ECD8, 0x8009EE24, 0x80100000
FLAGS, SEED, CURRENT = 0x80091A1C, 0x80091A24, 0x80091A28
STATE, RANGE, BUFFER_PTR = 0x800B0DD8, 0x80093176, 0x800B0E6C


def physical(address):
    return address & 0x1FFFFFFF


@unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
@unittest.skipUnless((ROOT / "assets/USA/main.exe").is_file(), "retail image unavailable")
class RenderLoadFontGlyphAltTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.retail = (ROOT / "assets/USA/main.exe").read_bytes()[0x29574:0x29984]
        with tempfile.TemporaryDirectory() as directory:
            work = pathlib.Path(directory)
            obj, elf, binary = (work / name for name in ("glyph.o", "glyph.elf", "glyph.bin"))
            subprocess.run(["tools/scripts/cc.sh", "src/main/main/Render_LoadFontGlyphAlt.c", str(obj)],
                           cwd=ROOT, check=True, capture_output=True)
            symbols = {
                "D_800B0DD8": STATE, "D_80093176": RANGE, "D_800B0E6C": BUFFER_PTR,
                "D_8009ECD8": TABLE, "D_8009ECDB": TABLE + 3, "D_8009EE24": CODES,
                "D_80091A20": FLAGS + 4, "D_80091A24": SEED, "D_80091A28": CURRENT,
                "CdRom_ReadSectorsFromLba": READ, "CdRom_PollReady": POLL,
                "rand": RAND, "srand": SRAND, "Render_LoadFontGlyph": LOAD,
                **{f"D_{FLAGS + i:08X}": FLAGS + i for i in range(4)},
            }
            (work / "glyph.ld").write_text(
                "".join(f"{name} = 0x{address:X};\n" for name, address in symbols.items())
                + "SECTIONS { .text 0x80038D74 : SUBALIGN(4) { *(.text) } "
                "/DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) } }\n"
            )
            subprocess.run(["mipsel-none-elf-ld", "-EL", "-T", str(work / "glyph.ld"),
                            str(obj), "-o", str(elf)], check=True, capture_output=True)
            subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary", "-j", ".text",
                            str(elf), str(binary)], check=True, capture_output=True)
            cls.compiled = binary.read_bytes()

    def test_byte_match(self):
        self.assertEqual(self.compiled, self.retail)

    def test_retries_seeding_uniqueness_and_slot_lookup(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_HOOK_CODE, UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS32
            from unicorn import mips_const as reg
        except ImportError:
            self.skipTest("unicorn unavailable")

        calls_to_trap = (READ, POLL, RAND, SRAND, LOAD)
        saved_regs = [getattr(reg, f"UC_MIPS_REG_S{i}") for i in range(8)]
        caller_regs = [getattr(reg, f"UC_MIPS_REG_{name}") for name in
                       ("A0", "A1", "A2", "A3", "V1", "T0", "T1", "T2", "T3", "T4",
                        "T5", "T6", "T7", "T8", "T9", "HI", "LO")]
        patterns = [([0], [0]), ([-1, 0], [1, 0]), ([0, 0], [1, -1, 2, 0])]
        for case in range(48):
            font_count = (9, 10, 18, 68, 105, 110, 120, 255)[case % 8]
            old_seed = (0, 1234, 0, 0x12345678)[case % 4]
            failed = (0, 0, 1, 1)[case % 4]
            seed = 17 if not failed and not old_seed else old_seed
            expected_rng = random.Random(seed)
            expected_codes, unique_count, forced = [], 0, 0
            expected_random_calls = 0
            for i in range(69):
                if (i + 2) % 10 == 0:
                    expected_codes.append(forced)
                    forced += 1
                    continue
                limit = min(font_count - 1, 104 if i + 2 < 10 else 109 if i + 2 < 20 else 113)
                value = expected_rng.randrange(32768) % (limit - 7) + 7
                expected_random_calls += 1
                if unique_count < font_count - 9:
                    while True:
                        unique = True
                        for previous in expected_codes:
                            if previous == value:
                                value = expected_rng.randrange(32768) % (limit - 7) + 7
                                expected_random_calls += 1
                                unique = False
                        if unique:
                            break
                expected_codes.append(value)
                unique_count += 1

            data_rng = random.Random(ENTRY + case)
            glyph = bytearray(data_rng.randbytes(328))
            groups = (0, 1, 3, 24)[case % 4]
            slots = (0, 1, 5, 100)[(case // 4) % 4]
            glyph[3] = groups
            glyph[4:4 + groups] = bytes([7] * groups)
            if groups and case % 3:
                glyph[4 + groups - 1] = 2
            found = groups - 1 if groups and case % 3 else 0
            if groups > 1 and case % 6 == 2:
                glyph[4] = 2
                found = 0
            glyph[28] = slots
            glyph[29:29 + slots] = bytes([255] * slots)
            if slots and case % 5:
                glyph[29 + slots - 1] = found
            slot = slots - 1 if slots and case % 5 else 255
            if slots > 1 and case % 7 == 0:
                glyph[29] = found
                slot = 0
            expected_return = glyph[glyph[29 + slot] + 4]
            reads, polls = patterns[case % len(patterns)]

            for name, body in (("retail", self.retail), ("C", self.compiled)):
                with self.subTest(case=case, code=name):
                    machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                    machine.mem_map(0, 0x200000)
                    machine.mem_write(physical(ENTRY), body)
                    for address, value in ((STATE, 1000), (STATE + 0x94, BUFFER),
                                           (BUFFER_PTR, BUFFER), (SEED, old_seed)):
                        machine.mem_write(physical(address), value.to_bytes(4, "little"))
                    machine.mem_write(physical(RANGE), b"\x02\x00\x05\x00")
                    machine.mem_write(physical(BUFFER), bytes([font_count]))
                    machine.mem_write(physical(FLAGS + 4), bytes([failed]))
                    machine.reg_write(reg.UC_MIPS_REG_SP, 0x801F0000)
                    machine.reg_write(reg.UC_MIPS_REG_RA, EXIT)
                    for r in saved_regs:
                        machine.reg_write(r, 0xCAFE0000 + r)
                    calls, random_calls, seeded = [], 0, False
                    runtime_rng = random.Random(seed)
                    seed_trials = iter((0, 32767, 65534, 17))

                    def hook(_machine, address, _size, _data):
                        if address in calls_to_trap:
                            _machine.emu_stop()

                    machine.hook_add(UC_HOOK_CODE, hook)
                    pc = ENTRY
                    for _ in range(10000):
                        machine.emu_start(pc, EXIT, count=100000)
                        pc = machine.reg_read(reg.UC_MIPS_REG_PC)
                        if pc == EXIT:
                            break
                        self.assertIn(pc, calls_to_trap)
                        result = 0
                        if pc == READ:
                            self.assertEqual(tuple(machine.reg_read(r) for r in
                                                   (reg.UC_MIPS_REG_A0, reg.UC_MIPS_REG_A1,
                                                    reg.UC_MIPS_REG_A2)), (1002, BUFFER, 3))
                            result = reads[calls.count(READ)]
                        elif pc == POLL:
                            result = polls[calls.count(POLL)]
                        elif pc == RAND:
                            result = runtime_rng.randrange(32768) if seeded else next(seed_trials)
                            random_calls += int(seeded)
                        elif pc == SRAND:
                            self.assertEqual(machine.reg_read(reg.UC_MIPS_REG_A0), seed)
                            seeded = True
                        elif pc == LOAD:
                            self.assertEqual(machine.reg_read(reg.UC_MIPS_REG_A0), expected_codes[0])
                            machine.mem_write(physical(TABLE), bytes(glyph))
                        calls.append(pc)
                        for r in caller_regs:
                            machine.reg_write(r, 0xD00D0000 + r)
                        machine.reg_write(reg.UC_MIPS_REG_V0, result & 0xFFFFFFFF)
                        pc = machine.reg_read(reg.UC_MIPS_REG_RA)
                    else:
                        self.fail("loader did not return")
                    self.assertEqual(calls.count(READ), len(reads))
                    self.assertEqual(calls.count(POLL), len(polls))
                    self.assertEqual(calls.count(SRAND), 1)
                    self.assertEqual(calls.count(LOAD), 1)
                    self.assertEqual(random_calls, expected_random_calls)
                    self.assertEqual(bytes(machine.mem_read(physical(CODES), 69)), bytes(expected_codes))
                    self.assertEqual(bytes(machine.mem_read(physical(FLAGS), 4)),
                                     bytes((1, 2, expected_codes[0], slot)))
                    self.assertEqual(int.from_bytes(machine.mem_read(physical(CURRENT), 4), "little"), TABLE)
                    self.assertEqual(int.from_bytes(machine.mem_read(physical(SEED), 4), "little"), seed)
                    self.assertEqual(machine.reg_read(reg.UC_MIPS_REG_V0), expected_return)
                    self.assertEqual(machine.reg_read(reg.UC_MIPS_REG_SP), 0x801F0000)
                    self.assertEqual([machine.reg_read(r) for r in saved_regs],
                                     [0xCAFE0000 + r for r in saved_regs])


if __name__ == "__main__":
    unittest.main()

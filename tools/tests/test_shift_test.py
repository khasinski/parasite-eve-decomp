import unittest

from tools.scripts import shift_test as st


class ShiftTestHelpers(unittest.TestCase):
    SCRIPT = (
        "    _gp = 0x8009CD70;\n"
        "    .main 0x80010000 : AT(main_ROM_START) SUBALIGN(4)\n"
        "    {\n"
        "        FILL(0x00000000);\n"
        "        main_RODATA_START = .;\n"
        "    }\n"
        "    .field_engine 0x800C1CA0 : AT(field_engine_ROM_START) SUBALIGN(4)\n"
    )

    def test_pad_script_reports_and_rebases_fixed_placements(self):
        plain, pinned = st.pad_script(self.SCRIPT, 0x10, rebase=False)
        self.assertIn(". += 0x10;", plain)
        self.assertEqual(pinned, [".field_engine @ 0x800C1CA0", "_gp = 0x8009CD70"])
        self.assertIn(".field_engine 0x800C1CA0 :", plain)
        rebased, _ = st.pad_script(self.SCRIPT, 0x10, rebase=True)
        self.assertIn(".field_engine (0x800C1CA0 + 0x10) :", rebased)
        self.assertIn("_gp = (0x8009CD70 + 0x10);", rebased)
        self.assertIn(".main 0x80010000 :", rebased)

    def test_explained_relocation_fields(self):
        self.assertTrue(st.explained(st.R_MIPS_32, 0x80012000, 0x80012010, 0x10))
        self.assertTrue(st.explained(st.R_MIPS_26, 0x0C004000, 0x0C004004, 0x10))
        self.assertTrue(st.explained(st.R_MIPS_LO16, 0x24427FF8, 0x24428008, 0x10))
        self.assertTrue(st.explained(st.R_MIPS_HI16, 0x3C028001, 0x3C028002, 0x10))
        self.assertFalse(st.explained(st.R_MIPS_32, 0x80012000, 0x80012020, 0x10))


if __name__ == "__main__":
    unittest.main()

import unittest

from tools.scripts import shift_audit as sa


class ParseAssignmentsTests(unittest.TestCase):
    def test_absolute_symbolic_and_provide(self):
        rows = sa.parse_assignments(
            "/* comment = 0x1; */\n"
            "D_80012018 = 0x80012018;\n"
            "CdRom_Init = Akao_SetCdMixVolume;\n"
            "PROVIDE(g_Foo = 0x8009CD70);\n"
        )
        self.assertEqual([(r.name, r.value, r.provide) for r in rows], [
            ("D_80012018", 0x80012018, False),
            ("CdRom_Init", None, False),
            ("g_Foo", 0x8009CD70, True),
        ])


class RegionTests(unittest.TestCase):
    def test_regions(self):
        self.assertEqual(sa.address_region(0x1F800010), "scratchpad")
        self.assertEqual(sa.address_region(0x1F801D88), "hardware")
        self.assertEqual(sa.address_region(0xBFC00000), "hardware")
        self.assertEqual(sa.address_region(0x80000004), "kernel")
        self.assertEqual(sa.address_region(0xA000DF80), "kernel")
        self.assertIsNone(sa.address_region(0x8009CD70))


class MapAndLayoutTests(unittest.TestCase):
    MAP = (
        " .rodata        0x80010000       0x60 build/USA/src/main/a.c.o\n"
        " .text.ratan2\n"
        "                0x80020000       0x40 build/USA/src/main/b.c.o\n"
        " .data          0x80030000       0x10 build/USA/asm/USA/main/data/z.data.s.o\n"
        " .data          0x80030010        0x0 build/USA/src/main/empty.c.o\n"
    )

    def test_parse_map_wrapped_names_and_skips_empty(self):
        secs = sa.parse_map_sections(self.MAP)
        self.assertEqual([(s.start, s.section) for s in secs], [
            (0x80010000, ".rodata"), (0x80020000, ".text.ratan2"), (0x80030000, ".data")])

    def test_location_kind(self):
        image = bytes(0x20010)  # zero bytes cover the asm blob
        layout = sa.Layout(sa.parse_map_sections(self.MAP), image, 0x80010000)
        self.assertEqual(sa.location_kind(0x80010004, layout, []), "data_c_unit")
        self.assertEqual(sa.location_kind(0x80020010, layout, []), "function")
        self.assertEqual(sa.location_kind(0x80030004, layout, []), "zero_blob")
        self.assertEqual(sa.location_kind(0x80040000, layout, []), "unplaced")
        self.assertEqual(sa.location_kind(0x80040000, layout, [(0x80040000, 0x80041000, "o")]),
                         "overlay_window")
        self.assertEqual(sa.location_kind(0x80300000, layout, []), "outside_image")


class SourceScanTests(unittest.TestCase):
    def test_aliases_literals_and_register_pins(self):
        found = sa.scan_source(
            'extern int a asm("D_8009CDDC");\n'
            'extern int b __asm__("g_GameState");\n'
            'register int *p asm("$10");\n'
            'void f(void) { register int x asm("s0"); *(u16 *)0x800F3368 = 1; y = 0x8009AF00; }\n'
            '/* 0x80012345 in a comment */ z = 0x80000000;\n'
        )
        self.assertEqual(found["alias"], ["D_8009CDDC", "g_GameState"])
        self.assertEqual(found["int_to_pointer_literal"], ["0x800F3368"])
        self.assertEqual(found["program_literal"], ["0x8009AF00"])

    def test_alias_classification_uses_pins(self):
        found = {"alias": ["D_8009CDDC", "D_8009D120", "g_GameState"]}
        sa.classify_aliases(found, {"D_8009CDDC"})
        self.assertEqual(found["alias_to_pinned"], ["D_8009CDDC"])
        self.assertEqual(found["alias_address_named_owned"], ["D_8009D120"])
        self.assertEqual(found["alias_named_owned"], ["g_GameState"])


class UnrelocatedTests(unittest.TestCase):
    def test_data_and_text(self):
        words = [0x8009CD70, 0x00001234, 0x3C02800A, 0x0C004000]
        data = b"".join(w.to_bytes(4, "little") for w in words)
        base = 0x80010000
        self.assertEqual([(a, k) for a, _, k in sa.find_unrelocated(data, base, set(), text=False)],
                         [(0x80010000, "data_word")])
        got = [(a, k) for a, _, k in sa.find_unrelocated(data, base, {0x80010008}, text=True)]
        self.assertEqual(got, [(0x8001000C, "jump")])


if __name__ == "__main__":
    unittest.main()

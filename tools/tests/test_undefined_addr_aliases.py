import pathlib
import tempfile
import unittest

from tools.scripts import gen_undefined_addr_aliases


class UndefinedAddressAliasTests(unittest.TestCase):
    def test_real_definitions_cancel_references_but_common_stays_unresolved(self):
        undefined, defined = gen_undefined_addr_aliases.parse_symbol_table("""
         U SharedFunction
00000000 T SharedFunction
00000004 C D_80090000
         U D_80090004
""")

        self.assertEqual(undefined - defined, {"D_80090000", "D_80090004"})
        self.assertIn("SharedFunction", defined)

    def test_reads_absolute_addresses_from_splat_catalogue(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "undefined.txt"
            path.write_text("D_80090000 = 0x80090000;\n")

            addresses = gen_undefined_addr_aliases.known_addresses([path])

        self.assertEqual(addresses, {"D_80090000": 0x80090000})


if __name__ == "__main__":
    unittest.main()

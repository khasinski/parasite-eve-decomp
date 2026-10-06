import pathlib
import tempfile
import unittest

from tools.scripts import overlay_extra_undefineds


class LinkedObjectsTests(unittest.TestCase):
    def test_reads_only_the_objects_the_script_links(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            build = root / "build" / "USA" / "overlays" / "room_x"
            script = root / "room_x.ld"
            script.write_text(
                "SECTIONS {\n"
                f"    {build}/src/overlays/room_lib/Lib.c.o(.rodata);\n"
                f"    {build}/src/overlays/room_lib/Lib.c.o(.text);\n"
                f"    {build}/asm/USA/overlays/room_x/data/D_1.data.s.o(.data);\n"
                "}\n")
            objects = overlay_extra_undefineds.linked_objects(script, build)

        self.assertEqual(objects, [
            build / "asm/USA/overlays/room_x/data/D_1.data.s.o",
            build / "src/overlays/room_lib/Lib.c.o",
        ])


if __name__ == "__main__":
    unittest.main()

import pathlib
import unittest
from unittest.mock import patch

from tools.scripts import try_drop_pins


ROOT = pathlib.Path(__file__).resolve().parents[2]

# Lines whose pin match used to swallow every asm except the last one.
CLAUSE_LINES = (
    ("src/main/psyq/libpad/padportd_outputs.c", 6, ('asm("$7")', 'asm("$6")')),
    ("src/main/render/Render_DrawObject.c", 94, ('asm("$20")', 'asm("$21")', 'asm("$22")')),
    ("src/main/render/Render_SetupBoneTransforms.c", 137, ('asm("$12")', 'asm("$13")')),
    (
        "src/overlays/scene_e20/RoomEffect_HoverOrbController_8018F750.c",
        302,
        ('asm("$12")', 'asm("$13")', 'asm("$14")'),
    ),
    (
        "src/overlays/scene_e20/RoomEffect_HoverOrbController_8018F750.c",
        326,
        ('asm("$12")', 'asm("$13")', 'asm("$14")'),
    ),
)


class _Baseline(object):
    def unlink(self):
        pass


class TryDropPinsClauseTests(unittest.TestCase):
    def test_main_tries_every_asm_clause_on_the_line(self):
        for rel, lineno, expected in CLAUSE_LINES:
            with self.subTest(rel=rel, line=lineno):
                original = (ROOT / rel).read_text()
                attempted = []

                def try_edit(src, text, candidate, baseline):
                    attempted.append(candidate)
                    return False

                with patch.object(try_drop_pins, "snapshot", return_value=_Baseline()), \
                     patch.object(try_drop_pins, "try_edit", side_effect=try_edit), \
                     patch.object(try_drop_pins, "compile_one", return_value=True):
                    try_drop_pins.main([rel])

                self.assertEqual((ROOT / rel).read_text(), original)
                removed = []
                for candidate in attempted:
                    before = original.splitlines()[lineno - 1]
                    after = candidate.splitlines()[lineno - 1]
                    if before == after:
                        continue
                    gone = [clause for clause in expected if clause not in after]
                    kept = [clause for clause in expected if clause in after]
                    self.assertEqual(len(gone), 1)
                    self.assertEqual(kept, [clause for clause in expected if clause != gone[0]])
                    removed.append(gone[0])
                self.assertEqual(tuple(removed), expected)

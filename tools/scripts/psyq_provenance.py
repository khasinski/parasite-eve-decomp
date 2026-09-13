"""Address-based SDK ownership, independent of source language or directory.

Evidence contains full retail-range hashes and comparisons to Psy-Q signatures.
Relocation wildcards identify provenance, never decompilation progress.
"""
import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]


def load():
    return json.loads((ROOT / 'configs/USA/psyq_provenance.json').read_text())['evidence']


def identity(ranges, fallback, evidence):
    """Return a stable SDK display path only when every text byte is covered.

    Ambiguous object identities still prove SDK ownership but retain a neutral
    display path. Several SDK objects in one TU likewise stay one report unit.
    """
    if not ranges:
        return None
    owners = []
    for start, size in ranges:
        end = start + size
        overlaps = [e for e in evidence
                    if int(e['address'], 16) < end and
                    int(e['address'], 16) + e['size'] > start]
        cursor = start
        for e in sorted(overlaps, key=lambda x: int(x['address'], 16)):
            left = int(e['address'], 16)
            right = left + e['size']
            if left > cursor:
                return None
            cursor = max(cursor, right)
        if cursor < end:
            return None
        owners.extend(overlaps)
    libraries = {e['library'] for e in owners}
    objects = {(e['library'], e['object']) for e in owners}
    if len(objects) != 1:
        library = next(iter(libraries)).lower() if len(libraries) == 1 else 'shared'
        return 'main/psyq/%s/%s' % (library, fallback.removeprefix('main/'))
    library, obj = next(iter(objects))
    start = min(a for a, _size in ranges)
    labels = {label['name'] for e in owners for label in e['labels']
              if int(e['address'], 16) + label['offset'] == start}
    public = sorted(name for name in labels if not name.startswith('text_'))
    label = (public or sorted(labels) or ['part_%08X' % start])[0]
    return 'main/psyq/%s/%s/%s' % (library.lower(), obj, label)

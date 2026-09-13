#!/usr/bin/env python3
"""Verify SDK attribution against retail and, optionally, external signatures."""
import argparse
import hashlib
import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--retail', type=pathlib.Path, default=ROOT / 'assets/USA/main.exe')
    parser.add_argument('--sdk-dir', type=pathlib.Path,
                        help='psyq-sigs directory containing 400/, 410/, etc.')
    args = parser.parse_args()
    metadata = json.loads((ROOT / 'configs/USA/psyq_provenance.json').read_text())
    retail = args.retail.read_bytes()
    assert hashlib.sha1(retail).hexdigest() == metadata['retail_sha1'], 'wrong retail image'
    for e in metadata['evidence']:
        offset = int(e['address'], 16) - 0x8000F800
        data = retail[offset:offset + e['size']]
        assert len(data) == e['size']
        assert hashlib.sha256(data).hexdigest() == e['retail_sha256'], e
        if args.sdk_dir:
            path = args.sdk_dir / e['sdk_version'] / (e['library'] + '.LIB.json')
            assert hashlib.sha256(path.read_bytes()).hexdigest() == e['signature_sha256'], path
            obj = next(x for x in json.loads(path.read_text())
                       if x['name'] == e['object'] + '.OBJ')
            start = e['sdk_offset']
            sig = obj['sig'].split()[start:start + e['size']]
            assert len(sig) == len(data)
            assert sum(x != '??' for x in sig) == e['known_bytes']
            assert all(x == '??' or int(x, 16) == b for x, b in zip(sig, data)), e
            for label in e['labels']:
                assert dict(name=label['name'], offset=label['offset'] + start) in obj['labels'], label
    print('OK: %d SDK provenance ranges verified%s.' %
          (len(metadata['evidence']), ' against signatures' if args.sdk_dir else ' against retail'))


if __name__ == '__main__':
    main()

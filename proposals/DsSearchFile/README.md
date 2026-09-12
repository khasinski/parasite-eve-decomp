# LIBDS file lookup

Production `DsSearchFile` at `0x80081414` matches all **736/736 linked bytes**
using native GCC 2.8.1, unsplit addresses, and unmodified GNU as.

The function refreshes the media cache, resolves up to eight path components,
then searches the 64-entry file cache. On success it copies the 24-byte
`DslFILE` to the caller and returns the cached entry pointer. Layout assertions
check the record size and filename offset. The copy retains the retail
four-word/two-word load-store ordering.

Seven register pins and seven empty barriers remain, all counted in the debt
baseline. They preserve the output pointer, independent filename/cache
induction variables, and copy register allocation/order. The pinned hardware
`$zero` is read through an empty output constraint for the independent offset.
There is no instruction asm, compiler/assembler patch, or binary rewriting.

`tools/tests/test_sector_file_search.py` checks every linked byte against the
retail SHA-256. The existing `verify_behavior.py` also passes all 768 cases for
return pointers, copied records, media state, and modeled external-call traces:

```sh
tools/scripts/cc.sh src/main/psyq/libds/DsSearchFile.c /tmp/ds-search.o
.venv/bin/python proposals/DsSearchFile/verify_behavior.py /tmp/ds-search.o
```

The older multi-function reconstruction in `../dsfile/candidate.c` is retained
as a historical experiment; production uses the source above.

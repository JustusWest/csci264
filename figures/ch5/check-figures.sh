#!/bin/sh
# Report figure references in _lectures that have no file on disk.
# Run from the repo root:  sh figures/ch5/check-figures.sh
missing=0
grep -rho '/figures/[A-Za-z0-9._/-]*' _lectures/ | sort -u | while read -r ref; do
    path=".${ref}"
    if [ ! -f "$path" ]; then
        echo "MISSING  $ref"
        missing=1
    fi
done
[ "$missing" -eq 0 ] && echo "(any MISSING lines above need downloading; none means all figures resolve)"

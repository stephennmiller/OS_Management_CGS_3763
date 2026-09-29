#!/usr/bin/env bash
# Build with warnings + sanitizers and run scripted checks against the simulator.
# Usage: scripts/check.sh   (run from anywhere)

set -u
cd "$(dirname "$0")/.."

BUILD_DIR=$(mktemp -d)
BIN="$BUILD_DIR/osm"
trap 'rm -rf "$BUILD_DIR"' EXIT
export TERM=dumb

fails=0
pass() { printf 'PASS  %s\n' "$1"; }
fail() { printf 'FAIL  %s\n' "$1"; fails=$((fails + 1)); }

# Run the binary with a time limit (macOS has no `timeout`).
run() { perl -e 'alarm shift; exec @ARGV' 5 "$BIN"; }

# 1. Build must produce zero warnings
warnings=$(clang -Wall -Wextra -pedantic -std=c11 -g -fsanitize=address,undefined \
    -o "$BIN" OSManagement.c -pthread 2>&1)
if [ $? -ne 0 ]; then
    echo "$warnings"; echo "FAIL  build"; exit 1
fi
if [ -z "$warnings" ]; then pass "build has no warnings"; else echo "$warnings"; fail "build has no warnings"; fi

# 2. Closed stdin exits immediately with status 0
run < /dev/null > /dev/null 2>&1
status=$?
if [ $status -eq 0 ]; then pass "exits on end of input"; else fail "exits on end of input (status $status)"; fi

# 3. Remaining Space shows the space left at the time each process was placed
# Prints "algorithm: rem1 rem2 ..." for each table in the memory output.
remaining=$(printf '1\n\n0\n' | run 2>&1 | awk '
    /\*+ .* Fit \*+/ { if (name) print name ":" rows; name = $2; rows = ""; next }
    /^[0-9]+\t/ && name { n = split($0, f, /\t+/); rows = rows " " (f[3] == "Not Allocated" ? "-" : f[n]) }
    END { if (name) print name ":" rows }')
expected='First: 5 0 0 5 15
Best: 0 0 10 5 15
Worst: 70 50 45 15 -
Next: 5 0 30 50 -'
if [ "$remaining" = "$expected" ]; then
    pass "remaining space per allocation"
else
    fail "remaining space per allocation"; echo "  expected:"; echo "$expected" | sed 's/^/    /'
    echo "  got:"; echo "$remaining" | sed 's/^/    /'
fi

# 3b. Block Size column shows the original size of the block each process went to
sizes=$(printf '1\n\n0\n' | run 2>&1 | awk '
    /\*+ .* Fit \*+/ { if (name) print name ":" rows; name = $2; rows = ""; next }
    /^[0-9]+\t/ && name { split($0, f, /\t+/); rows = rows " " (f[3] == "Not Allocated" ? "-" : f[4]) }
    END { if (name) print name ":" rows }')
expected='First: 15 20 15 35 80
Best: 10 20 15 35 80
Worst: 80 80 80 80 -
Next: 15 20 35 80 -'
if [ "$sizes" = "$expected" ] && printf '1\n\n0\n' | run 2>&1 | grep -q 'Block Size'; then
    pass "block size column"
else
    fail "block size column"; echo "  expected:"; echo "$expected" | sed 's/^/    /'
    echo "  got:"; echo "$sizes" | sed 's/^/    /'
fi

# 3c. Menu rejects a number followed by junk
out=$(printf '1abc\n0\n' | run 2>&1)
if grep -q 'Please enter a number' <<< "$out" && ! grep -q '\* Memory Management \*' <<< "$out"; then
    pass "menu rejects trailing junk"
else
    fail "menu rejects trailing junk"
fi

# 3d. Clearing the screen does not depend on TERM
err=$(printf '3\n\n0\n' | env -u TERM perl -e 'alarm shift; exec @ARGV' 5 "$BIN" 2>&1 >/dev/null)
if [ -z "$err" ]; then pass "no stderr output with TERM unset"; else fail "no stderr output with TERM unset: $err"; fi

# 4. File listing survives end of input at the y/n prompt
out=$(printf '2\n\n' | run 2>&1); status=$?
if [ $status -eq 0 ] && grep -q 'File: OSManagement.c' <<< "$out"; then
    pass "file listing with input ending at prompt"
else
    fail "file listing with input ending at prompt (status $status)"
fi

# 5. Names-only listing omits details
out=$(printf '2\nn\n\n0\n' | run 2>&1)
if grep -q 'File: OSManagement.c' <<< "$out" && ! grep -q 'Permissions:' <<< "$out"; then
    pass "names-only file listing"
else
    fail "names-only file listing"
fi

# 6. Both threads run to completion
out=$(printf '3\n\n0\n' | run 2>&1)
if grep -q 'Thread 1 completed' <<< "$out" && grep -q 'Thread 2 completed' <<< "$out"; then
    pass "threads complete"
else
    fail "threads complete"
fi

echo
if [ $fails -eq 0 ]; then echo "All checks passed"; else echo "$fails check(s) failed"; fi
exit $fails

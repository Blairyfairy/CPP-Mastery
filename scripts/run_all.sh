#!/usr/bin/env bash
# Build and run every example WITHOUT CMake (g++ or clang++ only).
# Usage: scripts/run_all.sh [compiler] [extra flags...]   e.g. scripts/run_all.sh clang++ -fsanitize=address
set -u
CXX="${1:-g++}"; shift || true
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$(mktemp -d)"
pass=0; fail=0
for f in "$ROOT"/src/*/*.cpp; do
  name="$(basename "$(dirname "$f")")__$(basename "$f" .cpp)"
  if "$CXX" -std=c++17 -Wall -Wextra -Wpedantic -I"$ROOT/include" -pthread "$@" "$f" -o "$OUT/$name" 2>"$OUT/$name.err"; then
    if "$OUT/$name" >"$OUT/$name.log" 2>&1; then printf 'PASS  %s\n' "$name"; pass=$((pass+1))
    else printf 'FAIL  %s (runtime)\n' "$name"; tail -5 "$OUT/$name.log"; fail=$((fail+1)); fi
  else printf 'FAIL  %s (compile)\n' "$name"; head -10 "$OUT/$name.err"; fail=$((fail+1)); fi
done
echo "passed=$pass failed=$fail"
[ "$fail" -eq 0 ]

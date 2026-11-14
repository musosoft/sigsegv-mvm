#!/usr/bin/env bash
set -euo pipefail
export UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
cd "$(dirname "$0")/.."
mkdir -p build/validation
set -x
gcc -c tests/func_replace_probe.S -o build/validation/func_replace_probe.o
for cxx in g++ clang++; do
    for mode in normal sanitized; do
        flags=(-O2)
        if [[ $mode == sanitized ]]; then
            flags=(-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -no-pie)
        fi
        if [[ $cxx == clang++ ]]; then flags+=(-Wno-unused-private-field); fi
        "$cxx" -std=c++20 -Wall -Wextra -Werror -Wno-unused-parameter \
            -Wno-sign-compare -Wno-type-limits "${flags[@]}" \
            -DPLATFORM_64BITS -include tests/patch_test_support.h -Itests/stubs -Isrc \
            tests/func_replace_jump.cpp src/mem/patch.cpp build/validation/func_replace_probe.o \
            -o "build/validation/test-$cxx-$mode"
        "build/validation/test-$cxx-$mode"
    done
done

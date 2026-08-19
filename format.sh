#!/usr/bin/env bash
set -euo pipefail

find src lib tests -type f \( -name '*.cc' -o -name '*.hh' \) \
    -exec clang-format-19 --style=Microsoft -i {} +

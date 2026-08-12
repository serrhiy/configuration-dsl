#!/usr/bin/env bash
set -euo pipefail

find src -type f -name '*.cc' -exec clang-format-19 --style=Google -i {} +

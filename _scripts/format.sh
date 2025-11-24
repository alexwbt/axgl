#! /bin/bash

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"

FILE_PATTERN="^$ROOT_DIR/\(axgl\|demo\|impl\|lib\).*\.\(cpp\|hpp\)$"

find $ROOT_DIR \
  -regex "$FILE_PATTERN" \
  -exec clang-format -i {} \;

find $ROOT_DIR \
  -regex "$FILE_PATTERN" \
  -exec clang-tidy {} -fix -- -std=c++20 \;

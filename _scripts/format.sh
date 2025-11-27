#! /bin/bash

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"

FILE_PATTERN="^$ROOT_DIR/\(axgl\|demo\|impl\|lib\).*\.\(cpp\|hpp\)$"

if [ "$1" != "--no-tidy" ]; then
  find $ROOT_DIR \
    -regex "$FILE_PATTERN" \
    -exec clang-tidy {} -fix -fix-errors -- -std=c++20 \;
fi

find $ROOT_DIR \
  -regex "$FILE_PATTERN" \
  -exec clang-format -i {} \;

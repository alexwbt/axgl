#! /bin/bash

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"
ROOT_DIRNAME="$(basename "$ROOT_DIR")"

FILE_PATTERN="(axgl|demo|impl|lib)/.*"


if [ "$1" != "--no-tidy" ]; then
  find $ROOT_DIR \
    -regextype posix-extended \
    -regex "^$ROOT_DIR/$FILE_PATTERN\\.cpp$" \
    -exec \
    clang-tidy {} \
    -fix \
    -fix-errors \
    -p _build/Debug \
    -header-filter=".*$ROOT_DIRNAME/$FILE_PATTERN\\.hpp$" \;
fi

find $ROOT_DIR \
  -regextype posix-extended \
  -regex "^$ROOT_DIR/$FILE_PATTERN\\.(cpp|hpp)$" \
  -exec clang-format -i {} \;

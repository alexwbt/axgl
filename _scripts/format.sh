#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"
ROOT_DIRNAME="$(basename "$ROOT_DIR")"

FILE_PATTERN="(axgl|demo|impl|lib)/.*"
CLANG_FORMAT=clang-format

[ -f "$ROOT_DIR/.env" ] && . "$ROOT_DIR/.env"

#
# Mode parsing
#
#   no args                       -> full-tree clang-tidy + clang-format
#   --no-tidy                     -> full-tree clang-format only
#   --files <file-1> <file-2> ... -> clang-format only the listed files
#
FILES_MODE=0
RUN_TIDY=1
if [ "$1" = "--files" ]; then
  FILES_MODE=1
  RUN_TIDY=0
  shift
fi
if [ "$1" = "--no-tidy" ]; then
  RUN_TIDY=0
  shift
fi

if [ "$RUN_TIDY" -eq 1 ]; then
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

#
# clang-format version check
#
REQUIRED_CLANG_FORMAT_MAJOR=20
if [ -z "$CLANG_FORMAT_SKIP_VERSION_CHECK" ]; then
  CF_VERSION=$($CLANG_FORMAT --version 2>/dev/null)
  CF_MAJOR=$(echo "$CF_VERSION" | sed -n 's/.*version \([0-9][0-9]*\)\..*/\1/p')
  if [ -z "$CF_MAJOR" ]; then
    echo "error: $CLANG_FORMAT not found" >&2
    exit 1
  fi
  if [ "$CF_MAJOR" != "$REQUIRED_CLANG_FORMAT_MAJOR" ]; then
    echo "error: $CLANG_FORMAT $REQUIRED_CLANG_FORMAT_MAJOR required, found $CF_MAJOR ($CF_VERSION)" >&2
    exit 1
  fi
fi

if [ "$FILES_MODE" -eq 1 ]; then
  if [ $# -gt 0 ]; then
    $CLANG_FORMAT -i "$@"
  fi
else
  find $ROOT_DIR \
    -regextype posix-extended \
    -regex "^$ROOT_DIR/$FILE_PATTERN\\.(cpp|hpp|fs|vs)$" \
    -exec $CLANG_FORMAT -i {} \;
fi

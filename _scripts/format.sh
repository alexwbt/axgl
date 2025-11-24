#! /bin/bash

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"

find $ROOT_DIR \
  -regex "^$ROOT_DIR/\(axgl\|demo\|impl\|lib\).*\.\(cpp\|hpp\)$" \
  -exec clang-format -i {} \;

find $ROOT_DIR \
  -regex "^$ROOT_DIR/\(axgl\|demo\|impl\|lib\).*\.\(cpp\|hpp\)$" \
  -exec clang-tidy {} -fix -fix-errors -- -std=c++20 \;

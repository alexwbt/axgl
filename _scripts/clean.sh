#! /bin/bash

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"

rm -rf \
  $ROOT_DIR/_build \
  $ROOT_DIR/_bin

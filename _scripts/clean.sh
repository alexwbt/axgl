#! /bin/bash

DIR="$(cd "$(dirname "$0")" && pwd)"

rm -rf \
  $DIR/../_build \
  $DIR/../_bin \
  $DIR/../_out \
  $DIR/../build \
  $DIR/../bin \
  $DIR/../out

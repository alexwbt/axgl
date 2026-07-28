#! /bin/bash

PRESET=${1:-debug}
shift

TARGET=""
NO_CONFIG=0

for arg in "$@"; do
  if [ "$arg" = "--no-config" ]; then
    NO_CONFIG=1
  elif [ -z "$TARGET" ]; then
    TARGET="$arg"
  fi
done

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"
LOG_DIR="$ROOT_DIR/_log"
LOG_FILE="$LOG_DIR/build.log"
ARCHIVE_LOG_FILE="$LOG_DIR/$(date +"%Y-%m-%d_%H-%M-%S")_${PRESET}${TARGET:+_$TARGET}.log"

log() {
  if [ -t 0 ]; then
    echo "$*" | tee -a "$LOG_FILE" | tee -a "$ARCHIVE_LOG_FILE"
  else
    cat - | tee -a "$LOG_FILE" | tee -a "$ARCHIVE_LOG_FILE"
  fi
}

mkdir -p $LOG_DIR
rm -f $LOG_FILE

cd $ROOT_DIR

if [ $NO_CONFIG -eq 0 ]; then
  log
  log "##########"
  log "########## Starting cmake config ($PRESET)"
  log "##########"

  cmake --preset $PRESET | log
fi

log
log "##########"
log "########## Starting cmake build ($PRESET${TARGET:+ target=$TARGET})"
log "##########"

if [ -n "$TARGET" ]; then
  cmake --build --preset $PRESET --target "$TARGET" | log
else
  cmake --build --preset $PRESET | log
fi
#! /bin/bash

PRESET=${1:-debug}

ROOT_DIR="$(cd "$(dirname "$0")" && cd .. && pwd)"
LOG_DIR="$ROOT_DIR/_log"
LOG_FILE="$LOG_DIR/build.log"
ARCHIVE_LOG_FILE="$LOG_DIR/$(date +"%Y-%m-%d_%H-%M-%S")_$PRESET.log"

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

if [ "$2" != "--no-config" ]; then
  log
  log "##########"
  log "########## Starting cmake config ($PRESET)"
  log "##########"

  cmake --preset $PRESET | log
fi

log
log "##########"
log "########## Starting cmake build ($PRESET)"
log "##########"

cmake --build --preset $PRESET | log

#! /bin/bash

PRESET="debug"
TARGET=""
NO_CONFIG=0
NO_BUILD=0

while [ $# -gt 0 ]; do
  case "$1" in
  --no-config)
    NO_CONFIG=1
    shift
    ;;
  --no-build)
    NO_BUILD=1
    shift
    ;;
  --target)
    if [ $# -lt 2 ]; then
      echo "error: --target requires an argument" >&2
      exit 1
    fi
    TARGET="$2"
    shift 2
    ;;
  -*)
    echo "error: unknown option '$1'" >&2
    exit 1
    ;;
  *)
    PRESET="$1"
    shift
    ;;
  esac
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
  log "#"
  log "# Starting cmake config ($PRESET)"
  log "#"

  cmake --preset $PRESET | log
fi

if [ $NO_BUILD -eq 0 ]; then
  log
  log "#"
  log "# Starting cmake build ($PRESET${TARGET:+ target=$TARGET})"
  log "#"

  if [ -n "$TARGET" ]; then
    cmake --build --preset $PRESET --target "$TARGET" | log
  else
    cmake --build --preset $PRESET | log
  fi
fi

#! /bin/bash

PRESET=${1:-debug}

DIR="$(cd "$(dirname "$0")" && pwd)"
LOG_DIR="$DIR/../_log"
LOG_FILE="$LOG_DIR/build.log"
ARCHIVE_LOG_FILE="$LOG_DIR/$(date +"%Y-%m-%d_%H-%M-%S")_$PRESET.log"

mkdir -p $LOG_DIR
rm -f $LOG_FILE

cd $DIR/..

echo | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "########## Starting cmake config ($PRESET)" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

cmake --preset $PRESET |
  tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

echo | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "########## Starting cmake build ($PRESET)" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

cmake \
  --build \
  --preset $PRESET |
  tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

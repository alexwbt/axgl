#! /bin/sh

DIR="$(cd "$(dirname "$0")" && pwd)"

TIME=$(date +"%Y-%m-%d_%H-%M-%S")

BUILD_TYPE=${1:-Release}

LOG_DIR="_log"
LOG_FILE="$LOG_DIR/build.log"
ARCHIVE_LOG_FILE="$LOG_DIR/$TIME.log"

mkdir -p $LOG_DIR
rm -f $LOG_FILE

echo | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "########## Starting cmake config ($BUILD_TYPE)" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

cmake \
  -B $DIR/../_build \
  -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
  -DFLATBUFFERS_BUILD_TESTS=OFF \
  -DCPPTRACE_BUILD_TESTING=OFF \
  -DARGS_BUILD_UNITTESTS=OFF \
  -DFLATBUFFERS_INSTALL=OFF \
  -DARGS_BUILD_EXAMPLE=OFF \
  -DASSIMP_BUILD_TESTS=OFF \
  -DGLFW_BUILD_DOCS=OFF \
  -DASSIMP_INSTALL=OFF |
  tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

echo | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "########## Starting cmake build ($BUILD_TYPE)" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

cmake \
  --build $DIR/../_build \
  --config $BUILD_TYPE |
  tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

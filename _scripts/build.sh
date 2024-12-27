#! /bin/bash

BUILD_TYPE=${1:-Debug}
VALID_OPTIONS=("Debug" "Release" "RelWithDebInfo" "MinSizeRel")

validate_input() {
  for option in "${VALID_OPTIONS[@]}"; do
    if [[ "$BUILD_TYPE" == "$option" ]]; then
      return
    fi
  done
  echo "Invalid input: $BUILD_TYPE"
  echo "Valid options are: ${VALID_OPTIONS[*]}"
  exit 1
}
validate_input

DIR="$(cd "$(dirname "$0")" && pwd)"
TIME=$(date +"%Y-%m-%d_%H-%M-%S")
LOG_DIR="$DIR/../_log"
LOG_FILE="$LOG_DIR/build.log"
ARCHIVE_LOG_FILE="$LOG_DIR/$TIME.log"

mkdir -p $LOG_DIR
rm -f $LOG_FILE

echo | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "########## Starting cmake config ($BUILD_TYPE)" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE
echo "##########" | tee -a $LOG_FILE | tee -a $ARCHIVE_LOG_FILE

if [ $BUILD_TYPE == "Debug" ]; then
  AXGL_DEBUG=ON
else
  AXGL_DEBUG=OFF
fi

cmake \
  -B $DIR/../_build \
  -DAXGL_DEBUG=${AXGL_DEBUG} \
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

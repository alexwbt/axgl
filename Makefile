ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

PRESET ?= debug
TARGET ?= demo_playground

CMAKE ?= cmake
CLEAN := $(ROOT)/_scripts/clean.sh
BUILD := $(ROOT)/_scripts/build.sh
FORMAT := $(ROOT)/_scripts/format.sh

.DEFAULT_GOAL := debug

.PHONY: all debug release target run format tidy clean setup help
.PHONY: demo_%

all: debug

debug:
	$(BUILD) debug

release:
	$(BUILD) release

target:
	$(BUILD) $(PRESET) --target $(TARGET) --no-config

demo_%:
	$(BUILD) $(PRESET) --target $@

run:
	@BIN="$(ROOT)/_bin/$(TARGET).exe"; \
	[ -f "$$BIN" ] || BIN="$(ROOT)/_bin/$(TARGET)"; \
	[ -f "$$BIN" ] || { echo "error: $(TARGET) not built" >&2; exit 1; }; \
	echo "== running $$BIN"; \
	"$$BIN"

format:
	$(FORMAT) --no-tidy

tidy:
	$(FORMAT)

clean:
	$(CLEAN)

setup:
	git lfs install && git submodule update --init --recursive

help:
	@echo "axgl build targets"
	@echo "  make                             build debug preset"
	@echo "  make release                     build release preset"
	@echo "  make target TARGET=<name>        build one target (default: demo_playground)"
	@echo "  make run [TARGET=<name>]         build and run (default: demo_playground)"
	@echo "  make <demo_target>               e.g. make demo_window"
	@echo "  make format                      clang-format"
	@echo "  make tidy                        clang-tidy + clang-format"
	@echo "  make clean                       remove _build/ and _bin/"
	@echo "  make setup                       init git submodules and LFS"

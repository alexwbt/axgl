ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

PRESET ?= debug
TARGET ?=
RUN_TARGET ?= demo_playground

CMAKE ?= cmake
CLEAN := $(ROOT)/_scripts/clean.sh
BUILD := $(ROOT)/_scripts/build.sh
FORMAT := $(ROOT)/_scripts/format.sh

.DEFAULT_GOAL := debug

.PHONY: all debug release configure target run format tidy clean setup help
.PHONY: demo_%

all: debug

debug:
	$(BUILD) debug

release:
	$(BUILD) release

configure:
	$(BUILD) $(PRESET)

target:
	$(BUILD) $(PRESET) --target $(TARGET)

demo_%:
	$(BUILD) $(PRESET) --target $@

run:
	$(BUILD) $(PRESET) --target $(RUN_TARGET)
	@BIN="$(ROOT)/_bin/$(RUN_TARGET).exe"; \
	[ -f "$$BIN" ] || BIN="$(ROOT)/_bin/$(RUN_TARGET)"; \
	[ -f "$$BIN" ] || { echo "error: $(RUN_TARGET) not built" >&2; exit 1; }; \
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
	@echo "  make target TARGET=<name>        build one target"
	@echo "  make run [RUN_TARGET=<name>]     build and run (default: demo_playground)"
	@echo "  make configure [PRESET=release]  cmake configure only"
	@echo "  make <demo_target>               e.g. make demo_window"
	@echo "  make format                      clang-format"
	@echo "  make tidy                        clang-tidy + clang-format"
	@echo "  make clean                       remove _build/ and _bin/"
	@echo "  make setup                       init git submodules and LFS"

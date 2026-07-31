# AGENTS.md

Notes for OpenCode sessions working in this repo. C++20 / CMake game-engine
sandbox on Windows (MSVC toolchain by default; MinGW also supported for net
demos).

## Setup (do this first, or builds fail silently)

- Submodules: `git submodule update --init --recursive`. All third-party deps
  live under `_external/` (glm, glfw, spdlog, assimp, asio, flatbuffers, tracy,
  freetype, args, cpptrace, utfcpp, stb, imgui). They are built as part
  of the CMake tree, not fetched.
- Git LFS is required. `core.hooksPath` is set to `.githooks`; the hooks abort
  checkout/commit/merge/push if `git-lfs` is missing. LFS tracks `*.png`,
  `*.ttf`, `*.assbin` (see `.gitattributes`). Without LFS, demo assets in
  `demo/**/res/` will be pointer files, not real data.

## Build

- Use the wrapper: `_scripts/build.sh [debug|release] [--target <target>] [--no-config]`
  (logs to `_log/build.log` plus a timestamped archive in `_log/`). `--no-config`
  skips the configure step. The default preset is `debug`. Use `--target <target>`
  (e.g. `--target demo_blending`) to build only that target; `--no-config` can
  appear in any position. Preset may be given as a positional arg (`debug`);
  flags can appear in any position. When testing a change, build the `debug`
  preset only (it is the default).
- Check `_log/build.log` for errors/output instead of relying on stdout, which
  may be empty, truncated, or intermixed with runtime output. Use the Read
  tool (or Grep) on `_log/build.log`, not `tail`/`cat` via Bash.
- Binaries go to `_bin/`; build artifacts to `_build/{Debug,Release}`.
  `CMAKE_EXPORT_COMPILE_COMMANDS` is ON, so `compile_commands.json` lands in
  `_build/Debug` — this is what `.clangd` and clang-tidy (`-p _build/Debug`)
  read. If you wipe `_build/`, clangd/clang-tidy break until you reconfigure.
- Clean: `_scripts/clean.sh` (removes `_build/` and `_bin/`).
- Debug preset defines `AXGL_DEBUG` and `TRACY_ENABLE=ON`; release turns both
  off. See "Debug/profiling instrumentation" below — these flags change
  behavior, not just optimization.

## Format / lint

- Style is clang-format **Allman braces, 2-space, 120 col, pointers left**
  (`.clang-format`). Match this exactly; do not reflow to LLVM defaults.
- `pre-commit` hook (in `.githooks`) auto-runs `clang-format -i` on staged
  `.cpp/.hpp` under `axgl/`, `demo/`, `impl/`, `lib/` and re-stages them. If
  you commit from a shell that doesn't run hooks, run `_scripts/format.sh`
  first.
- `_scripts/format.sh` also runs `clang-tidy -fix -fix-errors` (pass
  `--no-tidy` to skip) across the same four trees, then clang-format. It
  requires a configured `_build/Debug` for the compile database.
- clang-tidy checks are pinned in `.clang-tidy` (large bugprone/modernize/
  performance/readability set). Don't disable checks ad hoc; edit the file.

## There is no test suite

No `test/`, no CTest, no framework. "Verification" = the project builds and a
demo runs. Don't invent test commands or claim coverage.

## Architecture

- `axgl/` — header-only INTERFACE library, namespace `axgl`. Core types:
  `Axgl` (`axgl.hpp`) is the engine entrypoint with `initialize()` / `run()` /
  `terminate()` and a fixed-timestep loop. `configure_default(axgl)`
  registers the default services. Services are split into
  `axgl/include/axgl/interface/` (abstract) and `axgl/include/axgl/impl/`
  (default implementations). `axgl/CMakeLists.txt` links `util`, `glm`,
  `spdlog`, `cpptrace`, `Tracy::TracyClient` as INTERFACE deps — include these
  via `axgl`, not directly from `_external/`.
- `lib/` — internal support libraries: `net` (asio wrapper, INTERFACE),
  `glad`, `util`, `assbin`, `embedfile`, `bundlefile`. `net` is standalone
  (no `axgl` dependency) and used by the net demos.
- `impl/` — platform/backend implementations, built as libraries:
  `axgl_glfw_impl` (INTERFACE), `axgl_opengl_impl` (static, embeds its `res/`),
  `axgl_assimp_impl`, `axgl_bundlefile_impl`. **Demos link an `impl` target,
  not `axgl` directly** — the impl transitively pulls in `axgl`.
- `demo/` — executables. `demo/CMakeLists.txt` auto-discovers subdirectories
  with a `CMakeLists.txt`, so each numbered demo (`01_window` ... `12_gui`) and
  `demo/net/*` is its own target. `demo/playground` is the scratch app.
  Net demos (`demo/net/1_tcp_ping`, `demo/net/2_chatroom`) link `net` and, on
  MinGW/Windows, need `ws2_32 mswsock`.

## Codegen / resource pipeline (easy to miss)

`_cmake/resource.cmake` defines three CMake functions used across the tree.
They generate files into the build dir and add them as sources — a clean
build is required after editing inputs or the generated code goes stale.

- `embed_resource(target source_dir)` — runs the `embedfile` tool to turn
  `source_dir/*` into `${CMAKE_CURRENT_BINARY_DIR}/resources/.../*.cpp/.hpp`,
  added as a source of `target`. Used by `axgl_opengl_impl` (`res/`) and
  `demo/playground` (`res/`). Include path root is `.../resources`.
- `bundle_resource(target source_dir)` — runs `bundlefile` to produce
  `_bin/${target}_${source_dir}.bin`.
- `compile_fbs(target source_dir)` — runs `flatc --cpp` (the `flatc` binary
  built in `_bin/`) on `*.fbs`, emitting `*_fbs.h` into
  `${CMAKE_CURRENT_BINARY_DIR}/flatbuffers/...` and adding it to the target's
  include dirs. Used by `lib/bundlefile` (its own `bundle.fbs`) and
  `demo/net/2_chatroom` (`fbs/`). When you add a `.fbs`, rerun configure so the
  glob + custom command regenerate.

The `flatc`, `embedfile`, `bundlefile` tools are built from source as part of
the normal build (see `lib/bundlefile`, `lib/embedfile`, `_external/flatbuffer`)
and placed in `_bin/`. Don't expect them on `PATH`.

## Debug / profiling instrumentation (changes semantics)

`axgl/include/axgl/common.hpp` is included widely. Under
`AXGL_DEBUG && TRACY_ENABLE` (i.e. the debug preset) it:

- Defines `AXGL_PROFILE_SCOPE`, `AXGL_PLOT`, `AXGL_ALLOC`, `AXGL_FREE` as real
  Tracy macros; otherwise they expand to nothing.
- **Globally overrides `operator new` / `delete` / array forms** to route
  allocations through `TracyAlloc`/`TracyFree`. These are non-inline
  definitions in a header guarded by the combo flag — keep an eye on ODR
  issues if you include `common.hpp` from many TUs in a debug build.
- Sets `SPDLOG_ACTIVE_LEVEL` to DEBUG and defines `AXGL_LOG_*` macros
  (`AXGL_LOG_DEBUG/INFO/WARN/ERROR`) backed by spdlog. Log level is
  configurable via `-DAXGL_LOG_LEVEL=<0..4>` (0=DEBUG default in debug, 2=WARN
  default otherwise).

When adding code, prefer the `AXGL_LOG_*` / `AXGL_PROFILE_SCOPE` macros over
raw spdlog/Tracy calls so release builds stay zero-cost.

## Conventions that differ from defaults

- Pointer type alias `axgl::ptr_t<T>` = `std::shared_ptr<T>`; use
  `axgl::create_ptr<T>(...)` and `axgl::ptr_cast<T1>(p)` (dynamic) instead of
  raw `make_shared` / `dynamic_pointer_cast`.
- No comments in code unless explicitly requested; existing files are largely
  comment-free by convention.
- `.clangd` removes all `-W*` flags from the compile database and re-adds
  `-Wall -Wextra` — don't rely on per-target warning flags showing up in
  clangd diagnostics.

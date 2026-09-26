# re_tests

A collection of small C++ programs used as test material for evaluating
different strategies for using AI agents to reverse engineer (RE) compiled
binaries. Each program is compiled as a stripped, optimized release binary
(no debug info) with the same VS8 (Visual Studio 2005 / VC80) toolchain
used to build Warcraft III — the eventual real-world target this test
material is meant to prepare for.

## Goal

The point is not the programs themselves but the *binaries* they produce:
progressively harder ground truth for testing how well an AI agent can
recover structure, logic, and naming from a stripped binary when the
matching source is known. Each project folder is self-contained, has known
source, and increases in complexity so RE strategies can be benchmarked
against a difficulty ladder before being pointed at something like War3.

## Repository layout

```
re_tests/
├── CMakeLists.txt            # root build script: builds every project below
├── paths.cmake.example       # template for local machine paths (copy -> paths.cmake)
├── paths.cmake                # <- you create this, gitignored, your local VS8 path
├── cmake/
│   └── VS8Toolchain.cmake    # CMake toolchain file that drives an out-of-tree VS8 install
├── 01_HelloWorldExample/
├── 02_MoreAdvancedExample/
├── 03_ClassesAndVirtualsExample/
├── 04_MiniRtsExample/
└── ...                        # more 0N_* projects added over time
```

Each `0N_*` folder is an independent CMake project with its own
`CMakeLists.txt` and `src/`, but all of them are built together from the
root with a single command via `add_subdirectory`.

### Test projects

| # | Project | What it exercises |
|---|---------|--------------------|
| 01 | `01_HelloWorldExample` | Trivial baseline: straight-line code, one branch, a libc call. |
| 02 | `02_MoreAdvancedExample` | No classes/inheritance. Plain structs (POD), a hand-rolled linked list, recursion (factorial/fibonacci), and a recursive-descent arithmetic expression parser. Tests control flow, recursion, and struct layout recovery without any OOP/vtable noise. |
| 03 | `03_ClassesAndVirtualsExample` | Classes, a 3-level inheritance chain (`Shape` → `Rectangle` → `Square`), multiple inheritance (`Shape` + `Printable`, i.e. two vtable pointers per object), virtual dispatch, and raw function pointers used as comparator/callback parameters. Tests vtable/RTTI-less polymorphism recovery and indirect call resolution. |
| 04 | `04_MiniRtsExample` | A small Warcraft III-style RTS (~11k lines, estimated 330–390 functions in IDA) with no strings, no RTTI and no STL: 2–4 AI players play a deterministic match on a generated map. Covers a pool allocator, A* pathfinding, fog of war, a unit/hero/building class hierarchy, 16 ability and 7 buff classes behind vtables, lockstep command packets, an LZ-compressed replay, a trigger bytecode VM with a natives table, creeps and a computer player. Built as the realistic benchmark for bottom-up (callees-first) summarization. See [`04_MiniRtsExample/README.md`](04_MiniRtsExample/README.md). |

More projects can be added the same way — create `0N_Name/` with its own
`CMakeLists.txt` and `src/`, then add one line to the root
`CMakeLists.txt`.

## Prerequisites

- CMake >= 3.10
- Visual Studio 8 (2005) / VC80, as a self-contained install tree (does
  **not** need to be registered with the system — see below). This is
  intentionally the same ancient toolchain Warcraft III was built with.
- Windows, to actually run `cl.exe`/`link.exe`.

## Configuring your local paths

VS8 lives outside this repo, and possibly outside any "normal" install
location, so its path is not hardcoded anywhere. Instead:

1. Copy `paths.cmake.example` to `paths.cmake` (repo root).
2. Edit `paths.cmake` and set `VS8_ROOT` to the root of your VS8 install
   tree (the folder containing `VC/` and `Common7/`).
3. Optionally set `WINSDK_ROOT`, `EXTRA_INCLUDE_DIRS`, `EXTRA_LIB_DIRS` if
   a project ever needs headers/libs beyond VC80's own `include`/`lib`.

`paths.cmake` is gitignored — it's machine-local and never committed.

## Building everything with one command

From the repo root, on Windows, with `paths.cmake` set up:

```bat
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/VS8Toolchain.cmake -DCMAKE_BUILD_TYPE=Release
build.bat
```

This configures and builds **all** `0N_*` projects in one pass; each
project's own `CMakeLists.txt` only has to declare its executable and
sources, the root script handles the toolchain, flags, and wiring them all
together.

`NMake Makefiles` is used instead of a `Visual Studio 8 2005` generator
because that generator expects VS8 to be registered with the system;
`cmake/VS8Toolchain.cmake` instead points CMake straight at
`cl.exe`/`link.exe` inside your `VS8_ROOT`, so an arbitrary, unregistered
install path works.

Use `build.bat` (repo root) rather than a bare `cmake --build build` for
the build step: `cl.exe`/`link.exe`/`lib.exe` need `mspdb80.dll` (from
`Common7/IDE`) on `PATH` plus the headers/libs on `INCLUDE`/`LIB`, and
those only get set for the *configure*-time process — a plain
`cmake --build` starts a fresh shell process that never sees them.
`build.bat` loads the env CMake generated at configure time
(`build\vs8-env.cmd`) into its own shell session and then runs
`cmake --build` from inside that same session, so the whole
nmake → cl/link/lib process tree inherits it. It works whether you run it
from `cmd.exe` or from a Git Bash/MSYS shell (`./build.bat`). Pass a build
dir as the first arg if you used something other than `build`
(`build.bat mybuild`).

To build just one project, load the env and target it in the same shell
session (from `cmd.exe`):

```bat
call build\vs8-env.cmd
cmake --build build --target 03_ClassesAndVirtualsExample
```

## Build output characteristics

All projects are always built as release binaries, regardless of
`CMAKE_BUILD_TYPE`:

- Optimized: `/O2 /GL` (compiler) + `/LTCG` (linker) on MSVC.
- No debug info: any `/Zi`, `/ZI`, `/Z7` are stripped from the flags, and
  the linker is never passed `/DEBUG`, so no `.pdb` is generated.
- `/OPT:REF /OPT:ICF` to drop unreferenced code/data and fold identical
  functions, producing lean, more realistic-looking binaries.
- `NDEBUG` defined, so any `assert()` calls compile out.

This is enforced centrally in the root `CMakeLists.txt`, so individual
project `CMakeLists.txt` files stay minimal (just their sources). The one
per-project exception so far: `04_MiniRtsExample` adds `/GR-` (no RTTI),
because RTTI type descriptors would embed every class name as a string.

A non-MSVC fallback path (`-O2 -s`, symbol stripping) is also included in
the root script purely for convenience, so a project can get a quick sanity
build/compile-check on Linux/mingw while iterating on source, even though
the actual RE test binaries are the Windows/VS8 ones.

## Troubleshooting

**`LINK : fatal error LNK1104: cannot open file 'kernel32.lib'`**

VC80's own `include`/`lib` folders only contain the C/C++ runtime and
standard library — they do **not** include the Win32 API
(`kernel32.lib`, `windows.h`, etc.). That comes from a separate Windows
Platform SDK, which a normal VS2005 install registers system-wide, but a
standalone/copied VS8 folder usually doesn't have. Fix:

1. Check whether your VS8 folder already bundles one, e.g.
   `<VS8_ROOT>/VC/PlatformSDK/Lib` — the toolchain file auto-detects and
   uses this if present, so if it's there and you still hit this error,
   double check `kernel32.lib` actually exists under that `Lib` folder.
2. If not, set `WINSDK_ROOT` in your `paths.cmake` to any Platform SDK /
   Windows SDK install that has `Include/` and `Lib/` with `kernel32.lib`
   (e.g. a copy of the old Windows Server 2003 R2 Platform SDK, which is
   the SDK version War3-era tooling typically pairs with VS2005).
3. Re-run the `cmake -S . -B build ...` configure step (delete the `build/`
   dir first if it already has a cached failed compiler check).

**`cmake --build build` fails with `no such file or directory` /
`Generator: execution of make failed. Make command was: nmake -f Makefile /nologo`**

The `NMake Makefiles` generator normally finds `nmake.exe` by detecting a
*registered* Visual Studio install; since your VS8 isn't registered, that
detection can come up empty and `CMAKE_MAKE_PROGRAM` is left unset/bare.
`cmake/VS8Toolchain.cmake` sets `CMAKE_MAKE_PROGRAM` explicitly to
`<VS8_ROOT>/VC/bin/nmake.exe`. If you still hit this, delete `build/` and
re-run the configure step (a stale cache can keep the old bad path).

**Build fails with `NMAKE : fatal error U1077: '"...\cmake.exe"' : return code '0xc0000135'`**

`0xc0000135` is `STATUS_DLL_NOT_FOUND` — `cl.exe` (or `link.exe`/`lib.exe`)
couldn't find `mspdb80.dll`, which lives in `<VS8_ROOT>/Common7/IDE`. This
happens if you build with a bare `cmake --build build` instead of
`build.bat`: the `PATH`/`INCLUDE`/`LIB` env vars needed for the whole
`nmake → cl/link/lib` process tree only get set inside the *configure*-time
CMake process, which exits once configuring is done — a fresh
`cmake --build` from your shell never inherits them. Use `build.bat`
(repo root) instead, which loads `build\vs8-env.cmd` (generated at
configure time) into its own shell session before building. If you're
building a single target manually, `call build\vs8-env.cmd` first in the
same `cmd.exe` session before your `cmake --build --target ...` call.

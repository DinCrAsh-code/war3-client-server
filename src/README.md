# `src/` — the reconstruction

This is one growing reconstruction of the binary, not one folder per target.
Targets from the same image share types: the software float in `Math/CFloat.h`
and the handle table in `Game/game.h` came out of the first call tree and the
next one will almost certainly want them. **Look here before inventing a
type.**

## Layout

One directory per subsystem. A file's directory says what part of the game it
belongs to and nothing else — it is not a namespace, not a link unit, and not
a scope: every symbol still lives in the one flat namespace the shipped binary
has.

| directory | .cpp | headers | what is in it |
|---|---:|---:|---|
| `Pathfinding/` | 137 | 10 | both searches (the region-portal wavefront and the per-cell A\*), the movement client, the spatial grid, footprints, `CPathTrace` |
| `Widget/` | 122 | 18 | `CWidget` and `CSelectable`, sprites, attachments, selection, model tokens |
| `Agent/` | 114 | 22 | `CAgent`, the agile type system, observers, `TRefCnt`, handles, `CJassFunc`'s own class declaration (`jassfunc.h` - the JASS native/user function wrapper, RTTI-verified chain down to `TRefCnt`; its own 30-slot vtable stays out of scope, see `Jass/` below) |
| `Net/` | 182 | 15 | W3GS, the game-action queue, `CDataStore`/`CDataStoreScratch`, the net commands, `OsNet`'s TCP, and `CNetData::CNetSession`'s game state |
| `Frame/` | 115 | 7 | Storm's frame library, reconstructed from both directions at once and merged: `CLayer` and its `LAYERNODE` parent/child tree (which is also the event frame - 0x6F600720 is `CLayer::CLayer`), `CLayoutFrame`'s nine anchors and its dependents, `CFrame`, `CScreenFrame`, `CSpriteFrame`, `CSimpleFrame` and the module-global `CSimpleTop` it registers with (its four per-layer `CFramePriorityArray` draw lists and its dirty grid), the three drawables `CSimpleRegion`, `CSimpleFontString` and `CSimpleTexture`, buttons, grids, message frames, frame points and the event-handler registry. Reached from `CGameUI` in both directions: every panel the pause native walks and every frame the camera's cursor owns is one of these |
| `Jass/` | 101 | 4 | the JASS natives, their registration, and the handle-resolution layer under them; `CJassFunc`'s own `InstanceGenerator<CJassFunc>` factory (`jassfunc.cpp`/`jassfuncinstancegenerator.h`/`jassfuncgeneratorsingleton.cpp` - the class declaration itself lives in `Agent/jassfunc.h`) |
| `GameUI/` | 143 | 15 | `CGameUI` and `CCameraWar3`.  Two JASS natives met here - what pausing the game walks (the cursor, the five controllers, the unit tip, the selection overlay, the skin store) and what panning the camera walks (its properties, its controller binding, the timed pan, the cursor and animation tokenizers, the two fade timers) |
| `Unit/` | 87 | 13 | `CUnit` — its vtable, fields, movement and placement |
| `Player/` | 12 | 2 | the player record's four *other* views - the published score fields the end-game score screen reads, the tally list of everything the player ever produced or lost, and the hero cluster that ranks the heroes on it (`HeroEval`, `BestHeroData`).  The same object `Jass/` sees as `SJassPlayer` and `Widget/` as `SPlayerSlot` |
| `Item/` | 52 | 16 | `CItem`, abilities, inventory |
| `Misc/` | 61 | 7 | the cross-cutting sweeps: trivial accessors, field dispatchers, thunks with no home |
| `Math/` | 40 | 11 | `CFloat` and the software float library, matrices, vectors, hashes, colour |
| `Render/` | 97 | 10 | fonts and text blocks, the file cache and the font cache, `.\Texture.cpp`'s two caches and its `.tga`/`.blp`/solid-colour classes, HMATERIAL and the model-side texture walks, the render device singleton, and the picking chain the cursor uses |
| `Sync/` | 30 | 7 | `CTimeSync`/`CSyncChannel`, the event scheduler, threads and locks |
| `Config/` | 25 | 5 | the named configuration store and the map's overrides |
| `Containers/` | 34 | 11 | Storm's `TSHashTable`, `TSGrowableArray`, `TSExplicitList` and friends |
| `Storm/` | 17 | 6 | Storm singletons, attachments, the tokenizer |
| `Missile/` | 11 | 3 | missile dispatch and the thunderbolt path |
| `Game/` | 37 | 10 | the game-data singleton, bounds, mode, version gates, `CObjectDataTable` - the `Units\\UpgradeData` sheet the upgrade probes and the race-name table read - and `CSaveGame`: the `.w3z` header read, its three slot-table layouts and the campaign profile list the game-cache store keys on |
| `Os/` | 32 | 3 | Storm's OS abstraction as it is statically linked into `Game.dll`: the path helpers and file/directory wrappers at 0x6F6C5xxx/0x6F6D7xxx, the CRC-32, and the whole of `.\AsyncFile.cpp` - the per-file operation pool, its eight id-keyed buckets and the process-wide registry the three game-state natives open the map archive through |
| `Replay/` | 2 | 0 | the replay layer's two diagnostic state-name tables, reached from the game-data stream |
| `Input/` | 5 | 1 | the game's own input-event pump: the drain loop and 15-way type dispatcher on top of the raw Win32 ring in `Window/`, its type-9 (mouse-down) handler and the shared cursor-clamp helper, and the naked redirects into Storm's own generic message-subscriber-list broadcast everything ultimately posts through |

**`build/` stays flat.** Every basename in `src/` is unique, and the whole
pipeline addresses a translation unit by that basename alone —
`build/<basename>.asm`, `tools/quick.py <basename>`, the injection repo's
compile cache. Grouping the sources did not change any of that, and
`tools/hookset.py`'s `source_categories()` is the one place that turns a
listing back into a directory.

**No source file changed when the tree was grouped.** `tools/build.sh` puts
every category directory on the include path, so a quoted `#include "game.h"`
from `Pathfinding/` resolves exactly as it did when `src/` was flat. The
injection repo's `build_mix.py` does the same, for its own compile *and* for
the include closure its staleness cache is built from — if those two ever
disagree, a header edited in another category stops invalidating its
dependents and the build links fresh objects against a stale ABI.

## Where a new file goes

By what the code *is*, not by which dump it arrived in. A leaf shared by two
subsystems goes with the one that owns the type it operates on — the software
float is `Math/`, not `Pathfinding/`, even though the path code is what needed
it first. When nothing fits, `Misc/` is a real answer and not a failure: it is
where the trivial-accessor sweeps and the homeless thunks live.

`tools/hookset.py` reads the directory names straight off the filesystem, so
adding a category is `mkdir` plus moving files — nothing registers it.

## House rules

These have not changed; see CLAUDE.md for the full set.

- **One translation unit per original module.** Functions in a dump call each
  other for real, so they must not be inlined into each other. A callee gets
  its own TU rather than a `__declspec(noinline)`.
- **Struct offsets are exact, and only touched members get names.** Gaps are
  `char m_reservedNN[0x54];`.
- **Every reconstructed function carries its address** in a comment above it,
  and its entry in `tools/funcmap.py`.
- **Say why when the source looks odd.** A `goto`, a redundant-looking copy, a
  strange comparison order — if the shipped code demands it, the comment says
  so, otherwise the next reader will "fix" it and lose 20 instructions.

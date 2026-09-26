# 04_MiniRtsExample

A small Warcraft III-style real-time strategy game, written to be reverse
engineered as a stripped release binary. It has no graphics and no input:
2–4 computer players play a full match on a generated map, and the process
exit code is a digest of the final game state. The logic is meant to be
realistic and tangled the way a real game binary is. Deep call chains,
virtual dispatch, callbacks, and data-driven behaviour give a bottom-up
"summarize the callees first" strategy real work to do.

## No hints in the binary

The binary is built so the only things to go on are code and numbers:

- **No string literals** anywhere in the game code. There's no logging,
  `printf`, asserts or file names. The only strings in the binary are the
  CRT's own, and FLIRT/library signatures already cover those.
- **No RTTI** (`/GR-`, set in this project's `CMakeLists.txt`). With RTTI on,
  every polymorphic class gets a type descriptor containing its decorated
  name (`.?AVCUnit@@`).
- **No STL, no iostream.** They would drag in strings like
  `"vector<T> too long"`. Containers and the allocator are written by hand,
  like the Storm library that War3 uses.
- **Opaque ids.** Unit, ability, item and order ids are plain small
  integers, not FourCC codes like `'hfoo'` or War3's real order ids.
- **Compiled map script.** The trigger script is embedded as compiled
  bytecode. Function and native names exist only in `tools/`.

## What's in it

About 11,000 lines of C++03 (VC80-compatible) in 32 translation units:

| Area | Files | Notable RE content |
|------|-------|--------------------|
| Core / engine | `Base.h`, `Memory.cpp`, `Random.cpp`, `MathUtil.cpp`, `Bits.cpp`, `Compress.cpp`, `Timer.cpp`, `Events.cpp` | Size-class pool allocator with class-specific `operator new`, generational handle table, combined LCG/xorshift RNG, sine lookup table, polynomial `atan2`, CRC-32 (table built at runtime), FNV/murmur-style hashing, LSB-first bit writer/reader with varints/zigzag/quantized floats, LZSS compressor with hash chains, timer min-heap, and an event bus of function-pointer listeners. Also the `TArray<T>` template (several instantiations). |
| World | `Terrain.cpp`, `Pathing.cpp`, `SpatialGrid.cpp`, `Fog.cpp` | Value-noise terrain generation (cliffs, ramps, water, forests, expansions and creep camp sites), grid A* with an indexed binary heap and search stamps plus line-of-sight path smoothing, a bucket grid with intrusive lists and filter/visit callbacks, and ray-cast fog of war blocked by cliffs and trees. |
| Objects | `Objects.h`, `Widget.cpp`, `Unit.cpp`, `Movement.cpp`, `Combat.cpp`, `Orders.cpp`, `Hero.cpp`, `Building.cpp`, `Missile.cpp` | Class hierarchy `CObject → CWidget → CUnit → CHero / CBuilding → CGoldMine`, plus trees, items and 3 missile types. Stat recalculation, steering and separation, attack state machine (damage point, backswing, cooldown), War3 damage/armor tables, splash bands, XP/bounty distribution, order queue and "smart" right-click resolution, hero attributes/levels/skills/inventory, train/research/revive queues, construction. |
| Abilities / buffs | `Abilities.h`, `Ability.cpp`, `AbilityWorker.cpp`, `Buff.cpp` | 16 ability classes and 7 buff classes behind vtables (13 and 6 virtual slots), created by factories from data tables: harvest and build state machines, stun missile, nova, bash, heal, auras, channeled blizzard, summons, crit/evasion, chain bounce, line wave, autocast buffs, poison. |
| Game | `Game.cpp`, `Player.cpp`, `Commands.cpp`, `Replay.cpp`, `Script.cpp`, `MapScript.cpp`, `Creeps.cpp`, `AI.cpp`, `Data.cpp` | Main tick loop, food/upkeep/tech requirements, lockstep command packets (bit-packed, delta-coded handles, executed 2 turns later), replay recorder (LZ blocks with CRC, verified at the end), a stack-based trigger bytecode VM with 30 natives in a function-pointer table, creep camps (sleep at night, aggro, leash, item drops), and a computer player (build list, worker balancing, build placement, expansion, creeping, attack/retreat, hero spell heuristics, micro). |

The unit, ability, buff, item, upgrade, XP and damage tables live in
`Data.cpp` and drive most of the behaviour, so a lot of the meaning is only
recoverable by connecting code to table layouts.

## Size

VC8 isn't available where this was written, so this is an estimate. Builds
with GCC 13 using LTO:

| Build | Game functions |
|-------|----------------|
| `-O2 -flto` (inlines every single-call function) | ~335 |
| `-O2 -flto -fno-inline-functions-called-once` | ~375 |

GCC lists 3 destructor variants per class (113 of those entries) and emits
42 `.cold`/`.isra`/`.part` clones. VC8 emits 1–2 functions per class and no
such clones, so expect roughly **280–330 game functions**. Add ~40–60 CRT
functions (`/MD`), for a total of about **330–390 functions in IDA**.

## Suggested roots for the pipeline

- `main`: the whole program. Almost every function is reachable from it,
  directly or through vtables and function-pointer tables.
- The per-tick update (`CGame::Tick`): the simulation without startup.
- The AI think function (`CAIController::Think`): mostly leaf-heavy
  decision code over game state.
- The script interpreter (`CScriptVM::Execute`): an opcode `switch` jump
  table plus an indirect call through the natives table. It also recurses
  through `OP_CALL`.
- The per-unit update (`CUnit::Update`), then orders (`CUnit::ProcessOrder`)
  and the attack state machine: virtual calls into abilities and buffs.

Finding these by address in a stripped binary is part of the exercise. The
exit code, and the string-free `.rdata` tables referenced from `CGame::Init`,
are good anchors.

## Running

```
04_MiniRtsExample.exe [any args]
```

The command line only seeds the game: the arguments are hashed into the
seed, which also picks 2–4 players. The same arguments always produce the same match and
the same exit code. A full 30-minute (game time) match simulates in a couple
of seconds.

## Regenerating the map script

`src/MapScript.cpp` is generated. Edit `tools/map_script.asm`, then run:

```
python tools/script_asm.py tools/map_script.asm src/MapScript.cpp
```

The assembler's opcode values and native order must match `src/Script.cpp`.

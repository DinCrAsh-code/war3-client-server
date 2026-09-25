# Каталог исходников

Сгенерировано из Git, без исполнения или компиляции исходников.
Ревизия: `2fc76c8035554912dd66fc0a06a39eda376a806c`.
Файлов: **3465**; каталогов подсистем: **25**.
Форматы: `.cpp` — 3086, `.h` — 369, `.inl` — 9, `.md` — 1.

[SOURCE_FILES.tsv](SOURCE_FILES.tsv) перечисляет каждый путь и Git blob.
Классификация `primary_area`/`topics` наследуется от каталога и служит
для навигации. Это не индивидуальная проверка файлов, число функций
или оценка готовности. `cross-cutting` — служебная метка инвентаря;
для FND по-прежнему выбирается один из subsystem в правилах реверса.
Приоритеты конкретных цепочек: [карта src](../SRC_MAP.md).

| Каталог | Файлов | Основная область | Пересечения |
|---|---:|---|---|
| [(root)](../../../src) | 1 | documentation | source-navigation |
| [Agent](../../../src/Agent) | 559 | simulation | handles,lifetime,relations,jass |
| [Compress](../../../src/Compress) | 6 | resources | compression |
| [Config](../../../src/Config) | 43 | resources | configuration,map-overrides |
| [Containers](../../../src/Containers) | 71 | resources | shared-containers |
| [Frame](../../../src/Frame) | 166 | presentation | ui,events,layout |
| [Game](../../../src/Game) | 93 | startup | context,resources,savegame |
| [GameUI](../../../src/GameUI) | 237 | presentation | camera,selection,local-player |
| [Input](../../../src/Input) | 7 | presentation | input,dispatch,cursor |
| [Item](../../../src/Item) | 155 | simulation | items,abilities,inventory |
| [Jass](../../../src/Jass) | 258 | jass | vm,natives,handles,local-context |
| [Math](../../../src/Math) | 75 | simulation | softfloat,geometry,shared-math |
| [Misc](../../../src/Misc) | 215 | cross-cutting | accessors,thunks,unclassified |
| [Missile](../../../src/Missile) | 58 | simulation | projectiles,effects |
| [Net](../../../src/Net) | 415 | network | lockstep,commands,session,serialization |
| [Os](../../../src/Os) | 46 | resources | platform,files,async-io |
| [Pathfinding](../../../src/Pathfinding) | 241 | simulation | movement,pathing,collision |
| [Player](../../../src/Player) | 55 | simulation | player-records,scores |
| [Render](../../../src/Render) | 138 | presentation | graphics,resources,picking |
| [Replay](../../../src/Replay) | 2 | network | replay-diagnostics |
| [Storm](../../../src/Storm) | 33 | resources | shared-runtime,attachments |
| [Sync](../../../src/Sync) | 55 | simulation | time,checksum,rng,threads |
| [Task](../../../src/Task) | 72 | simulation | tasks,scheduling |
| [Unit](../../../src/Unit) | 261 | simulation | units,visibility,selection,movement |
| [Widget](../../../src/Widget) | 189 | simulation | world-objects,visibility,fog,presentation |
| [Window](../../../src/Window) | 14 | presentation | platform,input,window |

## Обновление

Из корня репозитория, Python 3 и Git:

```text
python research/reverse/tools/catalog_src.py --ref 2fc76c8035554912dd66fc0a06a39eda376a806c
python research/reverse/tools/catalog_src.py --ref 2fc76c8035554912dd66fc0a06a39eda376a806c --check
```

При новом импорте указать его commit; затем отдельно пересмотреть
SRC_MAP, SOURCES и затронутые FND. Генератор не повышает evidence
и не исправляет устаревшие комментарии в исходниках.

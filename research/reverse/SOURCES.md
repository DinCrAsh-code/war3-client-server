# Источники знаний о движке

Срез проверки: **2026-09-25**. Источник начальных FND-0001–0006 —
[war3-engine-atlas](https://github.com/DinCrAsh-code/war3-engine-atlas),
ветка `agent/sim-research`, commit
`7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8`.
Изучена локальная копия; commit совпал с удалённой веткой на момент проверки.
Источник закрытый: для перехода по ссылкам нужен отдельный доступ.
Это не утверждение о содержимом его `main` или последующих ревизий.

Из Атласа перенесено краткое описание контрактов движка и их ограничений.
Код, история, сырые отчёты и продуктовые компоненты других приложений не
копировались. Исходные динамические эксперименты здесь **не повторялись**:
все начальные карточки имеют `verification: source-reviewed`.

## S01–S09: первичные документы Атласа

| ID | Источник на закреплённой ревизии | Использование |
|---|---|---|
| S01 | [Карта подсистем](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/ENGINE_MAP.md) | Навигация, связные и открытые маршруты |
| S02 | [Текущий исследовательский индекс](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/notes/RESEARCH_INDEX.md) | Актуальная граница cases 049–052 |
| S03 | [Граница headless и display](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/notes/PASS_028_HEADLESS_BOUNDARY.md) | FND-0001 |
| S04 | [Юниты и команды](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/UNIT_COMMANDS.md) | FND-0002 |
| S05 | [Ввод сессии и фазы мира](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/WORLD_PHASES.md) | FND-0003 |
| S06 | [Начальные данные и RNG](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/STARTUP_STATE.md) | FND-0004 |
| S07 | [Привязки экземпляра JASS](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/JASS_INSTANCE_BINDINGS.md) | FND-0005 |
| S08 | [Провайдеры подготовки тумана](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/FOG_PROVIDER_QUERIES.md) | FND-0006 |
| S09 | [Активация мира](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/WORLD_ACTIVATION.md) | FND-0001, FND-0005; ограниченный компонентный маршрут |

Ссылки на компактные evidence и их хеши находятся в соответствующих
исходных контрактах. Числа в карточках — результаты источника, не локальные
прогоны этой задачи. Build scope: Warcraft III 1.26a, Game.dll 1.26.0.6401;
точный fingerprint источника S01–S09 (не переносится автоматически на S10):
`6d21bd9a0f9fbc8446f455c9e89ac994fed68174426fe608fcb9baefd4dec53c`.

## S10

Публичная [реконструкция в src](https://github.com/DinCrAsh-code/war3-client-server/tree/2fc76c8035554912dd66fc0a06a39eda376a806c/src),
commit `2fc76c8035554912dd66fc0a06a39eda376a806c`,
название коммита автора: `7k matching c++ funcs`.
По пояснению владельца, коллега с агентом Claude восстанавливает весь движок,
а здесь опубликован пройденный участок. Оценка около 82 тысяч функций в
Game.dll также сообщена коллегой; независимый подсчёт не выполнялся.

Инвентарь Git: 3465 файлов, 25 каталогов подсистем. Полный перечень путей и
blob SHA: [каталог](catalog/README.md). Прикладная классификация и просмотренные
символы: [SRC_MAP](SRC_MAP.md); выводы: FND-0007–0009.
Ссылки на файлы в карточках относительные для навигации; выводы относятся
к закреплённому здесь commit, а не автоматически к будущему состоянию файлов.

Scope по комментариям исходников — build 6401, x86. Проверенного manifest
с fingerprint Game.dll для этой поставки нет. Совпадение версии/адресов
не связывает её с образом S01–S09 автоматически. Точные адреса из кода
не перенесены в новые карточки как проверенные точки интеграции.

Прочитаны тела выбранных функций и их зависимости. В дереве соседствуют
C++/asm-тела, декларации, регистрации и переходы в оригинальный образ.
Статусы matching из комментариев и названия коммита не пересчитывались.
Указанные в `src/README.md` инструменты сборки и сравнения не входят в этот
экспорт; это не утверждение об их отсутствии у коллеги. Для воспроизведения
нужны соответствующие toolchain/flags, function registry, отчёты и harness.
Счётчики в исходном README описывают более ранний срез; текущие берутся из Git.

## S11: адресный реестр и matching pipeline коллеги

Приватный [claudecraft](https://github.com/FilippTheBestDev/claudecraft),
commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`, проверен чтением
`README.md`, `pipeline/CLAUDE.md`, схемы `docs/agent-worktrees-schema.md`,
выбранных `pipeline/src/`, `pipeline/docs/targets/` и адресных записей
`agent_worktrees/funcs/`. Для перехода по ссылке нужен доступ к репозиторию.

В этой ревизии Git-дерево содержит **76 572 JSON-записи функций**,
3673 JSON-записи классов и 648 записей данных. Прежний подсчёт файлов
завышал первые два числа на `.gitkeep`. Это адресный реестр IDA, **не**
число восстановленных функций и не оценка готовности клиента-сервера.

Полный адресный корпус дизассемблированных функций находится в
[`agent_worktrees/funcs/`](https://github.com/FilippTheBestDev/claudecraft/tree/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs):
один JSON на IDA-адрес, поле `raw_asm` содержит строки инструкций, а
`raw_bytes` — байты функции. Формат и происхождение полей описаны в
[схеме коллеги](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/docs/agent-worktrees-schema.md).
Проверка всех JSON на закреплённом commit дала `raw_asm` у **76 572 из
76 572** записей (2 361 804 строки), непустой `raw_bytes` у **76 558**;
14 записей без байтов относятся к старым выгрузкам. Это проверка
заполненности полей, **не** сверка строк и байтов с локальной DLL или IDA.
Отдельные call-tree дампы лежат в
[`pipeline/asm/`](https://github.com/FilippTheBestDev/claudecraft/tree/10950d496aa7357a6180c1956def50c2a3e8c1a9/pipeline/asm)
(1534 файла, из них 265 в `processed/`); ссылка `dump_file` есть только
у 27 адресных JSON. Корпус массово выгружен скриптом
[`verifier/ida_scripts/dump_agent_worktrees.py`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/verifier/ida_scripts/dump_agent_worktrees.py)
из IDA; старые дампы перенесены
[`backfill_agent_worktrees.py`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/pipeline/tools/backfill_agent_worktrees.py).
В этой ревизии видны 76 572 записи, а заявленные ~82 тысячи — внешняя
оценка: полноту выгрузки относительно всей DLL мы не подтвердили.

Статусы всех `agent_worktrees/funcs/*.json` этой ревизии пересчитаны:
`git ls-tree -rz` дал blob ID файлов `.json`, `git cat-file --batch`
прочитал их JSON-поле `status`; записи не менялись.

| Статус S11 | Число записей |
|---|---:|
| `TODO` | 68 472 |
| `THUNK` | 1110 |
| `EXACT` | 3791 |
| `IDENTICAL` | 117 |
| `DIFFERS` | 2879 |
| `MATCH-PARTIAL` | 108 |
| `MISMATCH` | 95 |
| **Всего** | **76 572** |

Без `TODO` и `THUNK` остаётся 6990 записей. Близость к названию
«7k matching» — **возможное объяснение**, а не подтверждение метода
подсчёта коллеги: эти 6990 включают `DIFFERS`, `MATCH-PARTIAL` и
`MISMATCH`. `EXACT`/`IDENTICAL` вместе дают 3908 записей, но и это
не доказывает автономную сборку, запуск, совместимость карт или политику
раскрытия. Пересборка и собственный verifier здесь не выполнялись.
Адресные статусы относятся только к этой ревизии и могут измениться
при параллельной работе.

`agent_worktrees/funcs/<ADDR>.json` хранит IDA-disassembly/bytes, имя,
`TODO`/`EXACT`/`DIFFERS`/`THUNK`, score и claim. Схема также различает
вердикты `angr`; `IDENTICAL` нельзя присваивать агентным утверждением.
`pipeline/tools/quick.py` пересобирает одну единицу трансляции для итерации;
`verify.py` сравнивает инструкции и записывает score; ABI/vtable/link-аудиты
проверяют отдельные классы ошибок. `verifier/tools/verify_gate.py` собирает
инъекцию, исполняет контрольный сценарий и проверяет tracepoints перед
освобождением claims через `finish_target.py`. Эти проверки не являются
оракулом отсутствия утечек видимости; такой сценарий нужно задать отдельно.

В `pipeline/CLAUDE.md` идёт миграция источника истины с `funcmap.py`/`asm`
на `agent_worktrees/`: верхнее правило требует записывать новые статусы через
`worktree_store.py`, а часть старых пошаговых инструкций ещё упоминает
`funcmap.py`. При расхождении используем актуальную схему и код инструмента,
а не исторический текст. Сырые IDA-выгрузки, бинарники и приватный код в этот
репозиторий не копируются. Fingerprint исходной Game.dll из прочитанных
адресных записей не установлен; адреса здесь — координаты S11, не переносимый
runtime-контракт.

## Контрольный benchmark, не evidence Game.dll

В `main` этого репозитория, commit
`93a7ff2e409aefa89b26647ed9a35c26b9c5cfe3`, опубликованы исходники
`tmp/re_tests/04_MiniRtsExample` и результаты прогона
`tmp/benchmark01/` на синтетическом MiniRTS. Сообщение коммита указывает
357 вызовов модели для 241 достижимой от `main` функции; результаты
предназначены для ручного разбора. Наличие отчётов не доказывает точность
семантических имён и не измеряет восстановление Game.dll. В Git также
попали локальные build-артефакты benchmark; мы их не используем как
источник игровых контрактов.

## Пробелы и отбор

Каталог `docs/client-server/`, классификатор нативов и численные оценки
нагрузки, на которые ссылался исходный бриф, в изученной ревизии Атласа
не найдены. Они не используются как подтверждение архитектуры или скорости.
Сведения о реализации сторонних продуктов, интерфейсах анализа и расследованиях
конкретных читов исключены из этой базы: они не определяют контракт движка.

Перепроверка ограничивалась чтением документов/выбранных исходников,
инвентаризацией Git и сравнением ревизий.
Бинарники, карты, гостевые сценарии, полные отчёты и серверная нагрузка в этой
задаче не исполнялись и не измерялись. Следующие агенты начинают с конкретной
находки и её источника, а не принимают пересказ за независимое доказательство.

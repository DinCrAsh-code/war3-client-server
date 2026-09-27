# FND-0081 — turn store отделяет inline payload при передаче буфера

| Поле | Значение |
|---|---|
| Subsystem | network |
| Tags | turn-store, replay, buffer, ownership, allocation, compatibility |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; пять тел `0x6F2C9450/2C9530/4C1BE0/543DF0/5484B0`. Исследованы поля и ветви хранения буфера, но не полное владение через все вызовы, аллокатор в игре и replay. Fingerprint DLL неизвестен. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0080](FND-0080-replay-payload-chunk-route.md); [C++ PR #53](https://github.com/FilippTheBestDev/claudecraft/pull/53) как вторичная реконструкция |

## Граница буфера

В этих телах общий вид store содержит указатель буфера в `+0x04`, поле
в `+0x08`, ёмкость в `+0x0C`, размер в `+0x10`, read position в `+0x14`
и 1460 байт inline storage с `+0x18`. Наличие этого вида у нескольких
классов не означает, что у них полностью совпадают vtable и lifetime.

[`0x6F2C9450`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9450.json)
вычисляет требуемую ёмкость как `offset + increment` и возвращает успех
без выделения памяти, если она не превышает текущую ёмкость. Для
внешнего буфера вызывает Storm realloc. Для inline буфера выделяет
внешнюю память и копирует `min(offset, прежняя ёмкость)` байтов, затем
записывает новую ёмкость. Сумма 32-битная; поведение при переполнении
не проверено. Источник и строка выделения могут прийти от вызывающего
кода; при null источнике функция выбирает собственные S11 allocation tags.

[`0x6F4C1BE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4C1BE0.json)
записывает по ненулевым выходным указателям буфер, размер и ёмкость,
обнуляет `+0x04/+0x0C` и вызывает виртуальный slot `+0x1C` для reset.
[`0x6F2C9530`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9530.json)
сначала выполняет detach, затем, если полученный указатель совпадал с
inline storage и размер ненулевой, выделяет отдельный буфер, копирует
ровно `size` байтов и, если выходная ёмкость запрошена, меняет её на
`size`. Возвращать
указатель внутрь очищенного store в этой ветви было бы ошибкой.

[`0x6F543DF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F543DF0.json)
обрабатывает sentinel `capacity == -1` обнулением буфера и ёмкости.
При положительном `+0x08` вызывает виртуальный Grow с нулевыми offset и
increment, затем ставит `size = 0`, `readPos = -1` и передаёт управление
`0x6F652040`. [`0x6F5484B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5484B0.json)
вызывает уже существующий деструктор `0x6F2C95B0`; при бите `flags & 1`
освобождает объект через Storm.

## Контроли и пределы

Положительный статический контроль: inline буфер с ненулевым размером
после `GetBuffer` копируется в отдельную память, а результат имеет
ёмкость, равную размеру. Отрицательные ветви: `Grow` при достаточной
ёмкости не выделяет память; `GetBuffer` не копирует внешний буфер и
не копирует inline буфер нулевой длины; деструктор при сброшенном
`flags & 1` не вызывает Storm free.

Сверка подготовленных C++ тел с ASM ограничена native VC8 сборкой и
локальным сопоставлением инструкций; даже полное совпадение отдельных
малых тел не заменяет штатный verify. Не установлены результат при
неудаче выделения, исключения, runtime ownership всех вызывающих путей,
точный fingerprint образа и совместимость replay в игре. Этот буферный
контракт не даёт права выдавать скрытое состояние клиенту.

# FND-0039 — событие `0xD01D4` условно снимает юнит с локального выбора

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, observer, SubmitUnit, selection, GameUI, presentation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен; адреса — координаты S11. S10 использован как вторичная реконструкция. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [CUnit message dispatcher `0x6F2A7E60`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A7E60.json), [handler `0x6F284950`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F284950.json), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [`PublishPosition` `0x6F285110`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F285110.json), [selection membership `0x6F4219B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4219B0.json), [selection remove `0x6F424CE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F424CE0.json), [registration in initializer `0x6F2A0E30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A0E30.json), [tracked listener install `0x6F28D7D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F28D7D0.json) |

## Вход и два исхода

У CUnit виртуальный dispatcher `+0x0C` (`0x6F2A7E60`) на сообщении
`0xD01D4` прямо вызывает `0x6F284950`. Это доказанный вход handler,
но не доказательство конкретного момента доставки сообщения. В
per-template initializer `0x6F2A0E30` этот ключ регистрируется через
CUnit vtable `+0x08` (`0x6F62A9A0`); `0x6F28D7D0` и обратный
`0x6F28D9C0` также устанавливают `FloatListener` с тем же ключом
на tracked ref юнита `+0x118` через `0x6F477550` при изменении
счётчика `unit+0x114`. Эти установочные пути объясняют происхождение
подписки, но не подменяют путь реального callback.

В `0x6F284950`, если `unit+0x114 == 0`, код записывает нулевой
`CFloat` в `unit+0x118` и переходит к `0x6F284830`; вызова
`PublishPosition` и selection removal в этой ветви нет. При
ненулевом `+0x114` handler ставит флаг `unit+0x5C |= 0x01000000`,
выполняет `0x6F2AB310`, публикует отдельное сообщение `0xD01DE`
через `0x6F26F970`, записывает значение в tracked ref `+0x118`
и вызывает собственный CUnit vtable `+0x100` с аргументами `(1, 4)`.
Динамический тип здесь определён: это `this` CUnit handler, а CUnit
vtable указывает на `0x6F285110`.

Если `PublishPosition(1,4)` возвращает **1**, handler переходит
сразу к `0x6F284830` и не входит в selection branch. Если
возвращает **0**, handler берёт запись локального слота через
`0x6F3A1650`, её поле `+0x34`, и вызывает
`0x6F421E20 → 0x6F4219B0` для проверки наличия именно этого
unit в списке `CSelectionWar3`. Затем `0x6F424CE0` ищет unit в
списке, уменьшает счётчик `+0x1F8`, отвязывает узел и при
соответствующем аргументе вызывает виртуальный `unit+0x194`.
Следуют GameUI helpers `0x6F333230` и, только если предыдущая
membership проверка была положительной, `0x6F332700`;
`0x6F284830` завершает оба исхода обновлением presentation state.
Подробный конечный экранный эффект этих helpers здесь не установлен.

## Почему это не fog- и не Net-пакет

В этом caller первый аргумент `PublishPosition` равен **1**. По
[FND-0010](FND-0010-unit-submit-gates.md) оба `SubmitUnit`
сначала, если бит 1 флагов не установлен, проверяют
relation/detection gate `0x6F3A15F0`; затем установленный бит 0
возвращает `1` **до** запроса fog grid. `0x6F285110` добавляет к
аргументу только ещё бит 0 из TLS/game mode, поэтому не создаёт
бит 1. Значит, в этой ветви успех раннего gate даёт `1` без
чтения fog grid, а его отказ даёт `0` без чтения fog grid;
последний ещё может быть перекрыт запасной проверкой unit bits
`+0x148/+0x14C` через `0x6F26F9E0`. Маска ответа `4` здесь
не означает, что fog code `4` реально был прочитан. Происхождение
двух разных масок мира для `PublishPosition` —
[FND-0017](FND-0017-local-unit-publication-masks.md).

Положительный статический контроль: сообщение `0xD01D4` входит
в `0x6F284950`; при `+0x114 != 0` и отрицательном ответе
`+0x100` достигается локальная selection branch, а при найденном
узле `0x6F424CE0` отвязывает его. Отрицательные контроли:
`+0x114 == 0` пропускает всю ветвь; положительный ответ
`+0x100` пропускает selection removal; пустой список или отсутствие
этого unit в `CSelectionWar3` не даёт `0x6F424CE0` отвязать узел.
Нулевой результат поиска GameUI пропускает внутренний UI callback
`0x6F333230`; выполненные ранее изменения CUnit остаются.

Отдельная S11 функция [`0x6F2CC350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CC350.json)
строит `CNetCommandUnitSelectionEvent` с type `0xA001B` из двух
полей своего source и передаёт его в `0x6F2CA010`. Просмотренный
`0x6F284950` не вызывает этот builder, его packet writer или
транспорт. Команда выбора — отдельный путь; её наличие не
доказывает сериализацию состояния юнита для конкретного клиента.
В показанной цепи доказана локальная проверка и mutation selection
state, не server→client выдача health/position/inventory.

Статусы S11: `0x6F284950`, `0x6F2A0E30` — `THUNK` в S10
реконструкции при наличии IDA-body; `0x6F285110` — `DIFFERS`,
остальные адресные связи прочитаны статически без исполнения.
Остаются открытыми точный runtime trigger `0xD01D4`, все side
effects helper'ов выбора и независимый outbound serializer state.
Следующий опыт: в двухигроковом матче записать `unit+0x114/+0x118`,
доставку `0xD01D4`, ответ `0x6F3A15F0`, return `+0x100`, локальную
selection list и отправленные bytes; сравнить с command type `0xA001B`
и контролем без права обзора.

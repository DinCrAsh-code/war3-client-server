# FND-0079 — sender key связывается с байтом игрока в локальной таблице

| Поле | Значение |
|---|---|
| Subsystem | network |
| Tags | sender-key, player-byte, session, lookup, admission, peer-boundary |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; setup gates `0x6F5C0830`/`0x6F5C0EE0`, запись `0x6F5496A0` → `0x6F5491D0`, чтение `0x6F54E7C0` → `0x6F54E7E0`, удаление `0x6F54FDF0` → `0x6F54E800`. Установлено локальное связывание key с player byte; источник доверия к key и привязка к authenticated peer не установлены. Fingerprint DLL неизвестен. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0078](FND-0078-local-event-turn-parser.md); [FND-0062](../simulation/FND-0062-inbound-selection-basic-order-application.md) |

## Жизненный цикл соответствия

Во время обработки session records два вызова
[`0x6F5C0830`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5C0830.json)
и
[`0x6F5C0EE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5C0EE0.json)
передают key byte в `cl`, player byte в `dl` и slot на стеке в
[`0x6F5496A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5496A0.json).
Оболочка получает player record из session data, добавляет
`slot*0x304+0x290` и вызывает
[`0x6F5491D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5491D0.json)
с key и player byte. Если byte в record `+0x2DC` совпал с key,
`0x6F5496A0` меняет его на player byte. Смысл `+0x2DC` за пределами
этой ветви не доказан.

`0x6F5491D0` берёт уже существующий entry из массива `table+0x40`
по `key & 0x7F`, очищает эту ячейку, при необходимости расширяет
массив `table+0x30`, кладёт туда тот же entry по индексу player byte
и пишет byte в `entry+0x36`. `playerId` перед этим усекается до байта;
при росте массива новые slots обнуляются. Независимая сверка этих полей
с [C++ PR #53, коммит `dca548a482`](https://github.com/FilippTheBestDev/claudecraft/pull/53)
включает helpers `0x6F538690/0x6F5386F0` для выбора allocation chunk
и переноса массива. Функция ожидает, что entry уже существует;
эта находка не описывает создание entry и его ключа.

## Условия перед регистрацией

`0x6F5C0830` собирает текущие слоты через `0x6F5C0780` и до цикла
регистрации сравнивает два счётчика с полями входного setup-объекта:
счётчик из локального списка с `input+0x11`, второй с `input+0x04`.
Несовпадение любого возвращает `0` до вызова `0x6F5496A0`.
В цикле 9-байтных записей вызов регистрации виден только при
`record[2] == 2`, `record[3] == 0` и результате локального lookup
`0x6F54F3F0`, отличном от `0xFF` (байт `0` допустим).

Второй путь, `0x6F5C0EE0`, получает список через тот же
`0x6F5C0780`; обе его ветви регистрации пропускают запись, когда
`record[3] != 0` или `record[0] == 0`. При прошедшем gate он снова
берёт player byte из `0x6F54F3F0` и передаёт его вместе с выбранным
слотом в `0x6F5496A0`. Общий сборщик `0x6F5C0780` сначала пробует
`0x6F53EFF0`: тот копирует 0xB8 байт из TLS-записи `index*0x304+0x13C`
только при состоянии `record+0x278 >= 1`, затем строит список через
`0x6F5BF090`. Это проверка согласованности локального setup, а не
свидетельство, что запись пришла от удостоверенного сетевого peer.

При разборе turn record из [FND-0078](FND-0078-local-event-turn-parser.md)
первый byte подзаписи идёт через
[`0x6F54E7E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54E7E0.json).
Её helper
[`0x6F54E7C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54E7C0.json)
ищет key через таблицу `slot*0x304+0x290` и
[`0x6F54C870`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54C870.json).
При найденном entry результатом становится `entry+0x36`, при
отсутствии — `0xFF`. Отдельная оболочка `0x6F54F3F0` тоже возвращает
`0xFF`, если session table ещё отсутствует.

На пути удаления
[`0x6F54FDF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54FDF0.json)
вызывает
[`0x6F54E800`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54E800.json)
на таблице того же slot. Последняя читает `entry+0x36`, по его
старшему биту очищает соответствующую ячейку `table+0x40` либо
`table+0x30`, затем удаляет entry из таблицы и уменьшает `table+0x48`.
Её собственный body не проверяет результат lookup на null; отсюда
нельзя выводить безопасное поведение для произвольного отсутствующего key.

## Контроли и границы

Положительный статический контроль: при существующем entry, key и
player byte `0x6F5491D0` записывает заданный byte в `+0x36`, а
`0x6F54E7E0` возвращает это поле после поиска key. Отрицательный
контроль: отсутствие key в lookup возвращает `0xFF`; отсутствие session
table в `0x6F54F3F0` также даёт `0xFF`. Несовпадение setup-счётчиков
в `0x6F5C0830` завершает обработку до регистрации. Здесь нет ветви, которая
сопоставляет key криптографически подтверждённому connection identity.

Отвергнута гипотеза, что байт sender, полученный turn parser, сам
удостоверяет сетевого игрока. Показанная цепь лишь переносит локальное
соответствие key → player byte, причём registration получает оба
значения от внешнего session record. Не изучены запись исходного entry,
происхождение session record, защитные проверки до его обработки,
поведение key `0xFF` внутри command dispatch и последствия удаления во
время активного turn. Для авторитетного приёмника нужен отдельный опыт,
который связывает конкретный connection с разрешённым player slot и
проверяет подменённый key/чужой unit identity на двух игроках.

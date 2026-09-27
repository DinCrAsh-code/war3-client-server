# FND-0022 — пересчёт масок объекта агрегирует ответы клетки по 12 слотам

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, grid, player-mask, item, presentation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [RebuildPlayerMasks `0x6F3A1460`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A1460.json), [caller `0x6F38EA30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F38EA30.json), [Prepare `0x6F26D0C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26D0C0.json), [Submit `0x6F00E7A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F00E7A0.json), [S10 C++](../../../../src/Widget/playertablerebuildmasks.cpp) |

## Вывод

`0x6F3A1460` обнуляет два выходных слова, получает пару кодов одной
клетки через `Prepare` и последовательно обходит **12** слотов. Для
слота `i` входное слово `players[i+1]` служит маской запроса к
`Submit`, а выходу присваивается бит **самого слота** `1 << i`:

| Условие для слота | Изменение выходов |
|---|---|
| `players[i+1] == 0` | Никакого; `Submit` не вызывается |
| Уже накопленный `outVisible` пересекается с `players[i+1]` | Добавить `1 << i` в `outVisible`, без `Submit` |
| `Submit(codeA, codeB, players[i+1])` имеет бит `4` | Добавить `1 << i` в `outVisible` |
| Иначе: поле host `+0x3C0` ненулевое или ответ имеет бит `2` | Добавить `1 << i` в `outFogged` |
| Иначе | Не добавлять бит никуда |

Это не пара независимых запросов «видим/не видим» для каждого игрока:
порядок обхода влияет на ранний путь, поскольку уже накопленный
`outVisible` проверяется против маски следующего слота. Выходы
содержат индексы слотов, а не копии входных `players[i+1]`.

Вызывающий `0x6F38EA30` находится в пути уведомления о смене маски
предмета и продолжает работу с renderer/UI helpers после пересчёта.
Это граница локального потребителя; из неё нельзя вывести полный
серверный список полей, разрешённых клиенту. Общая таблица кодов
`Submit` описана в [FND-0018](FND-0018-fog-grid-result-table.md).

## Контроли и пределы

Положительный статический контроль: оба output words обнуляются до
`Prepare`; цикл сдвигает однобитный `1 << i` ровно 12 раз; пересечение
с уже накопленным `outVisible` обходится без `Submit`; только ветви
`Submit & 4` и `+0x3C0`/`Submit & 2` заполняют соответствующий выход.
Отрицательные: нулевая входная маска слота пропускает весь шаг;
ответ `1` при нулевом `+0x3C0` не ставит ни видимый, ни fogged-бит.

Статус S11 у `0x6F3A1460` — `DIFFERS`, у его UI-caller — `THUNK`;
здесь проверены IDA-тела и опубликованный C++, не выполнена игра.
Не установлены все формирователи массива `players`, значение режима
`+0x3C0` в живом матче, конечное действие UI helpers и применение
этого результата к сетевой выдаче. Для динамического контроля нужны
два игрока с пересекающимися и непересекающимися входными масками,
одинаковой клеткой и переставленным порядком слотов; сравнить оба
выхода и фактический UI, не приравнивая его к серверному ACL.

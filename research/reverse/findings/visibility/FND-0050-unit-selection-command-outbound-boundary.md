# FND-0050 — сетевой выход выбора юнита передаёт команду, а не снимок его состояния

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, selection, network, command, disclosure-boundary |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен; адреса — координаты S11. S10 использован как вторичная реконструкция. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [вход `+0x1D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2979E0.json), [вход `+0x1D4`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F297B30.json), [builder `0x6F2CC350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CC350.json), [serializer entry `0x6F2CA010`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CA010.json), [payload writer `0x6F5542F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5542F0.json), [command queue `0x6F54D970`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54D970.json), [flush decision `0x6F54D930`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54D930.json), [queue flush `0x6F650480`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F650480.json), [client send `0x6F657340`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F657340.json) |

## Вход и состав команды

У CUnit vtable `+0x1D0` и `+0x1D4` ведут соответственно в
`0x6F2979E0` и `0x6F297B30`. В обеих функциях ветвь с нулевым
вторым stack-аргументом условно вызывает **один и тот же** builder
`0x6F2CC350`. Первая передаёт в `dl` значение `1`, если есть событие
`0x80239` у юнита или `0x80218` у разрешённого связанного объекта;
вторая передаёт `2` при `0x8023A` или `0x80219` соответственно.
Ненулевой второй аргумент ведёт через другие локальные side effects
и не входит в этот builder.

Builder создаёт `CNetCommandUnitSelectionEvent` с внутренним ID
`0xA001B` и однобайтовым индексом команды `0x1B`. В его объекте
`+0x18 = dl`, `+0x1C = source+0x0C`, `+0x20 = source+0x10`.
Source на названных входах — `this` CUnit. Вторичная S10
реконструкция базового CAgent именует `+0x0C/+0x10` как пару
handle/type tag ([заголовок](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Agent/agent.h)); само S11 тело доказывает чтение
**двух dword по этим смещениям**, не их переносимую семантику.

`0x6F2CA010` записывает в `CDataStore` байт `cmd+0x14` через
`0x6F4C2160`, затем вызывает `0x6F5542F0`: тот записывает байт
`cmd+0x18` и dword `cmd+0x1C`, `cmd+0x20` через
`0x6F4C2160/0x6F4C2360`. Поэтому содержимое именно команды —
`0x1B`, `1` или `2`, две исходные dword; ID `0xA001B` служит
идентификатором объекта/dispatch и этим writer не записывается.
Обратный builder [`0x6F540FD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F540FD0.json)
для case `27` читает ту же тройку полей через
[`0x6F554320`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554320.json)
перед обработкой команды.

## Условный выход в транспорт

`0x6F2CA010` передаёт созданный store в `0x6F54D970` с `edx=0`.
Тот берёт session из TLS, требует совпадения слота с
`session+0x610`, состояния соответствующего слота не ниже `3`
и непустых сериализованных данных. Для слота `0` при наличии
таблицы игроков дополнительный `0x6F545270` может отклонить
первый байт команды. Иначе bytes добавляются в буфер
`session+0x1C78`, если размер остаётся меньше `0x400`; при
достижении порога вызывается `0x6F54D930` и размер проверяется снова.

У `0x6F54D930` для mode, отличного от `LOOP` и `NONE`, есть путь
`0x6F650480`: последний отделяет первые 8 байт буфера, проверяет
ненулевую полезную длину до `0x400`, берёт NetClient из TLS и
вызывает `0x6F657340(data,size)`. Этот CNetClient method передаёт
аргументы виртуальному slot `0` объекта `client+0x2B4`.
Конкретный provider ниже этого вызова и итоговая доставка адресатам
не разрешены. Для `LOOP/NONE` `0x6F54D930` идёт в другую функцию
`0x6F54D760`, не в показанный `0x6F650480`.

## Контроли, расхождение и предел

Положительный статический контроль: при нулевом втором аргументе и
положительном `0x80239/0x8023A` собственный CUnit входит в builder;
writer действительно записывает один байт индекса, один байт события
и две dword. При выполненных session/size/mode/TLS gates возможен
переход до виртуального send provider.

Отрицательные контроли: отсутствие обоих соответствующих событий
пропускает builder; ненулевой второй аргумент идёт по другой ветви;
несовпадение session slot или состояние `<3` не добавляет bytes;
отсутствие NetClient не достигает send provider. Запись в очередь
сама по себе не доказывает немедленный flush. В показанном payload
нет координат, life, inventory, fog mask или результата
`SubmitUnit`/`PublishPosition` — это **команда выбора с парой полей
идентичности**, а не снимок состояния юнита. Вход `0xD01D4` из
[FND-0039](FND-0039-unit-d01d4-selection-publication-gate.md)
обновляет локальный selection state и прямо этот builder не вызывает.
Одна проверенная команда не исключает независимого serializer state.

В S10 [`cunit_agent6_netselectionevent.cpp`](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Net/cunit_agent6_netselectionevent.cpp)
меняет местами два назначения (`source+0x0C → cmd+0x20`), сам
помечает участок `DIFFERS` и оставляет порядок неопределённым.
Адресное S11 тело показывает противоположный порядок через
stack slots `var_14` (`cmd+0x1C`) и `var_10` (`cmd+0x20`).
Это конкретное расхождение реконструкции, а не результат
воспроизведения Game.dll.

Статусы S11: два CUnit входа — `THUNK`, builder и queue flush —
`DIFFERS`, serializer entry, writer, command queue, flush decision —
`TODO`, client send — `EXACT`; для всех названных адресов просмотрено
IDA-тело, но собственное исполнение не проводилось. Граница
server→client выдачи **полей состояния** остаётся открытой:
требуется найти независимый writer, получателя/игрока и вызов,
который соединяет его с правом видимости. Следующий опыт — при
двух игроках записать bytes команды `0x1B` и отдельно изменения
position/life юнита; сравнить outbound path при доступном и
скрытом юните, не принимая совпадение тайминга за зависимость.

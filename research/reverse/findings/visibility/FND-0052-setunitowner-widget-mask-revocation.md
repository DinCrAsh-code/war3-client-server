# FND-0052 — `SetUnitOwner` условно снимает биты представления до смены владельца

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, ownership, player-mask, selection, GameUI, revocation |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен; адреса — координаты S11. Проверен вход JASS `SetUnitOwner` и прямой owner-change маршрут; runtime и исходная DLL не воспроизводились. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [регистрация `SetUnitOwner`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/pipeline/docs/targets/jass-natives-registration-table.md), [native `0x6F3C5ED0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C5ED0.json), [owner-change routine `0x6F2A4BE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A4BE0.json), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [mask gate `0x6F2AD7E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD7E0.json), [mask writer `0x6F2AF6A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AF6A0.json), [selection membership `0x6F421E20`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F421E20.json), [selection presentation `0x6F28DCF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F28DCF0.json) |

## Причинный маршрут

JASS `SetUnitOwner(Hunit,Hplayer,B)` (`0x6F3C5ED0`) разрешает
сначала unit handle, затем player handle. Только при двух ненулевых
результатах он берёт byte `player+0x30` и вызывает
`0x6F2A4BE0(unit, newIndex, B, 1)`. Последняя функция сохраняет
старый индекс `unit+0x58`. Если `newIndex == oldIndex`, она
переходит прямо к выходу: mask/selection/event ветви не выполняются.

При разных индексах owner-change routine вызывает виртуальный
CUnit slot `+0x98` **до** записи нового `unit+0x58`
(`0x6F2A4C66–70` против `0x6F2A4E1D`). В S11 vtable CUnit
этот slot — `0x6F2AD7E0`. Он берёт 16-битные маски существующих
игроков и `unit+0x2C/+0x2E`, проверяет их пересечение, затем
виртуальный predicate `+0xF8` и, после исключения собственного
owner-бита, вызывает CUnit `+0x110` с оставшейся маской.
Target `+0x110` — `0x6F2AF6A0`; он выполняет
`unit+0x2C &= ~mask` и условно передаёт созданного агента
уведомления в GameUI. Точные gate и побочные действия этого
mask writer описаны в [FND-0027](FND-0027-widget-mask-notification.md).
Следовательно, смена владельца имеет прямой **условный** путь
отзыва части сохранённых битов `unit+0x2C` до присваивания
нового owner index. Это не утверждение, что снимаются все биты
старого владельца или очищается fog-сетка.

После этого `0x6F2A4BE0` меняет учёт старой и новой записи игрока,
перепривязывает связанный с юнитом path client и присваивает
`unit+0x58 = newIndex`. Ближе к концу функция проверяет наличие
юнита в selection list локальной записи через `0x6F421E20`.
Если membership положителен, она вызывает CUnit vtable
`+0x194` (`0x6F28DCF0`) с аргументами
`(1, 0, 1, 0, newPlayer+0x30)`.
Тело `0x6F28DCF0` обновляет selection circle и связанные
presentation эффекты; это **не** `CSelectionWar3::Remove`
`0x6F424CE0`, и в данном вызове нет доказанного unlink из списка.
Хотя в общем теле `0x6F28DCF0` есть путь к CUnit `+0x1D0/+0x1D4`
и команде выбора из [FND-0050](FND-0050-unit-selection-command-outbound-boundary.md),
**данный** вызов туда не идёт: `arg_C == 0` переводит исполнение
с `0x6F28DF90` на `0x6F28DFF1`, а `arg_4 == 0` пропускает
следующую отдельную ветвь.
Далее создаётся `CEventOwnerChange` с ID `0xD01A2`, старым
индексом и данными перехода и отправляется через CUnit vtable
`+0x10`. Состав observer recipients за этим входом здесь не
разрешён.

## Контроли и предел

Положительный статический контроль: валидные unit/player handles,
разные индексы владельцев, непустое пересечение mask, положительный
`+0xF8` и оставшийся после исключения owner-бита ненулевой mask
доводят `0x6F3C5ED0 → 0x6F2A4BE0 → +0x98 → +0x110`
до записи `unit+0x2C &= ~mask` **раньше** `unit+0x58 = newIndex`.
При membership в локальном selection list дополнительно достижим
presentation target `+0x194`.

Отрицательные контроли: неразрешённый unit или player handle
завершает native до owner-change routine; `newIndex == oldIndex`
пропускает весь переход; пустое пересечение, отказ `+0xF8` или
только исключаемый owner-бит не достигают mask writer; отсутствие
юнита в локальном selection list пропускает `+0x194`.
Отсутствующий GameUI пропускает UI sink **после** возможного
изменения `unit+0x2C`. Даже при membership вызов `+0x194`
из этого owner-change пропускает свой Net command builder из-за
`arg_C == 0`; другие callers `+0x194` могут его достигать.

В прямых телах до `+0x110` нет вызова fog writer `0x6F409E00`,
incremental refresh `0x6F40A650` или full rebuild `0x6F40A8F0`.
`unit+0x2C` — маска виджета, а не fog plane `+0x30` из
[FND-0019](FND-0019-unit-fog-writer-modes.md). Поздние события
`0xD01A2`, обновления player record `0x80261` и другие helpers
могут иметь непрослеженные эффекты. Поэтому по этой цепи нельзя
заключить, когда снимается прежний радиус обзора, исчезает спрайт
или отзывается уже доставленное клиенту состояние.

В S11 native имеет статус `THUNK`, owner-change routine — `TODO`,
mask gate и writer — `DIFFERS`; это статический разбор IDA-body,
а не проверка совпадения S10 C++ с оригиналом. Следующий опыт:
на юните с двумя сторонними наблюдателями и отдельным владельцем
поменять owner при контролируемых `unit+0x2C/+0x2E`, fog `+0x30`
и membership локального выбора; сравнить исходы при равном owner,
пустом mask и отсутствующем GameUI, затем проследить получателей
`0xD01A2`.

# FND-0032 — перемещение юнита обновляет пространственные сетки; связь с отзывом тумана открыта

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, unit, movement, pathfinding, spatial-grid, observer, revocation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса — координаты S11; исследованы прямой путь `CUnit::Reposition` и его пространственные callees, но не все виртуальные реализации и внешние события |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [Reposition `0x6F2A5D50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A5D50.json), [MoveTo `0x6F2AC220`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AC220.json), [handle delta `0x6F474250`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F474250.json), [trace delta `0x6F4737D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4737D0.json), [ApplyDelta `0x6F4A7380`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A7380.json), [RecomputeOrigin `0x6F4A6FD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A6FD0.json), [grid updates `0x6F4A6D70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A6D70.json) / [`0x6F4A6E40`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A6E40.json), [grid registration `0x6F49FF90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F49FF90.json), [footprint removal `0x6F2AD300`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD300.json), [registration teardown `0x6F4A00B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A00B0.json), [idle callback `0x6F49E850`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F49E850.json) |

## Вывод

`CUnit::Reposition` (`0x6F2A5D50`) при положительном `QueryVisibleImpl(1)`
или бите `unit+0x248 & 0x200` перед перемещением вызывает виртуальный
`RemoveFootprint` (`+0x14C`, для CUnit — `0x6F2AD300`) и
`DispatchPositionNotifyState(0)` (`0x6F283BC0`). Затем **все ветви**
достигают `CWidget::MoveTo` (`0x6F2AC220`). После него Reposition при
`QueryVisibleImpl(1)` или указанном бите, а также при `unit+0x1EC == 0`,
вызывает виртуальный `AddFootprint` (`+0x148`, для CUnit — `0x6F2AD0C0`)
и `DispatchPositionNotifyState(1)`. Эти вызовы поддерживают размещение
юнита и его представления; в прочитанном прямом пути вызова fog writer нет.

`MoveTo` без отдельного gate вызывает
`SHandleWithType::FlushedOriginDelta` (`0x6F474250`) для нового положения.
При успешно разрешённом handle дальнейший путь идёт в
`CPathTrace::AddOriginDelta` (`0x6F4737D0`) →
`ApplyDelta` (`0x6F4A7380`) → `RecomputeOrigin` (`0x6F4A6FD0`).
Последний пересчитывает origin и обновляет **две пространственные
регистрации** trace: path grid через `0x6F4A6D70` и collision grid через
`0x6F4A6E40`. Обе доходят до `CGridRegistration::UpdateBox`
(`0x6F49FF90`), который сравнивает прежний и новый прямоугольники и
перерегистрирует изменившиеся клетки. Это доказанный путь снятия
**старого пространственного footprint**, не доказательство снятия старого
радиуса в fog-плоскости.

После `RecomputeOrigin` функция `ApplyDelta` вызывает виртуальный observer
`CPathTrace` по слоту `+0x54` с адресом нового `trace+0x78` **только при
ненулевом флаге notify**. `MoveTo` выставляет этот флаг как
`(MoveTo.arg_1C == 0)`; `Reposition` передаёт туда свой `arg_1C`.
Конкретный target слота `+0x54` из прочитанного call tree и доступных
классов S11 не установлен. Поэтому возможный мост от движения к fog
остаётся открытым именно здесь.

Отдельная ветвь `RemoveFootprint` удаляет объект `unit+0x34` через
`0x6F47C100` → `CGridRegistration::TeardownRegistration`
(`0x6F4A00B0`). Teardown при отсутствии бита `registration+0x40 &
0x10000000` снимает регистрацию прежних клеток, освобождает slot таблицы
presence и затем вызывает `0x6F49E850`. Последняя функция возвращается
сразу при `owner+0x38 != -1`; иначе, если младшие 24 бита
`owner+0x3C` равны нулю, вызывает **другой** виртуальный callback
`owner.vtable+0x10` с аргументом `0`. Его concrete target также не
установлен. Снятие path registration и освобождение presence slot нельзя
отождествлять с очисткой `fogTable+0x30`.

## Основание и контроли

Положительный статический контроль: при вызове `Reposition` ветвь
`MoveTo` достигает вызова handle-delta независимо от gate на footprint;
при разрешённом CPathTrace она доходит до `ApplyDelta`;
`RecomputeOrigin` вызывает обе функции обновления сеток. Если новый
прямоугольник регистрации отличается, `UpdateBox` проходит ветвь
перерегистрации прежних и новых клеток. При `arg_1C == 0` дополнительно
вызывается observer `+0x54`.

Отрицательные контроли: при `arg_1C != 0` пересчёт origin и сеток
сохраняется, а observer `+0x54` пропускается. Если четыре координаты
прямоугольника в `UpdateBox` не изменились, функция возвращается без
перерегистрации клеток. `RemoveFootprint` пропускает teardown при
`unit+0x34 == 0`; в самом teardown бит `0x10000000` отключает обход
прежних клеток. Вызов `0x6F49E850` не означает безусловного callback:
его собственные `+0x38/+0x3C` gate могут завершить путь раньше.

## Ограничения и отвергнутые гипотезы

Чтение pinned S11 и реконструкции коллеги в `pipeline/src/Sync/`
не является исполнением DLL. Проверенный путь доказывает обновление
path/collision registration, но **не доказывает**, что `Reposition`
непосредственно вызывает [unit fog writer](FND-0019-unit-fog-writer-modes.md),
[fog invalidation](FND-0028-unit-fog-invalidation.md) либо
[полный rebuild](FND-0026-unit-fog-rebuild-trigger.md). В прямых телах
из этой цепи таких вызовов нет; это отрицательный поиск ограниченной
глубины, а не доказательство отсутствия cleanup в движке.

Открыты concrete targets `CPathTrace.vtable+0x54` и
`pending-owner.vtable+0x10`, связь с удалением самого CUnit, возможные
внешние события после движения, момент таймерного rebuild и значение
`dword_6FAB6A34 & 3`, которое определяет очистку `fogTable+0x30`
в [FND-0024](FND-0024-alliance-fog-rebuild.md). Поле `+0x30` у
`CGridRegistration` — его собственная пространственная структура;
совпадение offset с `fogTable+0x30` не связывает объекты.

## Значение для проекта

Для клиент-серверной выдачи данных нельзя считать обновление spatial grid
гарантией отзыва видимости у игрока. Следующий проверяемый шаг — разрешить
оба виртуальных target на конкретном классе и проследить удаление CUnit
до fog writer/rebuild, затем проверить на двух игроках старую и новую
клетку и момент прекращения выдачи скрытого состояния.

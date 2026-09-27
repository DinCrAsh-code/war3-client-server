# FND-0026 — fog rebuild имеет таймерный вход, но отзыв при уходе юнита не доказан

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, grid, unit, timer, lifecycle, revocation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [world activation `0x6F3A2920`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A2920.json), [activation callers `0x6F3AFE90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3AFE90.json) / [`0x6F3B0750`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3B0750.json), [timer callback `0x6F4299E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4299E0.json), [dispatcher `0x6F42FC80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F42FC80.json), [`CGameState` vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F94F6E4.json), [timer arm/cancel `0x6F426C50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F426C50.json), [teardown `0x6F3A2EC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A2EC0.json), [rebuild `0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json) |

## Вывод

У полного пересчёта fog `0x6F40A8F0` в S11 есть три прямых входа:
активация мира `0x6F3A2920`, путь изменения отношений
[FND-0024](FND-0024-alliance-fog-rebuild.md) `0x6F43E960` и
tail-jump из `0x6F4299E0`. Последний — обработчик события `0x80269`
в `CGameState` vtable slot `3`: `0x6F42FC80` выбирает его для IDA
case `524905`. `0x6F426C50(1)` вызывает timer arm `0x6F4778F0`
с этим кодом события; `0x6F426C50(0)` вызывает timer cancel
`0x6F477D30`. Обработчик `0x6F4299E0` снова arm-ит таймер, если
`CGameState+0x290 & 2 == 0`, и в обоих случаях передаёт
`globalPlayerTable+0x34` в `0x6F40A8F0`. Это статически подтверждает
самоповторяющийся путь пересчёта, пока его guard и жизненный цикл таймера
позволяют новые события. Интервал и момент исполнения не установлены.

Активация `0x6F3A2920` сначала ставит `world+0x3E0=1`, затем вызывает
rebuild на `world+0x34` (`0x6F3A29E2`–`0x6F3A29E5`). Её прямые
call sites — `0x6F3AFE90` и `0x6F3B0750`; второй перед активацией
включает таймер через `0x6F426C50(1)` и содержит отдельный путь карты
`WarcraftIIICredits.w3m`. Teardown
`0x6F3A2EC0` ставит `world+0x3E0=0` и при ненулевом `world+0x1C`
отменяет timer через `0x6F426C50(0)`.

Каждый из этих входов действительно **очищает `fogTable+0x30` только
если** `(dword_6FAB6A34 & 3) == 0` в начале `0x6F40A8F0`.
Иначе `memset` пропускается, а проходы fog-объектов и юнитов
продолжаются. После допустимых источников функция вновь вызывает
`ApplyUnitFogRadius` для юнитов; сам этот writer не содержит общего
удаления ранее установленного бита `+0x30` — [FND-0019](FND-0019-unit-fog-writer-modes.md).

## Основание и контроли

Положительный статический контроль: `0x6F426C50(1)` передаёт код
`0x80269` в `SAgentTimerArm::Arm`; dispatcher `0x6F42FC80`
вызывает `0x6F4299E0` для того же кода, а тот при сброшенном бите
`+0x290 & 2` повторно вызывает `0x6F426C50(1)` и затем tail-jump
в rebuild. `0x6F40A8F0` при нулевых младших двух битах глобального
слова выполняет `memset` всей плоскости `+0x30` до повторного нанесения.

Отрицательные контроли: установленный бит `CGameState+0x290 & 2`
пропускает **повторное arm**, хотя текущий rebuild всё равно выполняется;
`0x6F426C50(0)` отменяет timer. Ненулевой
`dword_6FAB6A34 & 3` пропускает очистку `+0x30` даже на timer-пути.
В просмотренных прямых ссылках на rebuild нет вызова из конкретного
`CUnit::RemoveFromWorld` или обработчика движения. Поиск текстовых
ссылок в S11 не покрывает виртуальные/косвенные вызовы и не является
доказательством их отсутствия.

Поиск адреса `6FAB6A34` во всём Git-дереве S11 находит только чтение в
`0x6F40A8F0`, без прямой записи по этому обозначению. Массив
`word_6FAB6A14[0..15]`, который rebuild заполняет перед проверкой,
заканчивается раньше `6FAB6A34`. Статический initializer
[`0x6F85DD00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F85DD00.json)
передаёт `0x30` в вызов для объекта от `6FAB6A00`, но его аргументы
не устанавливают значение `6FAB6A34`. Это ограничивает **прямые
текстовые ссылки**, а не доказывает ноль в живой DLL: возможны начальное
значение образа и косвенная/алиасная запись.

## Ограничения и значение для проекта

`0x6F3A2920`, `0x6F4299E0` и dispatcher в S11 имеют статус `TODO`;
timer arm остаётся `THUNK`, cancel — `EXACT`. Здесь не было исполнения
оригинальной DLL, замера интервала или наблюдения ухода юнита. Не
установлены источник изменения `dword_6FAB6A34`, все пути запуска
таймера в обычном матче, порядок timer event относительно удаления
юнита и перекрывающиеся источники обзора. Поэтому обещать отзыв
`+0x30` после ухода отдельного юнита нельзя. Следующая проверка:
динамически снять обе плоскости и ответы [FND-0018](FND-0018-fog-grid-result-table.md)
до/после удаления или перемещения одного юнита при двух значениях
`dword_6FAB6A34 & 3`, включая контроль второго источника обзора.

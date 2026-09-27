# FND-0034 — деактивация юнита снимает pathing footprint, но отзыв fog не прослежен

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, removal, deactivation, lifecycle, fog, pathfinding, observer |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса — координаты S11; исследован `CUnit::Deactivate` и его путь spatial teardown, но вызов из `RemoveUnit` и вся обработка событий не восстановлены |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [unit deactivation `0x6F282920`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F282920.json), [selectable deactivation `0x6F2C7410`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C7410.json), [widget deactivation `0x6F2ABEA0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2ABEA0.json), [RemoveFootprint `0x6F2AD300`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD300.json), [registration release `0x6F47C100`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F47C100.json), [spatial teardown `0x6F4A00B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A00B0.json), [agent event `0x6F2AB1E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AB1E0.json), [observer post `0x6F62A570`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A570.json), [KillUnit `0x6F3C8040`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C8040.json), [S10 C++](../../../../src/Unit/unit_deactivate.cpp), commit `2fc76c8035554912dd66fc0a06a39eda376a806c` |

## Вывод

В S11 vtable CUnit slot 13 (`+0x34`) указывает на
`CUnit::Deactivate` (`0x6F282920`). Этот метод без условного gate
вызывает `0x6F2C7410`, а тот начинает с `CWidget::Deactivate`
(`0x6F2ABEA0`). Последний вызывает виртуальный slot `+0x13C`
юнита (`0x6F28B2F0`) для выбора аргумента и **всегда** вызывает
slot `+0x14C` — `RemoveFootprint(0, useAlternate)`
(`0x6F2AD300`). При ненулевом `unit+0x34` RemoveFootprint выполняет
`0x6F47C100` и освобождает массив регистраций; каждая его запись
проходит `0x6F4A00B0` и снимается с пространственной сетки согласно
своим флагам — [FND-0033](FND-0033-spatial-observer-concrete-targets.md).
После этого `unit+0x34` становится нулём. Это конкретный путь
снятия **pathing footprint при вызове Deactivate**.

Сам `CUnit::Deactivate` также вызывает `0x6F27A5A0` для motion state,
`0x6F2E5A70` для одного optional handle, освобождает другие ссылки,
отменяет три таймера и передаёт `(unit, 0)` в `0x6F2AB1E0`.
Последняя функция выбирает ключ `0xD01A1` и при аргументе `0`
делает tail-jump в `0x6F62A570`, которое при ненулевой таблице
observer вызывает `0x6F62A000`. Эта ветвь **снимает регистрацию**
`(0xD01A1, unit)` из таблицы, а не доставляет сообщение подписчику;
детали и отличие от пути доставки — [FND-0035](FND-0035-unit-deactivation-event-registration-release.md).
В прочитанном непосредственном пути **нет прямого вызова**
`RefreshUnitFog` `0x6F40A650`, unit writer `0x6F409E00` или
полного rebuild `0x6F40A8F0`. Возможные эффекты release на нулевом
refcount и других виртуальных вызовов не разрешены.

JASS `KillUnit` (`0x6F3C8040`) разрешает unit handle и вызывает
slot `+0x124` (`CUnit::SetLife`) со значением ноль. Он сам не вызывает
`Deactivate` или fog writer в своём десятиинструкционном теле.
JASS `RemoveUnit` зарегистрирован в S11 как `loc_6F3C8060`, но
отдельного IDA-тела этого entry в просмотренном реестре нет.
Поэтому связь **`RemoveUnit` → `CUnit::Deactivate`** и момент её
исполнения здесь не установлены. `KillUnit` и `RemoveUnit` не следует
сливать в один removal path.

## Основание и контроли

Положительный статический контроль: `0x6F282920` вызывает
`0x6F2C7410` без ветвления, тот вызывает `0x6F2ABEA0`, а
`0x6F2ABEA0` в обеих ветвях результата slot `+0x13C` вызывает
`+0x14C`. Для CUnit S11 vtable этот target равен `0x6F2AD300`.
Когда `unit+0x34 != 0`, `0x6F2AD300` передаёт массив
`0x6F47C100`, который вызывает `0x6F4A00B0` для каждой
существующей регистрации. При отсутствии бита
`registration+0x40 & 0x10000000` последняя функция действительно
обходит старый пространственный прямоугольник для снятия клеток.

Отрицательные контроли: `unit+0x34 == 0` пропускает release и
teardown, хотя вызов RemoveFootprint сохраняется. Бит
`registration+0x40 & 0x10000000` пропускает обход старых клеток,
но release slot и дальнейшая очистка регистрации продолжаются.
`0x6F62A570` возвращается без обхода таблицы регистраций при нулевом
указателе observer `+0x08`. В `KillUnit` нулевой результат разрешения
handle завершает native без вызова `SetLife`.

## Ограничения и значение для проекта

Это чтение опубликованных S11 IDA-тел и S10 C++, не запуск DLL.
Имя `Deactivate` подтверждается S10-реконструкцией и местом в vtable,
но статическая цепь начинается **с вызова этого метода**, а не с
конкретного JASS `RemoveUnit`, смерти юнита или истечения decay-таймера.
Тело `loc_6F3C8060` не восстановлено; handle notifications `+0x5C`
и освобождение refcount-объектов не исчерпаны. Это ограничение покрытия,
не доказательство отсутствия
fog cleanup в них или в timer-пути [FND-0026](FND-0026-unit-fog-rebuild-trigger.md).

Снятие pathing footprint не очищает само по себе `fogTable+0x30`.
Для безопасной выдачи скрытого состояния следующий приоритет — получить
тело `RemoveUnit`, связать его с deactivation и event listeners, затем
наблюдать старую и новую fog-клетку после ухода единственного источника
обзора и после rebuild при контроле `dword_6FAB6A34 & 3`.

# FND-0018 — две плоскости fog-клетки проходят через общую таблицу ответа

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, grid, unit, jass, player-mask |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [Prepare `0x6F26D0C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26D0C0.json), [SubmitFromGrid `0x6F00E830`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F00E830.json), [Submit `0x6F00E7A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F00E7A0.json), [IsPointVisible `0x6F3BA430`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3BA430.json), [IsPointFogged `0x6F276240`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F276240.json), [IsPointMasked `0x6F276290`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F276290.json) |

## Вывод

`Prepare` и `SubmitFromGrid` читают одну пару 16-битных массивов клетки:
`this+0x30` превращается в `codeA = word | 0xF000`, а `this+0x2C` — в
`codeB = word & 0x0FFF`. Первый вход переводит мировую точку в клетку,
второй получает её индексы из позиции юнита; оба вызывают `Submit(codeA,
codeB, playerMask)`. Это связывает плоскости writer из
[FND-0013](FND-0013-unit-fog-writer-planes.md) с ответом по точке и
оставшимся после ранних gate путём юнита из
[FND-0010](FND-0010-unit-submit-gates.md).

`Submit` выбирает строку `r = field(+0x10) + 2*field(+0x14)`, затем
колонку:

1. Если младшие 16 бит `codeA & playerMask` ненулевые — колонка 0.
2. Иначе, если младшие 16 бит `playerMask & ~codeB` ненулевые — колонка 1.
3. Иначе — колонка 2.

| `r` | Колонка 0: пересечение с `codeA` | Колонка 1: бит маски вне `codeB` | Колонка 2: остальные |
|---|---:|---:|---:|
| 0 | 4 | 4 | 4 |
| 1 | 4 | 4 | 1 |
| 2 | 4 | 2 | 2 |
| 3 | 4 | 2 | 1 |

Три JASS-маршрута точки вызывают общий `Submit` и сравнивают результат
соответственно с `4` (`IsVisibleToPlayer`), `2` (`IsFoggedToPlayer`) и
`1` (`IsMaskedToPlayer`). Это **коды ответа запроса точки**, не готовая
политика выдачи полей юнита.

## Контроли и границы

Положительный статический контроль: `Prepare` (`EXACT`) и
`SubmitFromGrid` (`DIFFERS`) применяют одинаковые `| 0xF000` и
`& 0x0FFF` перед `Submit` (`DIFFERS`). В нём колонка 0 возвращает `4`
при любой строке. Поскольку `codeA` принудительно содержит `0xF000`,
маска с битом `12..15` попадает в колонку 0 независимо от содержимого
двух массивов клетки; например, маска `0xFFFF` из
[FND-0017](FND-0017-local-unit-publication-masks.md) даёт этот путь.

Отрицательный контроль: нулевая маска не попадает ни в колонку 0, ни в
колонку 1, но таблица всё же возвращает значение колонки 2, которое
зависит от `r`. Поэтому нельзя считать `Submit(..., 0)` универсальным
«нет права/нет ответа». При отсутствии пересечения с `codeA` смена только
бита второй плоскости может переключить колонки 1/2; результат при этом
зависит от строки, а не просто от одной плоскости.

В S11 запросы точки и `Prepare` отмечены `EXACT`; `SubmitFromGrid` имеет
низкий instruction score из-за распределения регистров, `Submit` —
`DIFFERS` (порядок операндов одной инструкции). Это анализ IDA-тел,
без собственного выполнения DLL. Писатели полей строки `+0x10/+0x14`,
допустимые значения `r` в живом матче и порядок изменения клеток не
установлены; таблица не проверяет границу `r`. Следующий опыт должен
сравнить обе плоскости клетки, `r`, три запроса точки и `SubmitUnit`
до/после одиночной записи карты или юнита, включая контроль без обзора.

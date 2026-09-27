# FND-0029 — полный fog rebuild передаёт производную сетку в terrain-представление

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, grid, rebuild, presentation, terrain, network-boundary |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [rebuild `0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json), [выход `0x6F406CC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F406CC0.json), [подготовка `0x6F406B00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F406B00.json), [переход `0x6F00F880`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F00F880.json), [singleton `0x6F01F5A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F01F5A0.json), [потребитель `0x6F759D60`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F759D60.json), [обновление клетки `0x6F759B70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F759B70.json), [terrain-запись `0x6F7599A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F7599A0.json), [terrain refresh `0x6F752570`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F752570.json), [dirty update `0x6F74EEA0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F74EEA0.json), [CVertexUndoable `0x6F743A80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F743A80.json), [S10 Storm singleton](../../../../src/Storm/stormsingletona.h) |

## Вывод

После проходов fog-объектов и юнитов полный rebuild `0x6F40A8F0`
вызывает `0x6F406CC0` на `0x6F40ADF8`–`0x6F40ADFA`. Этот вызов стоит
перед поздней развилкой по `dword_6FAB6A34 & 3`, поэтому не зависит от
неё. `0x6F406CC0` сначала меняет **производный 16-битный буфер**
`fogTable+0x34` через `0x6F406B00`; затем передаёт указатель на него,
значения полей `+0x60`, `+0x6C`, `+0x64` и маску
`word(fogTable+0x3C) | 0x8000` в `0x6F00F880`. Wrapper передаёт те же
пять значений в `0x6F759D60` объекта, возвращённого lazy singleton
`0x6F01F5A0`. Singleton хранится в `dword_6FAAE790`; S10 называет его
`SStormSingletonA`, но его опубликованный layout не содержит нового
контракта этих конкретных fog-вызовов.

`0x6F406B00` **не копирует целиком `+0x30` в `+0x34`**. Для каждого
посещённого 16-битного слова пусть `D` — прежнее слово `+0x34`, `S` —
слово `+0x30`, `M = word(fogTable+0x3C)`. Младшие 15 бит `D` всегда
сохраняются. Старший бит нового слова равен:

| Условие | Новый бит `0x8000` |
|---|---|
| `(fogTable+0x24 & 1) == 0` и хотя бы одно из полей `+0x10/+0x14` ненулевое | При `(S & M) != 0`: `(D & 0x7FFF) != 0`; иначе прежний `(D & 0x8000) != 0` |
| Остальные случаи | `(D & 0x7FFF) != 0` |

Это формула только для слов, обойдённых циклом `0x6F406B00`; границы
таблицы и допустимость её параметров здесь отдельно не подтверждались.
Сравнение с запросами игровой видимости по `+0x30/+0x2C` из
[FND-0018](FND-0018-fog-grid-result-table.md) показывает другую роль
`+0x34`: это вход в последующее обновление terrain-записей, а не одна
из двух плоскостей ответа `Submit`.

`0x6F759D60` для внутренней клетки требует ненулевого пересечения с
переданной маской у **четырёх соседних слов** буфера. Полученный boolean
сравнивается с битом `2` байта `+0xC` соответствующей 28-байтной записи
singleton `+0xE4`. При различии и пройденных воротах он вызывает
`0x6F759B70(x, y, boolean)`. Последний повторно проверяет прежнее
состояние, а для включения требует ещё положительного `0x6F7598C0`;
затем `0x6F7599A0` меняет terrain-состояние через `0x6F752570` и
`0x6F74EEA0`. Конструктор промежуточного объекта использует vtable
`CVertexUndoable`; `0x6F74EEA0` копирует запись клетки и выставляет
dirty-поле singleton `+0x938 = 1`. Это статически прослеженный
**локальный terrain/presentation sink**. В этой цепи передачи буфера
нет вызова сетевого транспорта или сериализации.

## Основание и контроли

Положительный статический контроль: аргументы `0x6F406CC0` проходят
через `0x6F00F880` без преобразования; у получателя `0x6F759D60`
первый аргумент действительно используется как указатель на 16-битные
слова, а последний — как маска `test` этих слов. `0x6F747C20` читает
тот же бит `2` записи, с которым сравнивает `0x6F759D60`; дальнейший
`0x6F74EEA0` ставит dirty-поле terrain singleton.

Отрицательные контроли: изменение `+0x30`, не меняющее предикат
`(S & M) != 0`, не меняет формулу `+0x34` в первой ветви; во второй
ветви значение `S` вообще не участвует. Даже изменение `+0x34` может
не вызвать terrain update, если не все четыре соседних слова имеют
пересечение с маской, boolean уже совпадает с битом записи, пара флагов
`0x200/0x100` блокирует вызов или `0x6F7598C0` отклоняет включение.
Нулевой размер обхода также не даёт вызовов клеточного обновления.

## Границы для клиент-серверного проекта

Это чтение IDA-тел S11 без запуска DLL и без сетевого trace. Указанные
функции, кроме singleton getter `DIFFERS`, имеют статус `TODO` в
адресном реестре. Вывод о локальном потребителе касается **этой прямой
цепи**, а не всех возможных выходов rebuild, соседних callback или
поздних вызовов `0x6F00FAC0`/`0x6F016CD0`. Отсутствие сетевого
вызова здесь не доказывает отсутствие сетевой выдачи где-либо ещё.
Terrain-результат нельзя принимать за момент, когда серверу разрешено
раскрыть юнит либо карту: правовой ответ остаётся в цепи
[FND-0018](FND-0018-fog-grid-result-table.md), а очистка `+0x30`
остаётся условной по [FND-0024](FND-0024-alliance-fog-rebuild.md).
Следующий контроль — на одиночном источнике обзора отдельно записать
обе игровые плоскости `+0x30/+0x2C`, производный буфер `+0x34`, четыре
слова клетки и бит terrain-записи до/после rebuild; параллельно
наблюдать реальную сетевую выдачу другому игроку.

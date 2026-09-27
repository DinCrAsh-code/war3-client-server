# FND-0017 — локальная публикация юнита читает две разные производные маски

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, player-relations, local-context, fog, presentation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [пересчёт масок `0x6F408070`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F408070.json), [публикация `0x6F285110`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F285110.json), [запрос без индекса `0x6F3A3830`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A3830.json), [запрос с индексом `0x6F3A38F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A38F0.json), [S10 C++](../../../../src/Agent/jassrefreshworldmasks.cpp) |

## Вывод

`0x6F408070` берёт запись локального слота `L` и сохраняет три слова в
объекте мира `+0x34`:

| Поле | Статически наблюдаемый расчёт |
|---|---|
| `worldState+0x3C` | `0xFFFF` при бите 0 поля `worldState+0x24`; иначе `0x0FFF`, если handle-ref записи L `+0xF0` ненулевой; иначе один бит `L` |
| `worldState+0x3E` | Прямая копия `record[L]+0x2E0` |
| `worldState+0x40` | OR слов `record[i]+0x2E0` для каждого бита `i` из `record[L]+0x2E0` |

Последнее — **один проход по маске L**, а не вычисление транзитивного
замыкания до неподвижной точки. Направленное происхождение `+0x2E0`
описано в [FND-0015](FND-0015-directed-alliance-mask.md).

`CUnit::PublishPosition` (`0x6F285110`) при ненулевом handle-ref локальной
записи `+0xF0` выбирает `0x6F3A3830` — вариант `SubmitUnit` без явного
индекса. В нём `worldState+0x40` подаётся на раннюю проверку юнита, а
`worldState+0x3C` — на запрос fog-сетки. Слово `+0x3E` этот вариант
не читает. Если `+0xF0` нулевой, публикация выбирает обычный
`0x6F3A38F0` с явным локальным индексом и маской его записи `+0x2E0`.
Значит, эти два входа нельзя свернуть в один «текущий player mask» при
проектировании раскрытия состояния.

## Контроли и пределы

Положительный статический путь: цикл `0x6F408070` сдвигает слово
`record[L]+0x2E0` до нуля и OR-ит маску каждого выбранного слота в
`+0x40`; `0x6F3A3830` отдельно загружает `+0x40` и `+0x3C` перед gate
и сеткой. Отрицательные: при нулевом `record[L]+0x2E0` итог `+0x40`
нулевой; при нулевом `record[L]+0xF0` `PublishPosition` не вызывает
`0x6F3A3830`; при отсутствующем активном world slot `+0x3E0`
публикация заканчивается до обоих вариантов запроса.

В S11 `0x6F408070` и `0x6F285110` имеют статус `DIFFERS`, оба `SubmitUnit`
— `TODO`; здесь сопоставлены IDA-тела, а не подтверждена эквивалентность
C++. Не установлены смысл `+0xF0`, допустимый порядок обновления этих
полей, все вызывающие `0x6F3A3830`, итоговое клиентское раскрытие и
полный набор читателей `+0x3E`. Отсутствие бита в одной маске нельзя
объявлять отказом доступа без остальных ветвей и динамического опыта.

Следующий опыт: зафиксировать локальный слот, менять по одному направленные
отношения A→L и B→A при постоянном тумане; сравнить три слова мира,
результаты обоих `SubmitUnit`, `PublishPosition` и наблюдаемый UI.
Отдельно проверить ветвь `+0xF0` и бит `worldState+0x24`; их способ
установки пока неизвестен, поэтому план не предполагает подмену адресов
в живом процессе.

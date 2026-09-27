# FND-0021 — три режима записи тумана карты меняют пару плоскостей по биту

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | jass, fog, grid, map-compatibility, player-mask |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [JASS-маска FND-0016](FND-0016-map-fog-shared-mask.md), [writer прямоугольника `0x6F3B0E90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3B0E90.json), [writer клетки `0x6F3B0E20`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3B0E20.json), [writer радиуса `0x6F3BA480`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3BA480.json), [строки `0x6F406850`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F406850.json), [`0x6F4069F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4069F0.json), [`0x6F406920`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F406920.json) |

## Вывод

После выбора `fogWord` в [FND-0016](FND-0016-map-fog-shared-mask.md)
JASS `SetFogState*` передаёт числовой режим writer сетки. В обоих
наблюдаемых видах записи — диапазон клеток (`0x6F3B0E90`) и одна клетка
(`0x6F3B0E20`) — режимы `1`, `2`, `4` делают над каждым битом слова
одинаковые переходы:

| Режим | Плоскость `+0x2C` | Плоскость `+0x30` |
|---|---|---|
| `1` | `old OR fogWord` | `old AND ~fogWord` |
| `2` | `old AND ~fogWord` | `old AND ~fogWord` |
| `4` | `old AND ~fogWord` | `old OR fogWord` |

Остальные значения режима в этих двух writer не записывают клетку.
`0x6F3BA480` (радиус) выбирает строковые writer для трёх тех же
режимов; крайние клетки записывает через `0x6F3B0E20` с исходным
аргументом режима. Точный набор клеток радиуса этим выводом не покрыт.

## Связь с запросом и контроли

По [FND-0018](FND-0018-fog-grid-result-table.md) читатель клетки
добавляет `0xF000` к слову `+0x30`, а у слова `+0x2C` оставляет младшие
12 бит. Для **одного выбранного младшего бита** `fogWord`, при отсутствии
других битов из маски запроса и строке ответа `r=3`, три записи дают
соответственно коды `1`, `2`, `4`. Это объясняет числовые режимы через
конкретную таблицу; при другой строке или составной маске ответ может
отличаться.

Положительный статический контроль: `0x6F3B0E20` (`EXACT`) содержит
три отдельные пары `or/and`, `and/and`, `and/or`; `0x6F3B0E90`
(`MATCH-PARTIAL`) повторяет их в диапазоне. В радиусе `0x6F3BA480`
(`THUNK`) видны вызовы `0x6F406850` (`and/or`), `0x6F4069F0`
(`and/and`) и `0x6F406920` (`or/and`); все три имеют `EXACT` в S11.
Отрицательные: неизвестный режим в `0x6F3B0E20` достигает `retn`
без записи, а диапазонный writer выходит после проверки режима.

Вывод относится к статическому IDA-потоку и указанным клеткам; игра не
запускалась. Границы/округление радиуса, порядок взаимодействия с fog
юнитов, время распространения в UI и поведение при некорректной области
не установлены. Из совпадения кода `4` для точки не следует право
отправить все поля юнита клиенту. Проверка: при фиксированных клетке,
маске и строке `r` последовательно применить `1→2→4→1`, сравнить обе
плоскости и три JASS-запроса точки; повторить на двух игроках для
одиночного и составного `fogWord` с отрицательным контролем вне области.

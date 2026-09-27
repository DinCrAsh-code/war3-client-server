# FND-0033 — spatial observer разрешается по классу; путь к fog writer остаётся открытым

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, unit, movement, pathfinding, spatial-grid, observer, teardown |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса — координаты S11; разрешены перечисленные vtable-классы, но конкретный runtime-класс handle, полученного из CUnit, не наблюдался |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [ApplyDelta `0x6F4A7380`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A7380.json), [CPoPos ctor `0x6F4875F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4875F0.json), [CPoPosCl ctor `0x6F487A80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F487A80.json), [CPoPos vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F9524A4.json), [CPoPosBh vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F952EF4.json), [CPoPosCl vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F952554.json), [CPoPosCl observer `0x6F495140`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F495140.json), [CPmRegion ctor `0x6F48C000`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F48C000.json), [CPmRegion vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F9529C4.json), [registration teardown `0x6F4A00B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A00B0.json), [idle gate `0x6F49E850`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F49E850.json), [slot 4 thunk `0x6F49E0D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F49E0D0.json), [second release `0x6F4AEF00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4AEF00.json) |

## Вывод

После пространственного пересчёта в [FND-0032](FND-0032-unit-reposition-spatial-observer-boundary.md)
`CPathTrace::ApplyDelta` (`0x6F4A7380`) вызывает виртуальный слот
`+0x54` с адресом нового origin `trace+0x78`, если флаг notify ненулевой.
Имя `CPathTrace` принадлежит реконструкции S11; IDA-конструктор
`0x6F4875F0` с тем же расположением origin `+0x78` и двух регистраций
`+0x94/+0x98` ставит vtable `NIpse::CPoPos`. Производный конструктор
`0x6F487A80` ставит vtable `NIpse::CPoPosCl`. Данные vtable S11
разрешают слот `+0x54` **условно на конкретный класс**:

| Runtime vtable | Target `+0x54` | Прочитанное действие |
|---|---|---|
| `NIpse::CPoPos` `0x6F9524A4` | `0x6F4879B0` | `retn 4`, пустой callback. |
| `NIpse::CPoPosBh` `0x6F952EF4` | `0x6F4879B0` | Тот же пустой callback. |
| `NIpse::CPoPosCl` `0x6F952554` | `0x6F495140` | Уведомление связанных пространственных записей при смене клетки. |

`0x6F495140` выполняется только при `trace+0xA8 != 0`, отличии
новой пары целочисленных клеток от кэша `+0xD0/+0xD4` и успешном
получении scratch-массива. Он собирает записи прежней и новой клетки,
обнуляет дубликаты, затем для оставшихся записей с ненулевым
`registration+0x30` и `registration+0x38 != -1` вызывает виртуальный
слот `+0x20` **объекта из `registration+0x30`**. Аргумент — указатель
на локальный tag buffer: два первых слова одинаковы, третье различается
для прежней (`0x6370266C`) и новой (`0x63702665`) клетки. После обхода
функция сохраняет новую клетку в `+0xD0/+0xD4`. Конкретные классы
получателей слота `+0x20` здесь ещё не разрешены. В этом прямом теле
нет записи в игровую fog-плоскость `fogTable+0x30`.

Другой виртуальный вызов из [FND-0032](FND-0032-unit-reposition-spatial-observer-boundary.md)
идёт по `CGridRegistration::TeardownRegistration` (`0x6F4A00B0`) →
`0x6F49E850` → `registration.vtable+0x10`.
`0x6F48C000` ставит на создаваемую регистрацию vtable
`NIpse::CPmRegion` (`0x6F9529C4`), где `+0x10` —
`0x6F49E0D0` → `0x6F4AEF00`. Последний освобождает вторую
handle/presence-запись через `dword_6FAB7788` при
`registration+0x14 != -1`, сбрасывает `+0x14/+0x18` в `-1`, затем
вызывает `CPmRegion.vtable+0x04` (`0x6F48AE20`), возвращающий объект
в пул. Это teardown spatial/presence, а не отзыв бита видимости игрока.

Для удаления **самого trace** у семейства CPoPos имеется другой слот
`+0x10`: у базового CPoPos это `0x6F4A7650`, который вызывает
`0x6F4A00B0` для обеих регистраций `trace+0x94/+0x98` и обнуляет
указатели. `CPoPosBh` `0x6F4AB430` и `CPoPosCl` `0x6F495F50`
делают собственную подготовку, затем делегируют базовому teardown.
Вызов этого слота при удалении конкретного CUnit не установлен.

## Основание и контроли

Положительный статический контроль: у объекта, созданного
`0x6F48C000`, vtable `CPmRegion`; `0x6F4A00B0` устанавливает
`registration+0x38 = -1` **до** `0x6F49E850`. Поэтому его gate
`+0x38 != -1` в этой конкретной цепи не блокирует последующий slot 4;
при нулевых младших 24 битах `registration+0x3C` вызов доходит до
`0x6F49E0D0` и освобождения второй записи. Это уточняет общий
отрицательный контроль в FND-0032.

Другой положительный контроль: при vtable `CPoPosCl`, notify `!=0`,
ненулевом `+0xA8`, смене клетки и доступном scratch-массиве слот
`+0x54` доходит до обхода старой и новой клетки и уведомляет только
записи, прошедшие его фильтры.

Отрицательные контроли: vtable `CPoPos`/`CPoPosBh` даёт пустой
`+0x54`. У `CPoPosCl` нулевой `+0xA8`, прежняя клетка или отсутствие
scratch-массива завершают callback без уведомления; дубликаты старой
и новой клетки обнуляются. В `0x6F49E850` ненулевые младшие 24 бита
`registration+0x3C` пропускают вызов `+0x10`. В `0x6F4AEF00`
sentinel `registration+0x14 == -1` пропускает освобождение через
`dword_6FAB7788`, но сброс полей и виртуальный `+0x04` остаются.

## Ограничения и значение для проекта

Это чтение S11, без собственного исполнения Game.dll. Статические
vtable-адреса разрешают target **после выбора класса**, но не доказывают,
что handle в `CUnit::Reposition` всегда указывает на `CPoPosCl` или
что связанный recipient `+0x20` имеет fog-эффект. Не прослежен вызов
`CPoPos` teardown из события удаления CUnit. `RemoveUnit` зарегистрирован
в JASS как `loc_6F3C8060`, но его тело отсутствует в просмотренной
выгрузке S11; `KillUnit` меняет life на ноль и не заменяет проверку
удаления. Нет доказанного пути к очистке `fogTable+0x30`.

Следующий шаг: установить тип объекта, возвращённого `LookupHandle`
для `CUnit+0x164`, разрешить `registration+0x30 → vtable+0x20` и
получить тело `RemoveUnit`/фактического lifecycle-вызова. Затем сравнить
обе fog-плоскости в прежней и новой клетке при двух игроках и
контролируемом источнике обзора. Успех spatial cleanup сам по себе
не устанавливает security-политику выдачи состояния.

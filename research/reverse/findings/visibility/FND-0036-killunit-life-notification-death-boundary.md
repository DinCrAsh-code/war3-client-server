# FND-0036 — `KillUnit` задаёт life=0 и открывает два канала уведомлений

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, KillUnit, life, death, observer, fog |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен; адреса — координаты S11. S10 использован как вторичная реконструкция, без запуска DLL. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [JASS `KillUnit` `0x6F3C8040`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C8040.json), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [SetLife `0x6F28B1F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F28B1F0.json), [tracked set `0x6F477350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F477350.json), [range update `0x6F4A92B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A92B0.json), [range notify `0x6F4A9230`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A9230.json), [range broadcast `0x6F497510`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F497510.json), [life event `0x6F2AB460`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AB460.json), [observer invoke `0x6F62A5D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A5D0.json), [S10 JASS life](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Jass/jassnatives_life.cpp) |

## Вывод

JASS `KillUnit` (`0x6F3C8040`) сначала разрешает unit handle через
`0x6F3BDCB0`. Только при ненулевом результате он передаёт адрес
`dword_6FAAE470` (ноль в S10) в CUnit vtable slot 73 (`+0x124`,
`0x6F28B1F0`). Сам native не вызывает метод смерти, деактивацию,
fog writer или rebuild. Важный предел: вызов `SetLife(0)` задаёт
целевое значение, а нижняя и верхняя границы фактического life-пула
применяются позже; без их состояния итоговое значение нельзя объявить
равным нулю для всякого юнита.

`0x6F28B1F0` передаёт выбранный CFloat в tracked ref юнита `+0x98`
(`0x6F477350`). Последний разрешает связанный range-объект, вычисляет
его значение на текущий момент и передаёт разницу в `0x6F4A92B0`.
Этот метод осаживает ramp (`0x6F4A90B0`), сохраняет прошлое значение,
ограничивает новое диапазоном `range+0x80/+0x84`, записывает его
в `range+0x78` и вызывает `0x6F4A9230` с новым и старым значениями.
Если бит `range+0x4F & 1` сброшен, `0x6F4A9230` создаёт событие
со значениями в `+0x10/+0x14`; из данного caller третий аргумент
равен нулю, поэтому событие идёт в `0x6F497510`. Тот обходит список
`range+0x64` и вызывает у каждого зарегистрированного получателя
vtable slot `+0x20`. Это первая открытая граница жизненного цикла.

После возврата из tracked ref `0x6F28B1F0` **безусловно** вызывает
`0x6F2AB460`. Он собирает сообщение `0xD01E6` с указателем
на widget и вызывает CUnit vtable `+0x10` (`0x6F629A90`), который
переходит на slot `+0x14` (`0x6F62A7B0`). При наличии observer
table `0x6F62A5D0` ищет записи с этим ключом и вызывает у их
ресурсов slot `+0x0C`. Это вторая открытая граница. Она отличается
от снятия регистрации `0xD01A1` при деактивации в
[FND-0035](FND-0035-unit-deactivation-event-registration-release.md).

## Контроли и проверенный получатель

Положительный контроль: у разрешённого unit `KillUnit` обязательно
входит в slot `+0x124`; `0x6F28B1F0` после `0x6F477350` всегда
переходит к `0x6F2AB460` без проверки «уже мёртв» или сравнения
старого и нового life. `0x6F4A92B0` вызывает range notify после
записи clamped значения; `0x6F4A9230` при сброшенном suppress-бите
и нулевом listener-аргументе вызывает `0x6F497510`. При наличии
записей в `range+0x64` он вызывает их slot `+0x20`.

Отрицательные контроли: неразрешённый unit handle завершает native
до `SetLife`; бит `range+0x4F & 1` пропускает range notify;
пустой список `range+0x64` не вызывает получателя. Независимое
сообщение `0xD01E6` при нулевой observer table CUnit `+0x08`
не входит в `0x6F62A5D0`; при отсутствии записи с ключом
`0xD01E6` не вызывается handler. Эти gates не отменяют уже
выполненную запись значения range.

Конкретный пример получателя `0xD01E6` есть в S11:
`CBuffDrainBonus`, `CBuffDrainBonusLife` и `CBuffDrainBonusMana`
имеют vtable slot 3 `0x6F0A0820`
([class `CBuffDrainBonusLife`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F898534.json),
[body](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F0A0820.json)).
При `0xD01E6` он проверяет флаг `+0x20 & 0x400` и два
range-предиката `0x6F022060` относительно
`dword_6FAAE4D0`; только затем вызывает ability removal
`0x6F079CC0`. Это условная реакция конкретного buff, **не**
общая процедура смерти юнита. Наличие класса с таким handler
ещё не доказывает его регистрацию у произвольного unit.

## Открытая граница для fog revoke

В проверенных прямых телах до двух списков получателей нет вызова
`CUnit::Deactivate` `0x6F282920`, `RefreshUnitFog` `0x6F40A650`,
unit fog writer `0x6F409E00` или полного rebuild `0x6F40A8F0`.
S10 [объясняет](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Jass/jassnatives_life.cpp),
что последствия смерти идут от life-пула, но не приводит здесь
проверенный путь до удаления/обзора. Возможные side effects
`0x6F4A90B0`, конкретный состав обоих observer-списков,
порог смерти, её таймер и связь с `Deactivate` требуют отдельной проверки.
Отсутствие прямого вызова на просмотренном участке не доказывает
отсутствия отзыва fog при смерти.

Следующий адресный эксперимент: на unit с единственным источником
обзора снять старое и новое значение life, диапазон `+0x80/+0x84`,
флаг `+0x4F`, содержимое range list `+0x64` и observer registrations
`0xD01E6`, затем зафиксировать их concrete vtable targets и момент
`0x6F282920`/`0x6F40A650`/`0x6F40A8F0`. Проверить отдельно
`KillUnit` при живом и уже нулевом life и после decay-таймера.

# FND-0028 — создание юнита пишет fog двумя путями, удаление старого радиуса остаётся открытым

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, creation, movement, fog, presentation, invalidation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [создание `0x6F29F990`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F29F990.json), [CUnit slot 107 `0x6F2A0E30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A0E30.json), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [RefreshUnitFog `0x6F40A650`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A650.json), [другой unit path `0x6F05AAD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F05AAD0.json), [ability notification `0x6F0D6380`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F0D6380.json), [Reposition `0x6F2A5D50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A5D50.json), [S10 caller](../../../../src/Widget/playertableunitfogrefresh.cpp) |

## Вывод

`CreateUnitAtPosition` (`0x6F29F990`) на `0x6F29FBBC`–`0x6F29FBE0`
вызывает виртуальный слот юнита `+0x1AC` (CUnit slot `107`). В его
IDA-теле `0x6F2A0E30` после настройки объекта, `0x6F284DA0` и
получения fog-таблицы из глобальной таблицы игроков `+0x34` следует
`RefreshUnitFog(unit, 0xFFFF)` на `0x6F2A1CFA`–`0x6F2A1D07`.
Sentinel `0xFFFF` заставляет `RefreshUnitFog` вычислить маску из
отношений игрока и запросов юнита. При допуске флагами юнита эта
функция сначала обращается к локальному world frame, потом вызывает
`ApplyUnitFogRadius(unit, mask, 0, 0)` —
[FND-0008](FND-0008-fog-ui-dependency.md).

После виртуального вызова тот же `CreateUnitAtPosition` может сделать
**дополнительный** прямой вызов writer. Он берёт маску из
`VisibilityMaskWord_6F40B1E0`, проверяет ненулевую маску и
`QueryVisibleImpl(1) != 0`, затем вызывает
`ApplyUnitFogRadius(unit, mask, 1, 1)` на `0x6F29FBEC`–`0x6F29FC1D`.
Ненулевой третий аргумент принудительно выбирает режим `4` внутри
writer, а четвёртый в его поклеточном пути подавляет установку бита
`+0x30`; на прямом row writer пути четвёртый аргумент не влияет —
[FND-0019](FND-0019-unit-fog-writer-modes.md). Поэтому эти два вызова
нельзя объединять в одно «нанесение нового радиуса» без проверки
фактических ветвей.

Другой путь `0x6F05AAD0` после изменения состояния юнита тоже
вычисляет `VisibilityMaskWord` и при ненулевой маске и положительном
`QueryVisibleImpl(1)` вызывает `ApplyUnitFogRadius(..., 1, 1)`.
`SAbilityHandleCarrier::NotifyOwnerOfGrant` (`0x6F0D6380`) имеет ещё
один вход: только при ненулевом аргументе и пройденных проверках
способности вызывает `ApplyUnitFogRadius(..., 0, 0)`.
Полный проход `0x6F40A8F0` наносит допущенные юниты через
`ApplyUnitFogRadius(..., 0, 0)` отдельно от этих единичных вызовов —
[FND-0026](FND-0026-unit-fog-rebuild-trigger.md).

Обычный `CUnit::Reposition` `0x6F2A5D50` меняет положение через
снятие/восстановление footprint и вызов базового `CWidget::MoveTo`, но
его собственное IDA-тело не содержит прямых вызовов `0x6F40A650`,
`0x6F409E00` или полного rebuild `0x6F40A8F0`. Это ограниченная
**отрицательная находка о прямых вызовах**, не доказательство отсутствия
fog-эффекта через уведомления, виртуальные вызовы или таймер.

## Основание и контроли

Положительный статический контроль: пара `vtable[0x1AC]` →
`RefreshUnitFog(..., 0xFFFF)` подтверждается и CUnit vtable-записью,
и двумя IDA-телами. Прямой второй вызов в `CreateUnitAtPosition`
расположен после первого и использует именно `1,1`, а не `0,0`.
В S11 среди текстовых прямых callers `0x6F409E00` видны
`0x6F29F990`, `0x6F05AAD0`, `0x6F0D6380`, `0x6F40A650` и
`0x6F40A8F0`; их условия различаются.

Отрицательные контроли: нулевая маска либо ложный
`QueryVisibleImpl(1)` пропускают второй writer в `CreateUnitAtPosition`;
нулевой аргумент `0x6F0D6380` пропускает его ветвь. Сам
`RefreshUnitFog` выходит до world frame и writer при нулевом юните
или условии флагов `(unit+0x5C & 0x100) && !(unit+0x5C & 0x20)`.
В `Reposition` проверка прямых call instructions такого writer не нашла.

## Ограничения и значение для проекта

Это просмотр закреплённого исходного тела и S10 C++, без исполнения
Game.dll; `0x6F2A0E30` и `CreateUnitAtPosition` остаются `THUNK`,
`RefreshUnitFog` и `Reposition` — `DIFFERS` в S11. Не установлен
маршрут удаления прежней области при сдвиге/уходе юнита, даже если
новая область уже нарисована. `Reposition` содержит косвенные вызовы
и уведомления, а текстовый поиск прямых адресов не даёт полного
call graph. UI-вызов `RefreshUnitFog` и последующая передача данных
после полного rebuild не устанавливают окончательный момент обновления
экрана. Следующий оракул: юнит одного игрока переместить из области A
в непересекающуюся B, затем удалить; до/после каждого события и timer
event `0x80269` сравнить обе плоскости A/B, запросы точки и юнита,
локальный UI и наблюдаемое раскрытие второму игроку. Контроль: второй
независимый источник обзора в A, чтобы не приписать его отзыв первому.

# FND-0037 — relation teardown может вызвать `CUnit::Deactivate`, источник смерти не установлен

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, lifecycle, relation, deactivation, observer, fog, RemoveUnit |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен; адреса — координаты S11. Сопоставлен закреплённый S10 C++; runtime-путь смерти/RemoveUnit не воспроизводился. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [relation slot 4 `0x6F4A5A70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A5A70.json), [code 1 notifier `0x6F4A4DF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A4DF0.json), [callback host `0x6F4815A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4815A0.json), [callback install `0x6F46DA10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F46DA10.json), [code router `0x6F46C3A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F46C3A0.json), [delegate teardown `0x6F46C320`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F46C320.json), [delegate bind `0x6F47FCE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F47FCE0.json), [observer release `0x6F62AA40`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62AA40.json), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [S10 relation cascade](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Agent/agentbaseabsrelatedcascade.cpp), [S10 notifier](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Agent/agentbaseabsnotify.cpp), [S11 RemoveUnit registration](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/pipeline/docs/targets/jass-natives-registration-table.md) |

## Вывод

S11 содержит **условный общий вход в deactivation**, отличный от
внутреннего тела `CUnit::Deactivate` в [FND-0034](FND-0034-unit-deactivation-fog-revocation-boundary.md).
`NIpse::CRlAgent` vtable slot 4 (`+0x10`, `0x6F4A5A70`) ставит бит
`0x01000000` в свои flags `+0x4C`, затем вызывает
`0x6F4A4DF0(this, 0)`. Эта функция строит context из relation
`+0x14/+0x18`, передаёт code `1` в `0x6F4815A0` на
`g_pTimeSync` (`dword_6FAB73D8`). При установленном callback
`g_pTimeSync+0x254` host передаёт `(code, context, subjectId)`.
`0x6F46DA10` устанавливает в этот slot `0x6F46C3A0`.

`0x6F46C3A0` при code `1` передаёт context в `0x6F46C320`.
Тот разрешает пару handle/typeTag из context через `0x6F03FA30`,
берёт **delegate из `resolved+0x54`**, проверяет его на null и
вызывает у **delegate**, а не у relation или handle object,
виртуальный slot `+0x34` (`0x6F46C359–0x6F46C35E`).
`0x6F47FCE0` отдельно подтверждает, что `+0x54` — refcounted
delegate agent. Если его runtime vtable — CUnit
`0x6F931934`, slot `+0x34` равен `0x6F282920`, то есть
`CUnit::Deactivate`. Наличие CUnit в конкретном relation и
исполнение такого teardown после смерти или `RemoveUnit`
этими телами **не доказаны**.

Перед slot `+0x34` callback формирует событие с ID `0x40190064`
и вызывает у delegate slot `+0x10`. Для CUnit он равен
`0x6F629A90`, который передаёт сообщение в observer slot
`+0x14`; состав получателей не восстановлен. После slot
`+0x34` callback вызывает `0x6F471BB0` (observer по адресу
`delegate+0x14`) и `0x6F62AA40` (observer по адресу delegate),
затем сбрасывает связанные handle-поля через `0x6F471F90`
и отпускает ссылку на delegate. `0x6F62AA40` обходит/освобождает
event-регистрации и ресурс таблицы, если он есть; это cleanup
observer, а не запись в fog plane `+0x30`.

## Основание и контроли

Положительный статический контроль: `0x6F4A5A70` безусловно
вызывает code 1 notifier до detach/cascade relation; notifier
передаёт именно `1` в `0x6F4815A0`; `0x6F46C3A0` только в
ветви code 1 вызывает `0x6F46C320`. При валидном resolved
handle и ненулевом `resolved+0x54` последняя функция делает
virtual `+0x34`. Таблица CUnit фиксирует target
`0x6F282920` для этого slot. Далее `0x6F4A5A70` выполняет
detach relation lists `0x6F4A4C40`, cascade `0x6F4A57F0`
и сбрасывает свои `+0x50/+0x54`.

Отрицательные контроли: нулевой callback в
`g_pTimeSync+0x254` завершает `0x6F4815A0` без вызова
`0x6F46C3A0`; code `2` и `3` в `0x6F46C3A0` проходят
другие ветви и не вызывают `0x6F46C320`. Неразрешённый
context handle или нулевой `resolved+0x54` пропускает
delegate slot `+0x34`. Даже на положительном пути конкретный
target зависит от runtime vtable delegate; vtable relation
сам по себе не доказывает тип CUnit.

## Граница связи со смертью, удалением и fog

`KillUnit` в [FND-0036](FND-0036-killunit-life-notification-death-boundary.md)
задаёт life=0 и уведомляет два набора слушателей. В прочитанных
телах от него до `NIpse::CRlAgent` slot 4, code 1 notifier или
`0x6F46C320` переход не установлен. JASS `RemoveUnit` указан
в S11 registration table как `loc_6F3C8060`, но отдельного
`agent_worktrees/funcs/0x6F3C8060.json` в закреплённой ревизии
нет. Следовательно, **ни `KillUnit`→этот relation teardown,
ни `RemoveUnit`→этот relation teardown не подтверждены**.
Это предел доступного корпуса, не отрицательный результат о
поведении Game.dll.

В `0x6F46C320` и проверенном продолжении нет прямого вызова
`RefreshUnitFog` `0x6F40A650`, writer `0x6F409E00` или
full rebuild `0x6F40A8F0`. До вызова `Deactivate` остаётся
неразрешённый observer delivery события `0x40190064`;
после него — виртуальное освобождение refcounted delegate.
Ни один из этих вызовов нельзя автоматически считать fog revoke.
Даже доказанный spatial teardown внутри `Deactivate` снимает
footprint, но не устанавливает, когда очистится fog `+0x30`.

Следующая проверка: получить тело `RemoveUnit` `loc_6F3C8060`
с определением функции в scratch IDA, а для сценария смерти
зафиксировать concrete получателей life-событий из FND-0036.
На живом unit и после `KillUnit`/`RemoveUnit` проследить,
вызывается ли `CRlAgent` slot 4 и какой delegate лежит
в `resolved+0x54`; затем наблюдать старую fog-клетку
`+0x30` и gate полного rebuild.

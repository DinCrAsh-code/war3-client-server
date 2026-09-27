# FND-0064 — basic order проходит два разных счётчика до запуска задачи

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, queue, task, unit, lifecycle, stop |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; один вход из inbound `0xA0010` в generic COrder. Fingerprint DLL не установлен, Game.dll не запускалась. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10 unit order model](../../../../src/Unit/unitorder.h), [FND-0062](FND-0062-inbound-selection-basic-order-application.md) |

## Два разных поля, решающих разные вопросы

[FND-0062](FND-0062-inbound-selection-basic-order-application.md) доводит
команду `0xA0010` до `0x6F2CA800`, который в одной из ветвей передаёт
generic order в [`CUnit::SubmitOrder` `0x6F2A4AB0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A4AB0.json).
Первый guard `unit+0x5C & 0x100` полностью пропускает order.
Далее `unit+0x198 > 0` выбирает отдельную ветвь:
при флаге replace вызываются [`0x6F2831A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2831A0.json)
и [`StartOrderNow` `0x6F2A0510`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A0510.json),
иначе [`AppendOrder` `0x6F2A0650`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A0650.json).
При `+0x198 <= 0` перед выбором replace/append проверяется
[`IsOrderAlreadyActive` `0x6F2832E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2832E0.json):
он требует разрешённый head `unit+0x19C`, подходящую ability из
`0x6F279A90` и её vtable `+0x22C` ответ. Положительный ответ
пропускает постановку дублирующего order; replace при этом может
сначала отменить текущий. Если order ещё не активен, replace вызывает
[`0x6F2A4A10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A4A10.json)
и `StartOrderNow`; ветвь append выполняет подготовку, проверку текущего
head и `AppendOrder`. Эти направления видны в одном body; смысл флагов
нельзя назначать без producer и конкретного order ID.

И `StartOrderNow`, и `AppendOrder` **отдельно** ограничивают
`unit+0x1B4` значением `0x1F4` (`500`): при `>500` выходят до
вставки, иначе связывают head/tail handles `+0x19C/+0x1A8` и
увеличивают `+0x1B4` на единицу. [`0x6F2831A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2831A0.json)
может очистить цепь и сбросить `+0x1B4`; `0x6F2A4A10` также
сбрасывает это поле после подготовки. **`+0x198` и `+0x1B4` —
разные смещения и разные gates.** Комментарий S10 в
[`unitorder.h`](../../../../src/Unit/unitorder.h) называет `+0x198`
числом queued orders, тогда как [`unit.h`](../../../../src/Unit/unit.h)
прямо предупреждает, что оно отлично от длины очереди `+0x1B4`.
Статически доказано различие полей и ветвлений; точный смысл `+0x198`
оставлен открытым. Эту терминологию S10 нельзя считать oracle.

## Переход от очереди к задаче

Если task handle `unit+0x174` отсутствует либо не разрешается,
`StartOrderNow` вызывает [`BeginOrder` `0x6F29DFF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F29DFF0.json)
с фиксированным флагом `1`; `AppendOrder` вызывает его с флагом
caller. Когда task уже существует, `StartOrderNow` публикует на unit
event `0xD02A5` о принятом order вместо немедленного `BeginOrder`.
Таким образом, положительная вставка не всегда означает немедленное
исполнение.

`BeginOrder` очищает bit `0x80` в order `+0x20`, затем по agile type ID
выбирает starter. Generic [`MakeOrderAgent` `0x6F294A40`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F294A40.json)
из basic `0xA0010` получает tag `0x2B6F7264` через `0x6F2712B0`,
поэтому показанный путь ведёт в [`0x6F299A00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F299A00.json).
Этот starter перед построением task проверяет два observer keys:
`0x8024B` на unit и `0x80226` на связанном player object. Если оба
запроса не дали записи, он выходит. На показанной положительной ветви
он создаёт task/order objects, связывает их с unit, регистрирует task
через `0x6F430C80` в world registry и условно уведомляет observers.
Сам факт регистрации task ещё не показывает эффект stop/move/attack.

## Контроли и предел результата

Положительный статический контроль: при `+0x5C & 0x100 == 0`,
`+0x1B4 <= 500`, допустимом order и пустом task handle выполняется
вставка с увеличением `+0x1B4` и вход в `BeginOrder`; generic type
попадает в `0x6F299A00`. Отрицательные ветви: dying bit или
переполненная очередь пропускают вставку; активный order может
пропустить повтор; существующая task ведёт к событию/ожиданию;
отсутствие обоих observer keys прерывает показанное создание task.
Не доказано, что эти gates всегда проходят для конкретной карты,
что side effects task выполняются в том же tick или что order от
сетевого sender аутентифицирован. Следующий опыт: Stop и point/target
order при пустой и непустой task, `+0x198`/`+0x1B4` независимо,
дублирующий order и dying unit; измерить очередь, task и видимый эффект.

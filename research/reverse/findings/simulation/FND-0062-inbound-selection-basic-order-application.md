# FND-0062 — входной basic order применяют к отфильтрованным юнитам из selection

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | selection, basic-order, observer, sender, unit, authority |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; IDA `raw_asm` для одной пары команд `0xA0016`/`0xA0010`. Fingerprint DLL не установлен; Game.dll не запускалась. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0057](FND-0057-selection-before-basic-order-queue.md) |

## Маршрут подписки и выбора

Регистратор [`0x6F2CF910`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CF910.json)
передаёт callback `0x6F2CEC70` для byte `0x16`, а callback `0x6F2CCE10`
для byte `0x10`. Обёртка [`0x6F549660`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F549660.json)
превращает byte в key `0xA0000 | byte`; [`0x6F549620`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F549620.json)
выбирает запись через TLS slot 13 и индекс с шагом `0x304`.
[`0x6F548F70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F548F70.json)
создаёт observer для key, записывает callback в `+0x14` либо обновляет
userdata существующего. Это конкретный получатель observer fire из
[FND-0057](FND-0057-selection-before-basic-order-queue.md); создание записи
не устанавливает её сетевую provenance.

[`0x6F2CEC70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CEC70.json)
берёт sender byte из command `+0x15` и по нему получает world player entry
`+0x34`. Count в command `+0x20` больше `12` приводит к выходу **до**
обработки пар. При допустимом count callback перебирает пары identity из
`+0x24`, разрешает их через [`0x6F03FA30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F03FA30.json),
проверяет tag `0x2B61676C` и затем применяет режим command `+0x18` к
selection данного player entry. Для одного режима непригодный объект
попадает в [`0x6F424CE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F424CE0.json)
на удаление из выбора; другая ветвь собирает допустимые объекты и
вызывает selection helpers `0x6F426090`, `0x6F425D10`/`0x6F425E80`.
Значение режимов и итоговое множество selection по всем ветвям пока не
проверены. Cap `12` находится здесь, хотя Attach parser из FND-0057
сам по себе его не показывает.

## Маршрут basic order в состояние CUnit

[`0x6F2CCE10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CCE10.json)
тоже берёт `+0x15` и world player entry `+0x34`. Целевую пару
`+0x20/+0x24` он разрешает через `0x6F03FA30`; отсутствующая запись
или несовпавший tag даёт пустую target reference, **но не прекращает**
обход selection. [`0x6F421AF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F421AF0.json)
перебирает selection entry, повторно разрешает identity, требует тот же
tag и нулевое `entry+0x20` перед callback. Предварительный callback
[`0x6F2C91A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C91A0.json)
отсеивает объекты без бита `0x40000000` в `unit+0x20`.

Основной callback [`0x6F2C91C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C91C0.json)
добавляет unit в рабочий список лишь после двух ворот: `0x6F285D10`
должен вернуть неноль для sender byte, а `0x6F2844D0` — ноль для
order ID и параметров команды. Первый helper читает owner `unit+0x58`,
world relations `0x6F3A37F0`/`0x6F3A36D0` и flag `unit+0x5C & 0x2000000`;
он не является одной проверкой «sender == owner». Чтение исходного ASM
подтверждено независимым разбором C++ тела в
[claudecraft PR #54](https://github.com/FilippTheBestDev/claudecraft/pull/54),
коммит `cba1aba72`: helper сначала ищет два ability tag через
`0x6F0787D0`, затем допускает совпадение sender с `unit+0x58`,
положительный directed visible bit от owner к sender, наличие первого
ability при сброшенном `unit+0x5C & 0x02000000` либо второго ability
при том же сброшенном бите и положительном directed enemy bit.
`0x6F3A37F0` и `0x6F3A36D0` читают соответствующие relation masks,
используя младшие пять бит sender как индекс бита. Это локальная
пригодность команды после разбора, а не проверка источника сетевого пакета.
Второй helper имеет
отдельные cases `0xD0005` и `0xD0004`, затем цепочку state/ability
проверок. В частности, `0xD0004` при dying-бите `unit+0x5C & 0x100`
возвращает неноль и исключает unit из рабочего списка.

После сортировки рабочего списка callback `0x6F2CCE10` для каждого entry
создаёт order object через [`0x6F294A40`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F294A40.json)
из command `+0x1C` и sender, затем вызывает
[`0x6F2CA800`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CA800.json).
Бит `command+0x18 & 4` останавливает цикл после первого entry.
`0x6F2CA800` записывает в `unit+0x240/+0x244` пару из resolved
order target либо `-1/-1`, затем выбирает один из
[`0x6F2A0770`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A0770.json),
[`0x6F2A0650`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A0650.json),
[`0x6F2A4AB0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A4AB0.json)
или [`0x6F2A0510`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A0510.json).
Последние два helpers имеют собственный выход при dying-бите `0x100`.
Итого это доказанный вход в CUnit order state, но успешное движение,
бой или изменение мира для конкретного приказа здесь не показаны.

## Контроли и следующий рубеж

Положительный статический контроль: callbacks зарегистрированы ровно на
`0xA0016` и `0xA0010`; при допущенном unit рабочий список ведёт к
`0x6F2CA800` и записи `unit+0x240/+0x244`. Отрицательные ветви:
selection count `>12` прерывает modify; неподходящая identity или tag
не добавляет объект; отсутствие бита `0x40000000` и ненулевой ответ
`0x6F2844D0` отсекают unit; dying-флаг запрещает показанные order paths.
Неверная target identity оставляет пустую target reference, поэтому
её нельзя считать универсальным отказом всей команде.

Вывод не устанавливает, что command sender byte принадлежит сетевому
peer, что selection была получена из того же turn, или что каждая
ветвь применяет order в точности один раз. Следующий опыт: один и два
юнита с разными owner/alliance, устаревшая identity цели, выбор `>12`,
перестановка selection/order и dying unit; сверить список выбранных,
фактический order state и эффекты на точной DLL для двух игроков.

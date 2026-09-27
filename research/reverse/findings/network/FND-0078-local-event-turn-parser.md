# FND-0078 — локальное событие `0x1F` достигает turn parser

| Поле | Значение |
|---|---|
| Subsystem | network |
| Tags | order, event, queue, turn-parser, sender-byte, peer-boundary |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; потребитель локальной очереди `0x6F5530D0` → event dispatch `0x6F551D80` → `0x6F5516E0` → turn parser `0x6F550730`. Установлен маршрут при прохождении его ветвей, но не доставка peer, аутентификация отправителя и фактический эффект команды. Fingerprint DLL неизвестен. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0076](FND-0076-local-net-event-enqueue-boundary.md); [FND-0066 в PR #3](https://github.com/DinCrAsh-code/war3-client-server/blob/83bc16bee5252f67c8ae22d913777d30407f2f10/research/reverse/findings/network/FND-0066-online-turn-store-flush-boundary.md) |

## Потребление события

[FND-0076](FND-0076-local-net-event-enqueue-boundary.md) устанавливает
локальный enqueue события `0x1F` с указателем payload в `event+0x08`,
длиной в `+0x0C`, id в `+0x14` и флагом в `+0x15`.
[`0x6F5530D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5530D0.json)
берёт голову контейнера через `netData+0xF40`, держит текущий event в
`+0x1B40`, удаляет обработанный event из списка `+0xF38` вызовом
`0x6F549180`, передаёт его в
[`0x6F551D80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F551D80.json)
и затем вызывает `0x6F54B810`. Если `+0xF40 <= 0`, этого маршрута нет.
Если включена таблица `netData+0x173C` и ячейка для id пуста, event
пропускается до dispatch; точный смысл таблицы здесь не установлен.

`0x6F5530D0` может вычислить payload hash для id `0x1E`/`0x1F` через
`0x6F39E5C0`, когда выполняются дополнительные state/flag условия.
Ветви, обходящие hash, всё равно сходятся на вызове `0x6F551D80`.
Этот hash не является установленной проверкой сетевого отправителя.

`0x6F551D80` создаёт reader из `event+0x08/+0x0C` и читает id из
`event+0x14`. При достижении dispatch case `0x1E` вызывает
[`0x6F5516E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5516E0.json)
с последним аргументом `0`, case `0x1F` — с `1`. Передаваемый slot
равен нулю при нулевом `event+0x15`, иначе берётся из `netData+0x610`.
В обоих случаях `0x6F5516E0` вызывает
[`0x6F550730`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F550730.json)
с reader и slot. Полученное значение меняет `slot*0x304+0x284` и
`netData+0x225C`; вариант `0x1F` дополнительно имеет ветвь изменения
`+0x2258` и `slot*0x304+0x2E0`. Семантика счётчиков пока не доказана.

`0x6F550730` сначала читает 16-битное слово через `0x6F6516C0` и
проверяет курсор reader. Для slot `0` при нулевом `0x6FAB65F4` оно
возвращает `0` до разбора записей. Дальше `0x6F652300` читает запись,
а `0x6F54E7E0` ищет её первый byte в локальной таблице
`slot*0x304+0x290`: найденный элемент даёт byte `+0x36`, отсутствие —
`0xFF`. Затем идут ветви обработки command subtype, включая вызов
`0x6F545270` для части значений. Это локальный lookup; связь
таблицы с authenticated peer по показанной цепи не установлена. Локальный
жизненный цикл key → player byte ограничен в
[FND-0079](FND-0079-sender-key-local-binding.md).

## Контроли и границы

Положительный статический контроль: при непустой очереди, разрешённом
id `0x1F` и достижении case `31` reader передаётся из `0x6F551D80`
через `0x6F5516E0` в `0x6F550730` с флагом `1`. Case `30` для `0x1E`
проходит тот же маршрут с флагом `0`. Отрицательные ветви: пустая очередь
или отключённая ячейка таблицы id не вызывают этот dispatch; slot `0`
с нулевым `0x6FAB65F4` не проходит к чтению command record.

Отвергнута гипотеза, что вычисление hash в `0x6F5530D0` само
удостоверяет отправителя: в том же теле есть маршруты к dispatch без
hash, а `0x6F54E7E0` лишь ищет byte в локальной таблице. Здесь не
разобраны все session gates большого `0x6F551D80`, jump table всего
`0x6F550730`, происхождение `event+0x15`, lifetime payload, доставка
другому участнику и эффект приказа в мире. Путь normal online flush
из [FND-0066](https://github.com/DinCrAsh-code/war3-client-server/blob/83bc16bee5252f67c8ae22d913777d30407f2f10/research/reverse/findings/network/FND-0066-online-turn-store-flush-boundary.md)
остаётся отдельным. Следующий опыт — трасса двух игроков с неверным
sender byte и чужой unit identity как отрицательными контролями.

# FND-0068 — point, target и fogged order расходятся по payload и обработчикам

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, point, target, fogged, selection, identity, disclosure, ranking |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; входящие `0xA0011`–`0xA0015`, от Attach до вызова order на выбранных юнитах. Fingerprint DLL неизвестен, исполнение не повторялось. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10 unit order model](../../../../src/Net/netcommand_unitorder.h), [S10 dispatcher](../../../../src/Net/netcommand_dispatch.cpp), [FND-0062](FND-0062-inbound-selection-basic-order-application.md) |

## Граница семейства

[`0x6F2CF910`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CF910.json)
регистрирует пять отдельных callback для ключей `0x11`–`0x15`.
Сетевой dispatcher S10 направляет соответствующие варианты в builders;
проверенные S11 Attach hooks сначала читают общую основу, затем поля
своего варианта. Адреса в таблице — координаты записей S11, не runtime API.

| Wire ID | Дополнение к basic `+0x18..+0x24` | Attach | Callback | Конструктор order |
|---|---|---|---|---|
| `0xA0011` point | два `CFloat` `+0x28/+0x2C` | [`0x6F553F60`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553F60.json) | [`0x6F2CD020`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CD020.json) | `0x6F294B30` |
| `0xA0012` target image | point и пара `+0x30/+0x34` | [`0x6F553FC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553FC0.json) | [`0x6F2CD4E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CD4E0.json) | `0x6F294D40` |
| `0xA0013` target image 2 | target image и пара `+0x38/+0x3C` | [`0x6F554020`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554020.json) | [`0x6F2CDA40`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CDA40.json) | `0x6F294E40` |
| `0xA0014` fogged | point и три dword `+0x30..+0x38`, byte `+0x3C`, два `CFloat` `+0x40/+0x44` | [`0x6F5540B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5540B0.json) | [`0x6F2CDFF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CDFF0.json) | `0x6F294F50` |
| `0xA0015` fogged 2 | fogged и два dword `+0x48/+0x4C` | [`0x6F554130`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554130.json) | [`0x6F2CE550`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CE550.json) | `0x6F295050` |

Для `0xA0012/13` пары `+0x30/+0x34` и `+0x38/+0x3C`
передаются в `0x6F03FA30` как ссылки на объекты, а найденный объект
проверяется по tag `0x2B61676C`. В `0xA0014/15` эти же смещения
**не проходят** такой lookup: callback передаёт `+0x30..+0x44` в
[`0x6F255180`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F255180.json),
который копирует шесть полей в составной descriptor. Fogged-ветвь
наследует point, а не target image. Она всё ещё разрешает общую пару
`+0x20/+0x24`; называть весь payload «координатой вместо handle»
было бы неверно. Название `fogged` из S10 не доказывает политику тумана.

Все пять callback берут слот из `+0x15`, проходят выборку его юнитов,
строят отдельный тип order и вызывают `0x6F2CA800` на допущенных
юнитах. Например, point строится через `0x6F294B30`, fogged — через
`0x6F294F50`; обе ветви передают в `0x6F2CA800` word флагов `+0x18`.
Внутри выборки fogged `0x6F2CB4F0/0x6F2CB710` вызывают
`0x6F285D10` для отношения юнита к слоту `+0x15`, затем
`0x6F2C9010` и условно `0x6F279560`; тип order для записи в рабочий
список различается (`6`/`7`). Это **фильтр применения**, не доказанная
аутентификация отправителя или правило выдачи скрытой цели.

Для point/fogged candidate row дополнительная сверка S11 и
[C++ PR #54, коммит `fc651d6ca`](https://github.com/FilippTheBestDev/claudecraft/pull/54)
уточнила два поля. [`0x6F284350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F284350.json)
берёт беззнаковый минимум ответов ability slot `+0x220` для order ID и
точки; если все ответы равны `-1`, вычисляет масштабированный квадрат
расстояния от позиции юнита до точки через `CFloat`, с нулевым Z.
Результат идёт в row `+0x0C` и участвует в ранжировании кандидатов.
[`0x6F421DE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F421DE0.json)
заполняет row `+0x14`: null player list `+0x1C0` даёт `0`, иначе
вызывает уже существующий `SUnitMembershipList::Contains(unit)`.
Это локальный выбор кандидата, не server admission.

## Контроли и открытая граница

Положительный статический контроль: при разрешённой общей паре
`+0x20/+0x24`, прохождении фильтров order и выбранном допущенном
юните показанная point/fogged ветвь достигает своего конструктора и
`0x6F2CA800`. Отрицательный контроль:
неразрешённая или чужого tag пара объекта в target image не даёт
объектной ссылки; `0x6F285D10`-ветвь меняет обработку выбранного юнита.
Сам callback может продолжить работу с пустой ссылкой, поэтому из этого
**не следует** безусловный отказ всей команды. Если ability не даёт
score, `0x6F284350` не отвергает кандидата, а переходит к расстоянию;
если membership list отсутствует, `0x6F421DE0` возвращает ноль.
Не показаны producer всех пяти вариантов, origin/authentication слота,
фактическая
смена task, эффект move/attack или безопасность `fogged` payload при
выдаче клиенту.

Следующая связанная проверка: найти исходящие producer `0xA0011`–`15`,
сопоставить descriptor `+0x30..+0x44` с получателем и измерить на двух
игроках point/target/fogged при видимой, скрытой и устаревшей цели.
Отдельно проверить, что запрет выбора чужого юнита и сокрытие цели
сохраняют обычные карты и команды в тумане.

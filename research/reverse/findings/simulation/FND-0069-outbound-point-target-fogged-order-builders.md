# FND-0069 — семь исходящих builders ставят выбор перед point/target/fogged order

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, point, target, fogged, flags, selection, turn-store |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; семь wrappers и builders исходящего семейства `0xA0011`–`0xA0015` до записи подготовленного payload в turn store. Fingerprint DLL неизвестен, Game.dll не запускалась. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0057](FND-0057-selection-before-basic-order-queue.md), [FND-0068](FND-0068-point-target-fogged-order-family.md) |

## Исходящая сторона семейства

По S11 семь wrappers переставляют биты аргументов и передают их builders.
Два варианта `0xA0012` и два варианта `0xA0014` различаются тем, берут
ли цель/descriptor из объекта или из предоставленных полей. Все семь
builders запрашивают локального игрока через `dword_6FAB65F4+0x28`,
вызывают [`0x6F425490`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F425490.json)
с `0` **до** сериализации приказа, затем свой serializer. Для basic
order аналогичный порядок показан в [FND-0057](FND-0057-selection-before-basic-order-queue.md).

| ID | Wrapper → builder | Источник дополнительных полей | Serializer |
|---|---|---|---|
| `0xA0011` | [`0x6F339CC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F339CC0.json) → [`0x6F2CBA00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CBA00.json) | point `CFloat` pair | `0x6F2C9890` |
| `0xA0012` | [`0x6F339D50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F339D50.json) → [`0x6F2CBAD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CBAD0.json) | объектная identity и point из virtual `+0xB8`; null даёт `-1` identity и `g_CFloatZero` | `0x6F2C9950` |
| `0xA0012` | [`0x6F339DD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F339DD0.json) → [`0x6F2CBC10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CBC10.json) | point pair и optional object identity; null даёт `-1` identity | `0x6F2C9950` |
| `0xA0013` | [`0x6F339E60`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F339E60.json) → [`0x6F2CBD00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CBD00.json) | point pair и две optional object identities; null каждой даёт `-1` | `0x6F2C9A10` |
| `0xA0014` | [`0x6F339F00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F339F00.json) → [`0x6F2CBE20`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CBE20.json) | указатель на шесть полей descriptor, без null guard на этом входе | `0x6F2C9AD0` |
| `0xA0014` | [`0x6F339F80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F339F80.json) → [`0x6F2CBF10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CBF10.json) | optional descriptor; null даёт zero полям `+0x30..+0x38`, `0xFF` byte и `g_CFloatZero` | `0x6F2C9AD0` |
| `0xA0015` | [`0x6F33A010`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F33A010.json) → [`0x6F2CC050`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CC050.json) | optional descriptor как выше и optional вторая identity; null identity даёт `-1` | `0x6F2C9B90` |

Wrappers собирают word `+0x18` из двух входных flag слов `A` и `B`:
биты выхода `0←A0`, `1←A1`, `2←B20`, `3←A3`, `4←A4`, `5←B21`,
`6←A2`, `8←B11`; другие биты этими wrappers не устанавливаются.
Это перестановка, а не передача исходного flag word без изменения.
Все семь S11-тел содержат тот же порядок сдвигов/масок; различны
положение аргумента на стеке и сигнатура вызова своего builder.

Serializer каждого варианта строит buffer, вызывает variant `0x6F553F30`
... `0x6F554100`, затем [`0x6F54D970`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54D970.json).
У этой записи в turn store есть собственные gates текущего record/state/size,
так что вызов builder ещё не гарантирует появления команды у peer.
На входе [FND-0068](FND-0068-point-target-fogged-order-family.md)
показывает соответствующие Attach и callback, но statically не связывает
отправителя с аутентифицированным сетевым участником.

## Контроли и пределы

Положительный статический контроль: каждый показанный builder вызывает
`0x6F425490` до своего serializer; serializer вызывает `0x6F54D970`
после записи variant payload. Отрицательные ветви: null target identity
даёт sentinel `-1`; null fogged descriptor в optional overload даёт
zero/`0xFF`, тогда как прямой overload разыменовывает его без guard.
Это не доказывает, что null доходит из UI, что запись проходит gates
turn store или что receiver правильно обрабатывает устаревшую identity.
Не установлены вызывающие UI/JASS операции всех wrappers, выбор между
direct/fogged overload, смысл всех битов и игровые эффекты приказа.

Следующая проверка — caller каждого wrapper и условие выбора варианта,
затем двухигроковый опыт с видимой, скрытой и устаревшей целью и
проверкой порядка selection/order и результата в мире.

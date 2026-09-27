# FND-0073 — пять вариантов приказа записывают поля через общую цепь writer

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, point, target, fogged, serialization, wire, disclosure |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; пять исходящих сериализаторов `0x6F2C9890`–`0x6F2C9B90` и шесть writer-функций `0x6F553EB0`–`0x6F554100`. Это порядок вызовов и передаваемых полей, не доказательство байтового формата, записи у peer или проверки прав. Fingerprint DLL и исполнение неизвестны. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0069](FND-0069-outbound-point-target-fogged-order-builders.md), [FND-0071](FND-0071-fogged-target-descriptor-provenance.md) |

## Общий каркас

Пять исходящих функций [`0x6F2C9890`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9890.json),
[`0x6F2C9950`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9950.json),
[`0x6F2C9A10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9A10.json),
[`0x6F2C9AD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9AD0.json)
и [`0x6F2C9B90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9B90.json)
имеют один каркас: создают локальный буфер через `0x6F2C9290`,
передают байт из объекта `+0x14` в `0x6F4C2160`, вызывают свой writer,
затем `0x6F54D970` и освобождают буфер через `0x6F2C95B0`.
Входной `edx` передаётся в `0x6F54D970`. В [FND-0069](FND-0069-outbound-point-target-fogged-order-builders.md)
показано, что builders используют этот вход с `0`; смысл всех значений
и итоговое прохождение turn-store gates здесь не установлены.

## Накопительное добавление полей

Каждый writer вызывает предка, затем дописывает собственные поля.
Таблица описывает *порядок вызовов* `WriteWord`/`WriteDword`/`WriteByte`
и writer для `CFloat`; внутреннее представление `CFloat` и обрамление
буфера этим анализом не доказаны.

| Вариант | Writer в S11 | Передаваемые поля объекта после байта `+0x14` |
|---|---|---|
| Общая основа | [`0x6F553EB0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553EB0.json) | word `+0x18`; dword `+0x1C`, `+0x20`, `+0x24` |
| `0xA0011` point | [`0x6F553F30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553F30.json) | основа; `CFloat` `+0x28`, `+0x2C` |
| `0xA0012` target | [`0x6F553F90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553F90.json) | point; dword `+0x30`, `+0x34` |
| `0xA0013` two targets | [`0x6F553FF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553FF0.json) | target; dword `+0x38`, `+0x3C` |
| `0xA0014` fogged | [`0x6F554050`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554050.json) | point; dword `+0x30`, `+0x34`, `+0x38`; byte `+0x3C`; `CFloat` `+0x40`, `+0x44` |
| `0xA0015` fogged plus target | [`0x6F554100`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554100.json) | fogged; dword `+0x48`, `+0x4C` |

По исходным телам writer не запрашивает видимость игрока и не отбрасывает
часть descriptor перед вызовом `0x6F54D970`. Эти поля поступают от
builders; их происхождение ограничено [FND-0069](FND-0069-outbound-point-target-fogged-order-builders.md)
и [FND-0071](FND-0071-fogged-target-descriptor-provenance.md).
Следовательно, право на сведения нельзя выводить из самого факта
сериализации. Это не утверждение, что пакет дошёл до другого игрока:
дальнейшие gates и адресация здесь не проверены.

## Контроли и границы

Положительный статический контроль: для `0xA0014` после общей основы
передаётся point, затем три dword descriptor, один byte и два `CFloat`;
`0xA0015` добавляет к ним ещё две dword identity. Отрицательный контроль:
`0xA0012` проходит по другой цепи и после point пишет две dword identity,
без fogged-полей `+0x38/+0x3C/+0x40/+0x44`. Вызовы writer не содержат
recipient-based filtering; это не исключает проверок до или после них.

Для нового авторитетного контракта потребуется проверить точный формат
`CFloat`, размер/обрамление записи и решение о получателе в `0x6F54D970`
и далее. Двухигроковый контроль должен отдельно снять command variant,
payload, запись в turn store и реально выданные поля при видимой,
скрытой и устаревшей цели.

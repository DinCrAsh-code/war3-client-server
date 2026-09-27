# FND-0071 — fogged order переносит снимок полей цели

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, target, fogged, descriptor, identity, disclosure |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; один исходящий путь `0x6F37C660` → `0xA0014` и соответствующий входящий callback. Fingerprint DLL неизвестен; исполнение не воспроизведено. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10 widget layout](../../../../src/Widget/widget.h), [S10 unit layout](../../../../src/Unit/unit.h), [FND-0068](FND-0068-point-target-fogged-order-family.md), [FND-0069](FND-0069-outbound-point-target-fogged-order-builders.md), [FND-0070](FND-0070-point-target-route-selected-unit-visibility.md) |

## Происхождение descriptor

В показанном target route [`0x6F37C660`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F37C660.json)
при положительном локальном `0x6F2AD5D0` вызывает
[`0x6F261E20`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F261E20.json)
на объекте цели. Результат передаётся в `0x6F339F00` и
[`0x6F2CBE20`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CBE20.json),
который кладёт шесть полей в command `0xA0014` по `+0x30..+0x44`.
Здесь `fogged` — имя класса и wire-варианта, а не доказанное состояние
невидимой цели: именно положительный локальный predicate ведёт в эту ветвь
([FND-0070](FND-0070-point-target-route-selected-unit-visibility.md)).

| Descriptor | Источник в `0x6F261E20` | Поле команды `0xA0014` |
|---|---|---|
| `+0x00` | `target+0x30`; S10 называет это `m_footprintType` | `+0x30` dword |
| `+0x04` | результат [`0x6F25D280`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F25D280.json): составная маска из полей и virtual ответов цели | `+0x34` dword |
| `+0x08` | `target+0x248`; смысл этого поля для всех типов цели не установлен | `+0x38` dword |
| `+0x0C` | virtual slot `+0xEC`; S10 называет его `GetOwningPlayerIndex` | `+0x3C` byte |
| `+0x10/+0x14` | первые две координаты из virtual `+0xB8` через `0x6F4743A0` | `+0x40/+0x44` `CFloat` |

`0x6F25D280` не возвращает константу: его ветви читают, среди прочего,
`target+0x5C`, `+0x20`, `+0x24C`, `+0x248`, `+0x33` и virtual slots.
Поэтому descriptor содержит производное состояние цели. Его полный
семантический разбор остаётся отдельной задачей. Сериализатор
[`0x6F554050`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554050.json)
пишет три dword, byte и два `CFloat` после общей point-части.

## Входящий путь и контроли

После Attach `0x6F5540B0` входящий
[`0x6F2CDFF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CDFF0.json)
копирует эти шесть полей через `0x6F255180` в локальный descriptor и
передаёт их в конструктор order `0x6F294F50` для выбранных юнитов.
Тот вызывает
[`0x6F2861E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2861E0.json):
первые четыре поля descriptor сохраняются в order `+0x68..+0x74`, а
два `CFloat` — в наблюдаемые координаты `+0x78/+0x80`. Значит поля
доживают до объекта приказа, а не используются только при разборе пакета.
Показанный callback отдельно разрешает **общую** пару object reference
`+0x20/+0x24` через `0x6F03FA30` и проверяет её tag. Он не заменяет
шесть полей descriptor данными найденного объекта перед конструктором.
Дальнейшая проверка внутри order/task этим чтением не установлена.

Положительный статический контроль: ненулевая цель с положительным
локальным predicate, прошедшая guards `0x6F37C660`, достигает
descriptor builder и `0xA0014`; его шесть полей читаются входящим
callback. Отрицательные ветви: нулевая цель уходит в point route;
нулевой predicate выбирает `0xA0012` с другим payload; неразрешённая
общая пара `+0x20/+0x24` не даёт object reference, хотя callback может
продолжить работу. Это отвергает гипотезу, что `0xA0014` переносит
только приблизительную точку, но **не** доказывает возможность подменить
результат симуляции или выдачу payload другому игроку.

Для будущего авторитетного приёмника descriptor — клиентский input,
который надо сравнивать с допустимой целью и серверным состоянием.
Совместимость требует сохранить смысл приказа по известной в тумане
цели; безопасность требует отдельно проверить, какие поля можно
передать конкретному клиенту. Следующий опыт — видимая/скрытая/устаревшая
цель на двух игроках: снять шесть полей, lookup общей identity,
результат order и эффект task по отдельности.

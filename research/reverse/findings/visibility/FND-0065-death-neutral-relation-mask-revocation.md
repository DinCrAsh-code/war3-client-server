# FND-0065 — смерть связанного юнита снимает вклад в маску обзора владельца способности

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, death, relation, player-mask, ability, revocation |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен. Адреса относятся только к этой выгрузке, DLL не запускалась. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [CAbilityNeutral vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F87A844.json), [CAbilityAllied vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F87AB44.json), [CAbilityNeutralInteract vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F886FD4.json), [neutral entry `0x6F06AE00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F06AE00.json), [allied entry `0x6F06AE70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F06AE70.json), [relation add `0x6F06ACD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F06ACD0.json), [event registration `0x6F2AB3E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AB3E0.json), [observer dispatch `0x6F62A5D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A5D0.json), [event handler `0x6F061400`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F061400.json), [D01A0 branch `0x6F0613C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F0613C0.json), [relation remove `0x6F061190`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F061190.json), [mask add `0x6F2967F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2967F0.json), [mask remove `0x6F28DC80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F28DC80.json), [mask recompute `0x6F284DA0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F284DA0.json), [mask query `0x6F27A460`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F27A460.json), [fog rebuild `0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json) |

## Установка отношения и получатель смерти

Vtable `CAbilityNeutral` `0x6F87A844` указывает на `0x6F06AE00`
в slot `+0x2F8` и на `0x6F061400` в observer slot `+0x0C`.
`CAbilityAllied` `0x6F87AB44` имеет тот же observer handler;
его `+0x2F8 = 0x6F06AE70` вызывает `0x6F06AE00` только после
положительной проверки `0x6F3A3700`. При вызове этих entry
`0x6F06AE00` может передать target в `0x6F06ACD0`.

`0x6F06ACD0` отвергает target с `target+0x5C & 0x100`, с
`target+0x20 & 1` и уже выбранный через `0x6F049740` target
в том же слоте. В прошедшей ветви он разрешает unit-владельца
ability из `ability+0x30` либо `0x6F472890`, сохраняет target
в записи ability, регистрирует ability на target для `0xD01A0`
через `0x6F2AB3E0(target, ability, 1)` и для `0xD01A2` через
`0x6F26EF10`. Затем связывает запись с `target+0x164` и вызывает
`0x6F2967F0(owner, target-player-index)`. Последний увеличивает
счётчик в блоке `owner+0x13C` через `0x6F27A3C0` и сразу
пересчитывает `owner+0x148/+0x14C` через `0x6F284DA0`.

По [FND-0059](FND-0059-dying-death-event-dispatch-fog-boundary.md)
unit-событие `0xD01A0` публикуется после установки dying bits.
При сохранённой регистрации observer dispatch попадает в
`0x6F061400`; case `0xD01A0` по адресу `0x6F0614C1` вызывает
`0x6F0613C0`. Эта функция берёт subject из event `+0x0C` и
передаёт его в `0x6F061190`.

## Условный отзыв и наблюдаемый потребитель

`0x6F061190` перебирает записи ability в пределах её ненулевого
`ability+0xBC` и через `0x6F049740` ищет именно subject смерти.
Только при совпадении он снимает
observer-записи `0xD01A0`/`0xD01A2`, разрывает отношение
`ability+0x6C ↔ subject+0x164`, разрешает owner из
`ability+0x30` либо `0x6F472890` и вызывает на нём
`0x6F28DC80` с player index из subject vtable `+0x64`.
`0x6F28DC80` уменьшает счётчик блока `owner+0x13C` через
`0x6F27A400`, затем пересчитывает `owner+0x148/+0x14C`.
Таким образом, смерть **связанного target** может снять его
вклад в маску **другого unit, владельца ability**. Это отдельный
маршрут от снятия детекта в [FND-0061](FND-0061-death-detector-contribution-revocation.md).

`0x6F27A460` читает базовые биты блока `owner+0x13C`; пересчёт
`0x6F284DA0` кладёт этот результат в `owner+0x148/+0x14C`
и добавляет к `+0x14C` биты отношений игроков. При последующем
полном fog rebuild `0x6F40A8F0` читает `unit+0x14C` для
допущенных к проходу юнитов, объединяет его с mask игрока
и передаёт в `0x6F409E00`. Это доказанный **потребитель
пересчитанной маски**, но не доказанный немедленный запуск
rebuild из death handler. Семантика блока и иных его писателей
рассмотрена в [FND-0020](FND-0020-detection-revocation.md).

## Контроли и предел

Положительный статический контроль: один активный вклад
`CAbilityNeutral` в соответствующий счётчик `owner+0x13C`,
совпадающий subject события `0xD01A0`, ненулевой
`ability+0xBC` и счётчик `1 → 0`
снимают базовый бит; пересчёт может изменить `owner+0x14C`,
после чего будущий fog rebuild прочитает новое значение.
Отрицательные контроли: отвергнутый при установке target не
получает эту observer-запись; несовпадающий subject не доходит
до `0x6F28DC80`; нулевой `ability+0xBC` сразу пропускает
цикл; при `2 → 1` базовый бит остаётся; дополнительные
биты отношений могут сохранить прежний `+0x14C` даже после
снятия базового бита. Fog rebuild также имеет собственные
фильтры юнитов и условную очистку grid `+0x30`.

`CAbilityNeutralInteract` тоже вызывает helper `0x6F06ACD0`,
но его vtable `+0x0C = 0x6F061880`, а не `0x6F061400`.
Его путь `0xD01A0` к `0x6F061190` этим обзором **не
подтверждён**; общий helper не доказывает общий death handler.
Просмотренные `0x6F061190` и `0x6F28DC80` не вызывают
fog writer `0x6F409E00`, incremental update `0x6F40A650`
или full rebuild `0x6F40A8F0`. Момент изменения grid и
конечного ответа клиенту, как и runtime-вызов entry конкретной
ability в матче, остаются открыты. Это только static source
review; следующий эксперимент — смерть единственного связанного
target при наблюдении owner-счётчика, `+0x14C`, запуска rebuild
и локальной видимости, затем повтор с двумя вкладами.

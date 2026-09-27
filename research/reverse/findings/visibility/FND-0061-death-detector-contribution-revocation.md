# FND-0061 — death-событие условно снимает вклад способности-детектора

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, death, detection, ability, observer, revocation |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен. Адреса относятся только к этой выгрузке, DLL не запускалась. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [CAbilityDetector vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F887A24.json), [activation `0x6F055070`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F055070.json), [event registration `0x6F2AB3E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AB3E0.json), [dying handler `0x6F2A7D80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A7D80.json), [death event `0x6F2AD4D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD4D0.json), [CAbilityDetector handler `0x6F0551D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F0551D0.json), [death cleanup `0x6F0550D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F0550D0.json), [target loop `0x6F054EB0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F054EB0.json), [target decrement `0x6F284A50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F284A50.json), [counter writer `0x6F27A2B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F27A2B0.json), [query `0x6F27A320`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F27A320.json) |

## Источник, регистрация и доставка

Для `CAbilityDetector` S11 vtable `0x6F887A24` задаёт activation
slot `+0x80 = 0x6F055070` и event slot `+0x0C = 0x6F0551D0`.
Тот же event slot наследуют именованные в S11 `CAbilityTrueSight`,
`CAbilityMagicSentry`, `CAbilityGyroVision` и
`CAbilityBurrowDetector`. Это RTTI-идентификация конкретного семейства,
а не предположение по адресу. При вызове activation с unit
`0x6F055070` регистрирует ability через
`0x6F2AB3E0(unit, ability, 1)`: ключ observer — `0xD01A0`.
Перед этим он регистрирует `0xD01A2`, затем настраивает эффекты
через `0x6F054FC0`. Наличие ability в матче и исполнение
activation для конкретного unit остаются runtime-условиями.

По [FND-0055](FND-0055-unit-life-threshold-dying-fog-policy.md)
положительная ветвь порога жизни выставляет у исходного unit
`unit+0x5C |= 0x100`, затем `|= 0x20`.
[FND-0059](FND-0059-dying-death-event-dispatch-fog-boundary.md)
фиксирует последующий `0x6F2AD4D0`, который после первого
события `0x80259` публикует **unit-событие `0xD01A0`** через
CUnit vtable `+0x10`. При сохранённой observer-записи общий
`0x6F62A5D0` доставляет его в CAbilityDetector slot `+0x0C`.
Ветвь `0xD01A0` в `0x6F0551D0` вызывает `0x6F0550D0`.

## Снятие вклада детектора

`0x6F0550D0` получает unit из `ability+0x30` либо разрешает
его через `0x6F472890`. Gate пропускает cleanup, если
`unit+0x5C & 0x20` **стоит** либо `unit+0x5C & 0x100`
**не стоит**. Пара битов, выставленная dying handler,
проходит этот gate. При `0x100` без `0x20` функция выходит
раньше удаления эффекта и снятия observer-регистраций.

Если `ability+0x84` содержит список затронутых объектов,
`0x6F0550D0` вызывает `0x6F031C00` и `0x6F054EB0`, затем
освобождает `ability+0x84`. `0x6F054EB0` получает список через
`0x6F4795E0`, проходит его элементы, для каждого разрешённого
target вызывает `0x6F284A50(target, ability+0x90,
flags-from-0x6F031C40)` и снимает отношение между ability и
`target+0x164` через `0x6F479260`. `0x6F284A50` разрешает
target-объект по `target+0x130` и вызывает `0x6F27A2B0`.
Последний уменьшает выбранный счётчик игрока в одном или обоих
каналах; при переходе `1 → 0` снимает бит игрока в
`block+0x24` либо `block+0x6C`. Эти же счётчики читает
`CUnit::QueryDetection` `0x6F27A320` — базовый контракт
описан в [FND-0012](FND-0012-detection-refcounts.md).

Независимо от того, был ли `ability+0x84` ненулевым,
прошедший gate `0x6F0550D0` затем снимает регистрацию
`0x80261` у связанного observer и отписывает ability от unit
`0xD01A0` через `0x6F2AB3E0(unit, ability, 0)` и
`0xD01A2` через `0x6F26EF10(unit, ability, 0)`.
Последние два вызова являются **удалением observer-записей**,
а не публикацией новых событий.

## Контроли и предел раскрытия

Положительный контроль: активированный detector с непустым
`ability+0x84`, корректными target/relation, выбранным флагами
каналом и счётчиком `1` может пройти death delivery и уменьшить
этот счётчик до нуля; его бит снимается, а `QueryDetection`
по этому каналу для target начинает возвращать `0`, если другой
выбранный канал не активен. Отрицательные контроли: неисполненная
activation или отсутствующая observer-запись не доставляют
`0xD01A0` по данному маршруту; `0x100` без `0x20` пропускает
cleanup; нулевой `ability+0x84` не запускает target loop;
счётчик `2 → 1` не снимает бит и сохраняет положительный
`QueryDetection`; другой активный канал также может сохранить
положительный ответ. Если `0x6F27A2B0` встречает неположительный
счётчик, он возвращает `0` без ожидаемого `1 → 0`.

Это отзыв **вклада способности в детект конкретных targets**, а
не доказательство отзыва радиуса обзора исходного unit. Проверенные
тела не вызывают fog writer `0x6F409E00`, incremental
`0x6F40A650` или full rebuild `0x6F40A8F0`; не прослежен
момент очистки `fogTable+0x30` или экранного скрытия target.
Точная интерпретация `ability+0x90`, двух каналов и всех
путей фильтра `0x6F284A50` остаётся открытой. Это static
source review, не воспроизведение на DLL. Для проверки выдачи
нужно убить единственный source detector при одном невидимом
target, затем повторить с двумя перекрывающимися источниками:
сравнить observer deliveries, оба счётчика, `QueryDetection`,
`IsUnitDetected`, fog plane и локальный ответ публикации.

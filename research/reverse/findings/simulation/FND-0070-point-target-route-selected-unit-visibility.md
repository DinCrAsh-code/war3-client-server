# FND-0070 — локальный бит цели выбирает fogged приказ отдельно от selection

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, point, target, fogged, selection, visibility, disclosure |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; один high-level путь через `0x6F381060` и `0x6F37C660`, не все способы выдачи приказа. Fingerprint DLL неизвестен, динамического опыта нет. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10 local predicate](../../../../src/Widget/widget_visibleflags.cpp), [FND-0030](../visibility/FND-0030-local-sprite-refresh-gate.md), [FND-0068](FND-0068-point-target-fogged-order-family.md), [FND-0069](FND-0069-outbound-point-target-fogged-order-builders.md) |

## Три варианта в одном входе

Короткий [`0x6F381060`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F381060.json)
читает `record+0x30`. Ненулевое значение передаёт вместе с записью в
[`0x6F37C660`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F37C660.json)
для target route. Нуль берёт два float из `record+0x1C/+0x20` и
направляет их в [`0x6F37AC00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F37AC00.json)
для point route. Поэтому отсутствие ссылки на цель в этом входе не
означает отсутствие приказа: остаётся point путь.

Для существующей цели `0x6F37C660` вызывает
[`0x6F2AD5D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD5D0.json)
на разрешённом target object. В S11 это `EXACT` 13/13/13,
`CWidget::IsVisibleToLocalPlayer`; его C++ тело S10 отдельно прочитано.
Функция возвращает `1` при
`target+0x2E & 0x8000`, кроме ветви, в которой одновременно
`0x6F53F160` положителен и установлен bit `1` у объекта
`dword_6FAB65F4+0x34` в поле `+0x24`; иначе возвращает `0`.
`0x6F37C660` сохраняет **этот результат** в слот стека, который IDA
называет `arg_0`, перезаписывая прежний аргумент. Он выбирает
`0x6F339F00` (`0xA0014`) при результате `1` и `0x6F339D50`
(`0xA0012`) при `0`. Это выбор по **локальному predicate видимости
target**, а не доказательство server-side права на каждое его поле.
В этой конкретной ветви **положительная** локальная видимость ведёт к
wire-классу с именем `Fogged` (`0xA0014`). Значит название класса
нельзя читать как условие «цель скрыта».
[FND-0030](../visibility/FND-0030-local-sprite-refresh-gate.md)
показывает writer `0x6F2AB710`, который меняет этот же старший бит
`widget+0x2E` после fog rescan. Два предшествующих запроса
`0x6F4212D0`/`0x6F420DC0` и возможный fallback в point route
могут прервать этот выбор до отправки.

Независимо от этого `0x6F37C660` получает локальную selection через
`0x6F3A1650`, а [`0x6F421E50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F421E50.json)
берёт её первый объект через индекс `0`. При значении поля selection
`+0x1F8 > 1` и отрицательном результате
[`0x6F26DE50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26DE50.json)
для **этого выбранного объекта**, `0x6F339BC0` может поставить
дополнительный bit в исходный flag word. Это проверка выбранного
объекта; она не является доказанным тестом видимости цели из
`record+0x30`. Позднее flags передаются wrapper, чей bit mapping
описан в [FND-0069](FND-0069-outbound-point-target-fogged-order-builders.md).

Таким образом, локальная видимость выбранного юнита и локальный
predicate цели участвуют в **разных** решениях. Даже после выбора
`fogged` само имя варианта не доказывает, что payload раскрывает лишь
разрешённые сведения о target.

## Контроли и пределы

Положительный статический контроль: `record+0x30 != 0` допускает target
route; если его guards пропускают, `0x6F2AD5D0 == 1` ведёт к
`0xA0014`, а `== 0` — к `0xA0012`. Отрицательные ветви: нулевое
`record+0x30` уходит к point; `0x6F37C660` может вернуться до
отправки при `0x6F53F160` или пустой selection, а некоторые проверки
возвращают его к point route. Не доказаны тип записи, точный тайминг
между fog rescan и отправкой, допустимость скрытой цели, получение команды peer и
эффект в мире. Вывод по одному маршруту не распространяется на
JASS-issued orders и все способности.

Для архитектурного опыта нужны два игрока и три состояния цели:
видима, скрыта и устаревшая identity. Снимать конкретный wire ID,
flags, разрешение ссылки в receiver и итоговый order/task отдельно;
сравнить с point-only контрольной записью.

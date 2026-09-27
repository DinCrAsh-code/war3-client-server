# FND-0038 — ответ публикации юнита условно допускает локальный текст «MISS»

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, SubmitUnit, fog, combat, GameUI, text-tag, presentation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен; адреса — координаты S11. S10 использован как вторичная реконструкция. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [consumer `0x6F2BB970`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2BB970.json), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [CWidget vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F92E7BC.json), [`PublishPosition` `0x6F285110`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F285110.json), [`SubmitUnit` без индекса](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A3830.json), [`SubmitUnit` с индексом](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A38F0.json), [UI gate `0x6F3335A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3335A0.json), [text tag entry `0x6F333600`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F333600.json), [text tag sink `0x6F4E5600`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4E5600.json) |

## Связная ветвь

`0x6F2BB970` — конкретный потребитель результата виртуального slot
`+0x100`, который при runtime типе CUnit ведёт в `0x6F285110`.
До вызова он проверяет локальный predicate
`0x6F2AD5D0` по `widget+0x2E & 0x8000` с исключением для game mode 1
и флага `worldState+0x24 & 1`. Если predicate возвращает ноль,
consumer вызывает slot `+0x100` с аргументами `(0, 4)` и проверяет
возвращённый `eax`. Здесь `4` — запрошенная маска ответа, переданная
обоим вариантам `SubmitUnit`, а не индекс игрока. CUnit vtable
разрешает этот slot в `0x6F285110`, а базовая CWidget vtable — в
другой метод `0x6F2AD710`. Один виртуальный callsite не доказывает
динамический тип каждого получателя. Пример реального входа в
consumer есть в [combat path `0x6F0CF660`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F0CF660.json),
где вызов следует за ветвью промаха. Есть и другие прямые caller'ы;
этот пример не делает путь единственным.

`PublishPosition` использует локальный индекс из таблицы игроков
`+0x28`. При ненулевом handle-ref записи `+0xF0` он выбирает
`0x6F3A3830` с масками `worldState+0x40` для раннего gate и
`worldState+0x3C` для fog query; при нулевом — `0x6F3A38F0`
с явным индексом. Если `SubmitUnit` вернул ноль, `0x6F285110`
дополнительно проверяет биты самого юнита `+0x148/+0x14C`
через `0x6F26F9E0`. Только отрицание обоих результатов даёт
consumer ноль. Устройство двух масок и ранних выходов описано в
[FND-0017](FND-0017-local-unit-publication-masks.md) и
[FND-0010](FND-0010-unit-submit-gates.md); этот consumer показывает,
что их итоговый ответ действительно читается локальным представлением.

При положительном ответе `0x6F2BB970` требует
`0x6F3335A0 == 1`: `GetGameUI(0,0)` должен вернуть существующий
GameUI с ненулевыми `+0x1AC` и `+0x1B0`. Далее собираются литерал
`MISS`, позиция юнита через
[`0x6F2781F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2781F0.json)
и параметры `MissTextFadeStart`, `MissTextLifetime`,
`MissTextVelocity`, `MissTextHeight`, `MissTextColor` из `Misc`.
Цепь прямых вызовов — `0x6F333600 → 0x6F00F420 → 0x6F00F320 →
0x6F4E5600`. Последний путь получает `TextTagFont`, выделяет
локальную запись длиной `0x34` в своём массиве, копирует туда
аргументы позиции/цвета и вызывает графические helpers
`0x6F7BA790`, `0x6F7B8FE0`, `0x6F7B8C90`. Это граница подготовки
локального текстового тега. Точный экранный результат и возможные
эффекты нижних helpers статически здесь не проверены.

## Контроли и архитектурный предел

Положительный контроль: при runtime CUnit, нулевом `0x6F2AD5D0`,
положительном `PublishPosition(0,4)` и готовом GameUI consumer доходит до
`0x6F4E5600`. Внутри `PublishPosition` положительный ответ возможен
как от `SubmitUnit`, так и от запасной проверки битов юнита; в
`SubmitUnit` бит флага 0 способен дать ранний ответ до fog grid.
Поэтому факт текстового тега сам по себе не доказывает, что fog
grid разрешила раскрытие.

Отрицательные контроли: ненулевой predicate `0x6F2AD5D0` обходит
`PublishPosition` и текст; нулевая таблица игроков либо её активный
slot `+0x3E0` дают отрицательный ответ до обоих `SubmitUnit`;
нулевой ответ `PublishPosition` обходит GameUI; отсутствие готового
GameUI обходит текстовый tag sink. Даже внутри `0x6F4E5600` отказ
его проверки `+0x18` заканчивается до записи и графических вызовов.

В S11 `0x6F2BB970`, `0x6F333600`, `0x6F4E5600` и оба `SubmitUnit`
имеют статус `TODO`; `0x6F285110` — `DIFFERS`. Разбор основан на
IDA-body, без исполнения. S10
[`unit_publishposition.cpp`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/pipeline/src/Unit/unit_publishposition.cpp)
объявляет `PublishPosition` как `void`, хотя S11 caller
`0x6F2BB970` сразу проверяет его `eax`; сигнатуру вторичного C++
нельзя брать за контракт возвращаемого значения без проверки.

На прослеженном пути не найден вызов сетевого сериализатора или
транспорта: доказан локальный потребитель решения, а не формат
server→client выдачи. Просмотренная цепь передаёт в text tag
литерал/параметры текста и вычисленную позицию; она не показывает
передачу health, инвентаря, команд или полного объекта юнита.
Это не утверждение об остальных ветвях или полном отсутствии
сетевого пути в движке. Для будущего серверного решения нужна
отдельная per-player граница **до передачи полей скрытого юнита**;
`PublishPosition` и его UI-consumer лишь демонстрируют, что локальная
проверка смешивает relation/fog, запасную битовую маску и режимные
ранние ответы. Следующий опыт — при одном промахе сопоставить
`SubmitUnit` code, результат slot `+0x100`, GameUI gate, появление
text tag и захваченные bytes сетевого отправителя для двух игроков
с разными правами обзора.

# FND-0015 — alliance 5/9 наполняют маску игрока-получателя

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | alliance, player-relations, unit, jass, presentation |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса ниже — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [`JASS_SetPlayerAlliance`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C1050.json), [enable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3E6890.json), [disable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3E69E0.json), [пересчёт `0x6F41B4C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F41B4C0.json), [второй тест `0x6F3DF190`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3DF190.json) |

## Вывод

В `JASS_SetPlayerAlliance(A, B, type, value)` агент игрока `A` меняет
бит индекса `B` в поле `+0x38 + 0x10*type`. Для `type=5` это поле
`+0x88`, для `type=9` — `+0xC8`. В этих двух случаях native затем
вызывает `0x6F41B4C0` **для B**, несмотря на имя реконструкции
`CPlayerWar3FinishLeaveNotify`. Функция пересчитывает слово `B+0x2E0`:
обходит слоты `i=0..15`, получает агента игрока `i`, проверяет в его
полях `+0x88` и `+0xC8` бит индекса `B` и, если хотя бы один тест
успешен, устанавливает бит `i` в слове `B+0x2E0`.

Значит, в этом маршруте маска получателя `B` составляется из игроков,
чьи два поля отношений указывают на `B`. Это направленная связь:
изменение `A → B` пересчитывает именно `B+0x2E0`, а не автоматически
`A+0x2E0`. Слово `+0x2E0` затем читают
[`SubmitUnit`](FND-0010-unit-submit-gates.md) и локальный пересчёт
`SWorldVisibilityMaskState::RefreshPlayerWorldVisibilityMasks`.
Имена «shared vision» из комментариев к C++ здесь означают только
наблюдаемый путь маски; перечень допустимых клиенту полей не доказан.

## Ветви и статические контроли

| Адрес S11 | Роль | Статус S11 |
|---|---|---|
| `0x6F3E6890` / `0x6F3E69E0` | Set/clear бита B в поле агента A, индексированном `type` | THUNK / THUNK |
| `0x6F41B4C0` | Очистить и пересчитать `B+0x2E0`, затем возможные world/UI-вызовы | THUNK |
| `0x6F3DF190` | Проверить бит B в поле агента `+0xC8` | TODO |
| `0x6F408070` | Переложить маски локального игрока в world state | DIFFERS |

Положительный статический путь: `EnableAllianceFlag` при `type=5/9`
пишет поле `A+0x88/+0xC8`; цикл `0x6F41B4C0` читает эти поля у
записи слота `i=A` и OR-ит `1<<A` в `B+0x2E0`. Для `DisableAllianceFlag`
бит снимается, после чего полный пересчёт может убрать бит `A` только
если **оба** отношения от A к B теперь ложны.

Отрицательные пути: если оба теста отношения ложны, слот `i` не добавляется
в пересчитанную маску. Но ненулевое поле `B+0xF0` обходит цикл и ставит
`B+0x2E0 = 0x0FFF`, независимо от двух отношений; назначение этого
исключения не установлено. Для `type` вне `5/9` данный native не вызывает
`0x6F41B4C0`, хотя set/clear выбранного поля всё равно выполняется.
Это уточняет [FND-0011](FND-0011-alliance-refresh-divergence.md):
лишний вызов обновления мировой маски в C++ не равен пересчёту `+0x2E0`.

После записи `+0x2E0` функция проверяет, совпадает ли `B+0x30` с
локальным слотом мира `+0x28`; при совпадении и активном мире
(`world+0x3E0 != 0`) обновляет `world+0x34` и вызывает cleanup.
При активном мире также достигается локальный `WorldFrameWar3` путь.
Это побочные эффекты представления; необходимость каждого для игровой
симуляции не доказана.

## Пределы и следующий опыт

Выполнено чтение IDA-тела, без сборки, исполнения и сверки fingerprint.
Почему цикл проходит 16 слотов, а ветвь `+0xF0` пишет только 12 бит,
пока не объяснено. Возможны другие писатели `+0x2E0`; их полнота не
установлена. Не следует считать это слово готовым серверным ACL.
Проверка на двух игроках: `A → B` отдельно для 5 и 9, затем снять
сначала один тип, потом второй; сверять оба поля агента A, `B+0x2E0`,
ответы `SubmitUnit`/JASS и локальный UI. Контроль: обратное `B → A`
не задавать, `B+0xF0` держать нулевым и наблюдать, не меняется ли
`A+0x2E0` без отдельного источника.

Слово `+0x2E0` также выбирается JASS-нативами записи тумана карты при
ненулевом булевом аргументе: [FND-0016](FND-0016-map-fog-shared-mask.md).

# FND-0080 — replay payload идёт кусками `0x81`/`0x82` с `0x83` на конце

| Поле | Значение |
|---|---|
| Subsystem | network |
| Tags | replay, chunk, compression, serialization, decode, compatibility |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; исходящий `0x6F549450` → codec `0x6F6562F0` → serializers `0x6F5482A0/350/400`, входящий `0x6F5500E0` → `0x6F6563A0`. Известны локальные gates и поля записи; доставка peer, содержимое codec tables и исполнение replay не проверены. Fingerprint DLL неизвестен. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0078](FND-0078-local-event-turn-parser.md); [C++ PR #53](https://github.com/FilippTheBestDev/claudecraft/pull/53) как вторичная реконструкция |

## Исходящий маршрут

[`0x6F549450`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F549450.json)
возвращает `0`, если `netData+0xAD4 > 4` как **беззнаковое** число
или offset `+0xACC >=` длины replay stream `+0x628`. Иначе берёт
`min(+0x628 - +0xACC, 0x3FD)` байтов, ставит read position stream
`+0x62C` в `+0xACC` и читает chunk. Запись `0x81` имеет 16-битную
длину по смещению `+0x18` и payload с `+0x1A`.

Тот же chunk передаётся
[`0x6F6562F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F6562F0.json)
с выходной ёмкостью, равной исходному размеру chunk. Encoder читает
две таблицы по адресам S11 `0x6FA9B820` (число бит) и `0x6FACF518`
(код), упаковывает биты и возвращает `0`, если ёмкости не хватает.
Содержимое таблиц в доступных function records отсутствует. Ненулевой
результат выбирает сериализатор сжатого `0x82`
[`0x6F5482A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5482A0.json),
нулевой — несжатого `0x81`
[`0x6F548350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F548350.json).
Оба writer (`0x6F554690`/`0x6F554650`) пишут 16-битную длину и ровно
столько payload bytes после subtype. Оба serializers создают временный
turn store через `0x6F543D90`, пишут byte subtype, вызывают writer,
`0x6F650480` для flush и `0x6F2C95B0` для разрушения store.

После записи `0x6F549450` увеличивает `+0xAD4` на один и `+0xACC`
на размер исходного chunk. Когда новый `+0xACC >= +0x628`, вызывается
[`0x6F548400`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F548400.json)
для subtype `0x83` без дополнительных полей. Это локальный flush,
не доказательство получения другим участником.

## Входящий маршрут и контроли

[`0x6F5500E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5500E0.json)
не разбирает payload, если `netData+0xAD0 != 0`; при `+0x58C != 0`
тоже обходит ветвь append. При ненулевом аргументе compressed он
вызывает
[`0x6F6563A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F6563A0.json)
с выходной ёмкостью `0x3FD`. Decoder использует таблицы S11
`0x6FACF118` и `0x6F970F10`; при нулевом результате append не
вызывается, вместо него идёт `0x6F54C7E0` с кодом `7`. При ненулевом
результате decoded bytes добавляются к replay stream `+0x618` вызовом
`0x6F4C25A0`; для несжатого аргумента туда же идут исходные bytes.
Затем `+0xACC` увеличивается на фактически добавленную длину.

Положительный статический контроль: ненулевое сжатие одного chunk
выбирает `0x82`, нулевое — `0x81`; завершение чтения stream вызывает
`0x83`. Отрицательные ветви: `+0xAD4 > 4` или offset за длиной не
вызывают сериализатор; нулевой результат decoder не попадает в append.
Не установлены содержимое и происхождение codec tables, значение
`+0x58C`, реакция на нулевой chunk, sender/peer admission, точный
порядок replay при потере записи и совместимость полного replay с
обычным режимом или кастомной картой. Для исполнения этих C++-тел в
другом образе необходимы data mapping и точный fingerprint Game.dll;
адреса S11 не являются переносимым runtime контрактом.

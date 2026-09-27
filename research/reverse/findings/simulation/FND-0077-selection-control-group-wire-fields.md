# FND-0077 — selection и control-group используют пять путей записи

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | selection, control-group, serialization, handle-pair, outbound |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; пять serializers `0x6F2C9C50`–`0x6F2C9F50` и writers `0x6F554160`–`0x6F5542D0`. Установлена локальная запись байтов, не семантика выбора юнитов на двух игроках и не доставка peer. Fingerprint DLL неизвестен. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0074](FND-0074-order-command-store-admission.md); [C++ PR #53](https://github.com/FilippTheBestDev/claudecraft/pull/53), commit `5a8fe74ef` как вторичная реконструкция |

## Поля и порядок

Каждый из пяти serializers создаёт временный `CDataStoreCache1460`, пишет
байт команды из `cmd+0x14` через `0x6F4C2160`, вызывает свой writer,
передаёт store и player index в `0x6F54D970`, затем разрушает store.
Различие лежит в writer, а не в общем queue gate из
[FND-0074](FND-0074-order-command-store-admission.md).

| Байт `+0x14` в соответствующем классе | Serializer | Writer | Следующие поля в порядке записи |
|---|---|---|---|
| `0x16` selection modify | [`0x6F2C9C50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9C50.json) | [`0x6F554160`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554160.json) | byte `cmd+0x18`, word из low16 `cmd+0x20`, затем `count` пар dword из массива `cmd+0x24` |
| `0x17` define control group | [`0x6F2C9D10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9D10.json) | [`0x6F5541C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5541C0.json) | та же форма, отдельная оригинальная функция |
| `0x18` select control group | [`0x6F2C9DD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9DD0.json) | [`0x6F554220`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554220.json) | byte `cmd+0x19`, затем byte `cmd+0x18` |
| `0x19` select subgroup | [`0x6F2C9E90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9E90.json) | [`0x6F554270`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554270.json) | dword `cmd+0x18`, `+0x1C`, `+0x20` |
| `0x1A` refresh subgroup | [`0x6F2C9F50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9F50.json) | `0x6F5542D0` | нет дополнительных полей |

Значения `0x16`–`0x1A` и имена классов сверены с S11 constructors
[`0x6F546370`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F546370.json),
[`0x6F546450`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F546450.json),
[`0x6F540D60`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F540D60.json),
[`0x6F540E30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F540E30.json) и
[`0x6F540F00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F540F00.json),
но каждый serializer берёт байт
из объекта, а не подставляет литерал. Для `0x16`/`0x17` writer пишет
лишь младшие 16 бит count, после чего циклом по **полному 32-битному**
count записывает две dword на каждую пару. Исходный writer не ставит
здесь отдельную проверку максимального count.

## Контроли и границы

Положительный статический контроль: count `0` для `0x16`/`0x17`
пишет mode и нулевое слово без пар; count `1` добавляет ровно две
dword. Для `0x18` порядок `+0x19` перед `+0x18` подтверждается двумя
последовательными вызовами byte writer; замена порядка была бы другой
записью. Отрицательный контроль: ни один из пяти serializers сам не
выбирает получателя и не вызывает inbound обработчик; все заканчивают
в одном `0x6F54D970`. Для count `>0xFFFF` wire count и число
записанных пар могут расходиться; достижимость таких объектов в
обычном матче здесь не установлена.

Не следует считать пару dword проверенной unit identity или новым
сетевым контрактом продукта: из этой цепи известна форма записи,
а не admission источника, семантика пары на приёме либо право видеть
объект. Следующий шаг — связать новые producer `0x16`–`0x18` с
selection state, затем read-side decode и отрицательные контроли
чужой/скрытой identity на двух игроках. C++ PR #53 содержит девять
новых `DIFFERS` тел; native VC8 и bounded ASM сравнение не дают
official EXACT или runtime доказательства.

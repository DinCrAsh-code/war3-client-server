# FND-0057 — producer ставит изменения выделения перед базовым приказом

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | selection, order, network-command, identity, queue |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, предполагаемая Game.dll 1.26a/build 6401 x86; один путь CNetCommandUnitOrderBasic и CNetCommandUnitSelectionModify. Fingerprint DLL и исполнение не проверены. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10](../../SOURCES.md#s10), commit `2fc76c8035554912dd66fc0a06a39eda376a806c`; [FND-0002](FND-0002-command-order.md) |

## Вывод

В показанном локальном producer базового unit order сначала вызывается
реализация selection. Если она нашла непустые списки изменений, она
вызывает `CNetCommandUnitSelectionModify` с mode `2`, затем с mode `1`.
Лишь после возврата producer сериализует `CNetCommandUnitOrderBasic`.
Это причинный порядок **попыток постановки команд в локальный store**;
каждый send имеет собственные gates. Unit order не содержит списка
выделенных юнитов: у него word и три dword после заголовка, тогда как
selection modify несёт byte mode, word count и пары identity dword.
Приёмная сторона разбирает их как разные action cases и передаёт
observer lists только после локальных parse/sender gates.

Таким образом, порядок selection → order из [FND-0002](FND-0002-command-order.md)
имеет конкретный producer и parser boundary. Это не доказательство,
что любой selection delta и приказ доставятся или будут приняты миром,
и не спецификация нового клиентского API.

## Основание: отправитель и формат

1. [`0x6F339C60`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F339C60.json) собирает word флагов из двух аргументов и зовёт [`0x6F2CB940`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CB940.json). Тот строит `0xA0010` (`sub-index 0x10`): `+0x18` получает word, `+0x1C` — исходный `ecx`, `+0x20/+0x24` — `source+0x0C/+0x10` либо `-1/-1` при null source. Затем он выбирает player entry из world по `world+0x28`, вызывает `0x6F425490` с нулевым аргументом и **только после возврата** передаёт order в `0x6F2C97D0`.
2. [`0x6F425490`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F425490.json) сравнивает списки выбора. На ветви `arg_0==0` при ненулевом первом count в `0x6F425A07` зовёт [`0x6F2CF5A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CF5A0.json) с mode byte `2`; при ненулевом втором count в `0x6F425A19` зовёт его с byte `1`. Порядок двух вызовов виден в одном теле, но смысл mode `1/2` для итоговой selection membership здесь не назначается.
3. `0x6F2CF5A0` ограничивает число просматриваемых записей `min(count,12)`, проверяет доступную identity и gate объекта, складывает принятые пары `+0x0C/+0x10` в `0xA0016`, затем вызывает [`0x6F2C9C50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C9C50.json). `0x6F2C9C50` пишет sub-index и через [`0x6F554160`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554160.json) mode/count/пары в store, затем вызывает [`0x6F54D970`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54D970.json). Order идёт через `0x6F2C97D0` и собственный writer [`0x6F553EB0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553EB0.json): word `+0x18`, затем dword `+0x1C/+0x20/+0x24`, без массива selection.
4. Общий `0x6F54D970` проверяет активный индекс сетевой записи, состояние player record, минимальный размер, некоторые branch-specific gates и вместимость store (`0x400` байт до flush). Он может отвергнуть любую из команд; сама последовательность вызовов не гарантирует сериализованную пару. Запись в store в показанной ветви выполняется после этих gates.

## Основание: вход и отрицательные контроли

В S10 [`CNetData::DispatchActionByte`](../../../../src/Net/netcommand_dispatch.cpp) case `16` направляет `0xA0010` в [builder `0x6F540790`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F540790.json), case `22` направляет `0xA0016` в [builder `0x6F546370`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F546370.json).
S11 [`0x6F553EF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F553EF0.json) читает word и три dword в basic order. [`0x6F554430`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554430.json) читает mode byte, word count и пары dword в selection modify; в этом Attach нет локального cap `12` и проверки владельца каждой identity. Это утверждение только о показанном parser body, не о других слоях.

Оба builder после Attach пропускают observer fire при `suppressFire != 0` и проверяют выход read position за объявленную длину. Их FireCommand (`0x6F53B910`/`0x6F53BA90`) не передаёт sender `0xFF` в [`CNetPlayerRecord::FireToObserverLists`, `0x6F5378E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5378E0.json); при другом sender вызовы идут в две observer lists. Отрицательные ветви producer: нулевой selection count не вызывает delta builder; `arg_0!=0` в `0x6F425490` пропускает оба delta sends; null source в basic order даёт `-1/-1`, а не identity объекта. Разрешение этих ссылок, право sender управлять юнитом и итоговое изменение order/world здесь ещё не прослежены.

`0x6F553EF0` имеет статус `MISMATCH`, а некоторые send/parser bodies — `TODO` в matching pipeline. Их IDA `raw_asm` прочитан, но C++ S10 не принимается за byte-identical оракул. Следующий рубеж участка — concrete observer recipient, lookup identity и запись текущего приказа для move/stop/target; отдельно проверить устаревший handle и перестановку selection/stop в собственном двухигроковом сценарии.

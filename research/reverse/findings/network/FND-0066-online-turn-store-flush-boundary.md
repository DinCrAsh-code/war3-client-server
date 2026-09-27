# FND-0066 — online turn store копирует command bytes до отправки через net client

| Поле | Значение |
|---|---|
| Subsystem | network |
| Tags | turn, command-store, tls, net-client, outbound, peer-boundary, jass |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | S11, статический маршрут исходящей команды и flush при session tag вне `LOOP`/`NONE` предполагаемой Game.dll 1.26a/build 6401 x86; fingerprint образа, wire capture и сетевой peer не проверены |
| Sources | [S11](../../SOURCES.md#s11-адресный-корпус-ida-коллеги), commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10](../../SOURCES.md#s10), commit `2fc76c8035554912dd66fc0a06a39eda376a806c` |

## Вывод

Исходящий [`0x6F54D970`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54D970.json) берёт уже сериализованные bytes command store и при совпадении переданного record index с активным `netData+0x610` добавляет их в `CTurnStore` по `netData+0x1C78`. Он **не вызывает** локальный slot→sender-key writer `0x6F53E080` в показанном теле. Ветвь tag вне `LOOP`/`NONE` позже отправляет содержимое `CTurnStore` через net client из TLS slot `0x0E` и сбрасывает store. Это ограничивает [FND-0063](FND-0063-local-turn-sender-key.md): его loopback serializer нельзя переносить на online buffer без отдельного доказательства.

Путь заканчивается на виртуальном sink за `CNetClient+0x2B4`. Его dynamic type, связь с конкретным peer и входное декодирование до queued turn здесь не установлены. Наличие TLS network client не доказывает authenticated peer→participant key.

## Адресный маршрут и контроли

1. Конкретный JASS пример: [`0x6F43D840`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F43D840.json) создаёт временный `CDataStore`, пишет command byte из объекта, дописывает поля через [`0x6F554CF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F554CF0.json) и вызывает `0x6F54D970` с этим store и record index. [FND-0047](../jass/FND-0047-resume-dispatch-tls-boundary.md) связывает этого caller с исходящим `ResumeTriggerExec`; здесь значимы его уже готовые bytes, а не повторное описание trigger handler. `0x6F554CF0` пишет три слова из объекта `+0x18/+0x1C/+0x20` через `0x6F4C2360`; identity владельца этих полей внутри сетевой сессии сам writer не проверяет.
2. `0x6F54D970` через [`0x6F4C34D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4C34D0.json) читает TLS slot `13` → net data, требует `arg record index == netData+0x610` и `record+0x278>=3`, запрашивает `{src,size}` у command store через его vtable `+0x24` и требует `size>0`. Для index `0` при ненулевом `dword_6FAB65F4` он дополнительно передаёт первый byte в [`0x6F545270`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F545270.json); ненулевой результат прекращает запись. Это положительные gates и отрицательные контроли: иной record, преждевременное состояние, пустой store либо отвергнутый byte не достигают append.
3. Если накопленный размер `CTurnStore` с новым payload достигает `0x400`, `0x6F54D970` сначала вызывает [`0x6F54D930`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54D930.json). После этого повторный расчёт требует итог `<0x400`; иначе writer выходит через `NetSendCommandStore: rejected` без append. При успехе [`0x6F4C25A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4C25A0.json) копирует указанный byte range в `netData+0x1C78`; отдельного добавления sender-key в `0x6F54D970` нет. Поскольку ранний flush зависит от tag, этот пункт сам по себе не доказывает отправку именно online peer.
4. Для tag вне `LOOP`/`NONE` `0x6F54D930` вызывает [`0x6F650480`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F650480.json) на `netData+0x1C78`. Тот получает `{buffer,size}` через vtable `+0x24`, пропускает первые восемь bytes, при размере остатка `(0,0x400]` читает TLS slot `0x0E` и вызывает [`0x6F657340`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F657340.json). Последний tail-dispatch передаёт buffer и size в slot `0` объекта `CNetClient+0x2B4`. Нулевой остаток, недопустимый размер или пустой TLS client не достигают sink — отрицательные контроли. Затем `0x6F54D930` вызывает vtable `+0x1C` `CTurnStore` (`0x6F958670`) → [`0x6F543DF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F543DF0.json), который очищает/переинициализирует store. [S10 `CNetSendQueue::Flush`](../../../../src/Net/netclientevent.cpp) и [конструктор store](../../../../src/Net/netdatactor.cpp) подтверждают структуру subobject, но не заменяют S11 проверку.

## Ограничения и отвергнутые гипотезы

Прочитаны S11 `raw_asm` и S10 без запуска DLL и byte-match. S11 status: `0x6F54D970`/`0x6F543DF0`/`0x6F43D840` — `TODO`, `0x6F650480`/`0x6F4C25A0`/`0x6F545270` — `DIFFERS`, `0x6F657340` — `EXACT`. Не доказано, что исходящий command store содержит либо не содержит key в других звеньях; доказано только отсутствие его записи внутри показанного `0x6F54D970`. Не установлен dynamic type sink `+0x2B4`, wire framing после него, входной peer admission и путь к `0x6F54CAD0`/`0x6F550730`. Нельзя выводить потерю данных в штатном режиме из статических ветвей null TLS или failed send: их реальная достижимость и обработка выше не изучены.

## Значение для проекта

Авторитетный сервер должен различать готовую команду, выбранную запись, transport connection и player identity. Исходящий `CTurnStore` доказывает буферизацию и предел размера, но не заменяет контракт `connection → session → player`. Следующий адресный шаг — определить dynamic type sink `CNetClient+0x2B4`, его send/receive pair и место, где online bytes превращаются в event с проверенным sender; двухклиентный trace должен сопоставить исходящий store, реальный peer, queued event key и разрешённого player ID.

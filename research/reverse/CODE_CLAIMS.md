# Занятые адреса для новой C++-реконструкции

Этот реестр помогает агентам не переписывать одну функцию одновременно.
Адреса относятся к изученному срезу
[`claudecraft`](https://github.com/FilippTheBestDev/claudecraft/tree/10950d496aa7357a6180c1956def50c2a3e8c1a9)
`10950d496aa7357a6180c1956def50c2a3e8c1a9` (Warcraft III 1.26a,
build 6401). Снимок статусов: 2026-09-27.
Состояние здесь — снимок открытых веток, а не доказательство совпадения с
оригинальной `Game.dll`. Перед новой работой проверить этот список, открытый
PR и `agent_worktrees/funcs/0xADDR.json` в соответствующей ветке.

`DIFFERS` означает, что собственное C++-тело уже написано, но равенство
оригиналу не доказано. `THUNK` — временный переход в оригинал, **не**
переписанная функция. `TODO` — статус незавершённого адреса; наличие
опубликованного C++-тела у такого адреса отмечается прямо в строке.
Поля `source_tu`, `score` и `match` в store зарезервированы для штатного
`verify.py`; отсутствие этих полей не отменяет наличия исходника. При смене
статуса кодер обновляет адресную запись и описание своего PR; ведущий
синхронизирует этот межрепозиторный реестр при следующем осмысленном
изменении. На занятый адрес без сверки с владельцем не заходить.

## Обзор, обнаружение и туман — [PR #52](https://github.com/FilippTheBestDev/claudecraft/pull/52)

Владелец: `codex/visibility-fog-rescan`. 58 новых C++ тел в опубликованной
ветке; две временные зависимости остаются переходами в оригинал.

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F2AB710` | DIFFERS | видимость |
| `0x6F407DD0` | DIFFERS | обзор |
| `0x6F407E60` | DIFFERS | обзор |
| `0x6F392E70` | DIFFERS | обнаружение |
| `0x6F38C550` | DIFFERS | обнаружение |
| `0x6F38C5E0` | DIFFERS | обнаружение |
| `0x6F392B70` | DIFFERS | обнаружение |
| `0x6F3A1520` | DIFFERS | обнаружение |
| `0x6F406B00` | DIFFERS | пересчёт тумана |
| `0x6F406CC0` | DIFFERS | пересчёт тумана |
| `0x6F00F880` | DIFFERS | пересчёт тумана |
| `0x6F00FAC0` | DIFFERS | пересчёт тумана |
| `0x6F759D60` | DIFFERS | обход ячеек тумана |
| `0x6F759B70` | DIFFERS | обход ячеек тумана |
| `0x6F747C20` | DIFFERS | допуск ячеек тумана |
| `0x6F7598C0` | DIFFERS | обновление ячеек тумана |
| `0x6F7599A0` | DIFFERS | обновление ячеек тумана |
| `0x6F7597D0` | DIFFERS | проверка соседних ячеек |
| `0x6F755B90` | DIFFERS | соседний R/S-тег |
| `0x6F746790` | DIFFERS | соседний тип |
| `0x6F747010` | DIFFERS | соседний флаг 8 |
| `0x6F750C00` | DIFFERS | плотность R/S-тегов |
| `0x6F74BB80` | DIFFERS | четыре тега клетки |
| `0x6F74BAE0` | DIFFERS | декодер тега клетки |
| `0x6F747110` | DIFFERS | кешированный предикат типа |
| `0x6F743C60` | DIFFERS | порядок соседних координат |
| `0x6F74B920` | DIFFERS | классификатор поверхности |
| `0x6F7526B0` | DIFFERS | проверка terrain obstruction |
| `0x6F27A3C0` | DIFFERS | счётчик вклада обзора: добавить |
| `0x6F27A400` | DIFFERS | счётчик вклада обзора: снять |
| `0x6F28DC80` | DIFFERS | снять вклад обзора юнита |
| `0x6F27A240` | DIFFERS | добавить вклад детекта |
| `0x6F27A2B0` | DIFFERS | снять вклад детекта |
| `0x6F27A430` | DIFFERS | direct override обзора |
| `0x6F26C240` | DIFFERS | rawcode getter детекта |
| `0x6F26C2C0` | DIFFERS | rawcode getter обзора |
| `0x6F274250` | DIFFERS | checked-slot Assign детекта |
| `0x6F2742D0` | DIFFERS | checked-slot Assign обзора |
| `0x6F27F040` | DIFFERS | checked-slot constructor детекта |
| `0x6F27F0A0` | DIFFERS | checked-slot constructor обзора |
| `0x6F284A50` | DIFFERS | снять вклад детекта юнита |
| `0x6F296680` | DIFFERS | добавить вклад детекта юнита |
| `0x6F296910` | DIFFERS | установить direct override обзора |
| `0x6F3DF190` | DIFFERS | mask C8 для получателя |
| `0x6F284650` | DIFFERS | recipient relevance gate |
| `0x6F284D30` | DIFFERS | direct relation mask recompute |
| `0x6F284830` | DIFFERS | detection presentation refresh |
| `0x6F2AC3A0` | DIFFERS | detection change event |
| `0x6F4D3530` | DIFFERS | presentation helper |
| `0x6F278F10` | DIFFERS | presentation helper |
| `0x6F4D3540` | DIFFERS | presentation helper |
| `0x6F27A200` | DIFFERS | presentation helper |
| `0x6F2AB3B0` | DIFFERS | notification observer callback |
| `0x6F2967F0` | DIFFERS | shared-vision contribution add |
| `0x6F284CD0` | DIFFERS | fog rescan recipient gate |
| `0x6F28DB90` | DIFFERS | fog rescan notification worker; прежний собственный THUNK заменён |
| `0x6F506CE0` | DIFFERS | material detection reset; COW и слои |
| `0x6F506DC0` | DIFFERS | material detection final; COW и слои |
| `0x6F3A5DC0` | THUNK | зависимость |
| `0x6F752570` | THUNK | зависимость |

При независимой сверке `0x6F755B90` с ASM была исправлена инверсия
ветки плотных R/S-тегов. Компиляция пройдена, штатная проверка совпадения
и игровой запуск остаются открытыми.

Notification/presentation блок опубликован коммитом `b2785e886`, включая
три data symbols без угаданных значений. Shared-vision/rescan блок
опубликован коммитом `1fefaadfe`: два прежде свободных TODO адреса
получили C++-тела, собственный `0x6F28DB90` сменил THUNK на DIFFERS.
Material notification pair опубликована коммитом `f87c63c4d`.
Эти ветви остаются без штатного verify и игрового прогона.

## Формирование исходящих приказов — [PR #53](https://github.com/FilippTheBestDev/claudecraft/pull/53)

Владелец: `codex/sim-order-flag-builders`. В опубликованной ветке 55 новых
C++ тел: 46 для приказов/selection и девять для replay chunk/codec. Они
прошли ограниченную проверку исходника и native-компиляцию, но не полную
проверку совпадения.

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F339CC0` | DIFFERS | флаги приказа |
| `0x6F339D50` | DIFFERS | флаги приказа |
| `0x6F339DD0` | DIFFERS | флаги приказа |
| `0x6F339E60` | DIFFERS | флаги приказа |
| `0x6F339F00` | DIFFERS | флаги приказа |
| `0x6F339F80` | DIFFERS | флаги приказа |
| `0x6F33A010` | DIFFERS | флаги приказа |
| `0x6F2CBA00` | DIFFERS | кодирование приказа |
| `0x6F2CBC10` | DIFFERS | кодирование приказа |
| `0x6F2CBD00` | DIFFERS | кодирование приказа |
| `0x6F2CBE20` | DIFFERS | кодирование приказа |
| `0x6F2CBF10` | DIFFERS | кодирование приказа |
| `0x6F2CC050` | DIFFERS | кодирование приказа |
| `0x6F2CBAD0` | DIFFERS | кодирование приказа |

Следующий связный слой опубликован двумя осмысленными коммитами PR #53:
`9665dadbf` (writers) и `7b5a9b239` (serializers). Его 11 адресов имеют
собственные C++-тела; штатная проверка совпадения остаётся открытой:

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F2C9890` | DIFFERS | сериализатор команды |
| `0x6F2C9950` | DIFFERS | сериализатор команды |
| `0x6F2C9A10` | DIFFERS | сериализатор команды |
| `0x6F2C9AD0` | DIFFERS | сериализатор команды |
| `0x6F2C9B90` | DIFFERS | сериализатор команды |
| `0x6F553EB0` | DIFFERS | запись полей команды |
| `0x6F553F30` | DIFFERS | запись полей команды |
| `0x6F553F90` | DIFFERS | запись полей команды |
| `0x6F553FF0` | DIFFERS | запись полей команды |
| `0x6F554050` | DIFFERS | запись полей команды |
| `0x6F554100` | DIFFERS | запись полей команды |
| `0x6F54D970` | DIFFERS | допуск команды к локальной очереди |
| `0x6F54D930` | DIFFERS | flush очереди |

Слой special-session fallback опубликован коммитом `6568d4b3b`, с
ограничением по точной vtable S11 и непроверенной живой длине payload:

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F54D760` | DIFFERS | special-session fallback |
| `0x6F53E080` | DIFFERS | слот отправителя → байт |
| `0x6F6522D0` | DIFFERS | запись длины и payload |

Subtype `0xA0010` опубликован коммитом `d30c7ea45`:

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F339C60` | DIFFERS | basic order wrapper |
| `0x6F2CB940` | DIFFERS | basic order producer |
| `0x6F2C97D0` | DIFFERS | basic order serializer |

Selection/control-group serializer и writer опубликованы коммитом
`5a8fe74ef`. Все девять адресов до него были TODO без старых C++ тел.
`0x6F5542D0` уже EXACT и остаётся за границей блока.

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F2C9C50` | DIFFERS | selection/control-group serializer |
| `0x6F2C9D10` | DIFFERS | selection/control-group serializer |
| `0x6F2C9DD0` | DIFFERS | selection/control-group serializer |
| `0x6F2C9E90` | DIFFERS | selection/control-group serializer |
| `0x6F2C9F50` | DIFFERS | selection/control-group serializer |
| `0x6F554160` | DIFFERS | selection/control-group writer |
| `0x6F5541C0` | DIFFERS | selection/control-group writer |
| `0x6F554220` | DIFFERS | selection/control-group writer |
| `0x6F554270` | DIFFERS | selection/control-group writer |

Четыре selection/control-group producers опубликованы коммитом `57616668f`;
до claim все были TODO без C++ тел. Уже существующие THUNK
`0x6F2CC240`/`0x6F2CC2E0` не входят в этот блок.

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F2CF5A0` | DIFFERS | selection modify producer |
| `0x6F2CF6E0` | DIFFERS | selection modify producer |
| `0x6F2CF7D0` | DIFFERS | control-group define producer |
| `0x6F2CC1C0` | DIFFERS | control-group select producer |

Replay chunk/codec опубликован коммитом `57e1be6cc` и описан в
[FND-0080](findings/network/FND-0080-replay-payload-chunk-route.md).
Четыре S11 lookup data records не являются переносимыми таблицами для
другой сборки; внешний ctor `0x6F543D90` принадлежит отдельному claim.

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F549450` | DIFFERS | replay chunk pump |
| `0x6F6562F0` | DIFFERS | replay payload encoder |
| `0x6F6563A0` | DIFFERS | replay payload decoder |
| `0x6F5482A0` | DIFFERS | compressed replay serializer |
| `0x6F548350` | DIFFERS | uncompressed replay serializer |
| `0x6F548400` | DIFFERS | replay-done serializer |
| `0x6F554690` | DIFFERS | compressed replay writer |
| `0x6F554650` | DIFFERS | uncompressed replay writer |
| `0x6F554560` | DIFFERS | replay-done writer |

## Приём входящих приказов — [PR #54](https://github.com/FilippTheBestDev/claudecraft/pull/54)

Владелец: `codex/inbound-order-callbacks`. Пять крупных обработчиков и
шестнадцать вспомогательных C++ тел опубликованы в PR #54. QA исправил
порядок освобождения ссылки (`e21aecc44`) и сверил обе fogged ветви;
штатная проверка совпадения остаётся открытой.

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F2CD020` | DIFFERS | входящий callback приказа |
| `0x6F2CD4E0` | DIFFERS | входящий callback приказа |
| `0x6F2CDA40` | DIFFERS | входящий callback приказа |
| `0x6F2CDFF0` | DIFFERS | входящий callback приказа |
| `0x6F2CE550` | DIFFERS | входящий callback приказа |

| Адрес | Статус | Участок |
|---|---|---|
| `0x6F2C91A0` | TODO в store; C++ опубликован, штатный verify недоступен | сканирующий callback |
| `0x6F2C9140` | DIFFERS | comparator callback |
| `0x6F2CAA70` | DIFFERS | маршрутизация приказа |
| `0x6F2CAB80` | DIFFERS | применение приказа |
| `0x6F2CB4F0` | DIFFERS | fogged candidate scan |
| `0x6F2CB710` | DIFFERS | extended fogged candidate scan |
| `0x6F2798D0` | DIFFERS | fogged ability-chain admission |
| `0x6F279470` | DIFFERS | queued-order count |
| `0x6F47B780` | DIFFERS | reject-path request cleanup entry |
| `0x6F47B5B0` | DIFFERS | unregister resolved request |
| `0x6F491650` | DIFFERS | find resolved object in 12-slot holder |
| `0x6F491FB0` | DIFFERS | clear resolved object from holder |
| `0x6F285D10` | DIFFERS | local inbound sender eligibility |
| `0x6F3A37F0` | DIFFERS | sender visible relation bit |
| `0x6F3A36D0` | DIFFERS | sender enemy relation bit |
| `0x6F026870` | DIFFERS | Aall tag accessor |

Эти PR — параллельная работа по движку. Статусы не сообщают, что код уже
пригоден для игры, прошёл полный матч или устраняет утечку скрытого состояния.

# FND-0075 — special-session flush оборачивает остаток приказа в запись `0x1F`

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, command-store, special-session, sender-slot, record, replay |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | correctness, compatibility, security |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; ветвь `0x6F54D760` из flush gate `0x6F54D930`, helpers `0x6F53E080` и `0x6F6522D0`. Установлены статические условия и форма локальной записи; полная доставка peer, авторизация отправителя и игровой результат не установлены. Fingerprint DLL и исполнение неизвестны. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0074](FND-0074-order-command-store-admission.md), [FND-0066 в PR #3](https://github.com/DinCrAsh-code/war3-client-server/blob/83bc16bee5252f67c8ae22d913777d30407f2f10/research/reverse/findings/network/FND-0066-online-turn-store-flush-boundary.md) |

## Вход и условия

[FND-0074](FND-0074-order-command-store-admission.md) показывает, что
`0x6F54D930` переводит теги `LOOP` и `NONE` в
[`0x6F54D760`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54D760.json).
Эта ветвь дополняет normal online flush, ограниченный в
[FND-0066](https://github.com/DinCrAsh-code/war3-client-server/blob/83bc16bee5252f67c8ae22d913777d30407f2f10/research/reverse/findings/network/FND-0066-online-turn-store-flush-boundary.md).
Функция выбирает активную запись `session+0x08+index*0x304`.
Если unsigned счётчик этой записи по относительному `+0x2E4`
(`session+0x2EC+index*0x304`) больше нуля, она выходит. Ещё один
нулевой gate читает `session+0xFD4+(index==0 ? 0x3FC : 0)`;
семантика этого флага по данной цепи не установлена.

Через vtable-слот `+0x24` очереди `session+0x1C78` запрашиваются
указатель и размер. Если размер не превышает `readPos` из
`session+0x2244`, функция выходит без записи. Иначе она создаёт
локальный `CDataStore` с буфером от `0x6F650D40` и сначала пишет
нулевое 16-битное слово через `0x6F6516B0`.

## Содержимое записи и передача дальше

Из активной записи берутся local slot `+0x2DC` и таблица `+0x290`.
[`0x6F53E080`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F53E080.json)
превращает входной байт `0xFE` в `0xFF`; для остальных значений
вызывает `0x6F53E050` и возвращает байт найденной записи `+0x18`,
либо ноль при отсутствии записи. Следовательно, сама эта функция
возвращает лишь локально сопоставленный байт; привязку к сетевому
peer она не удостоверяет.

Следующие два байта — младшие 16 бит разности `size-readPos`.
После них копируется ровно столько байт из `source+readPos`.
[`0x6F6522D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F6522D0.json)
записывает эту тройку как byte, word и raw payload в локальный store.
Исходная функция закрывает очередь через vtable-слот `+0x1C`, затем
зовёт `0x6F54CC10` с id `0x1F`, адресом store и флагом `2`.
`0x6F54CC10` далее читает store через его vtable `+0x28`; этот вызов
нельзя считать доказательством удалённой доставки без следующей цепи.

## Контроли и границы

Положительный статический контроль: нулевой счётчик, нулевой gate и
`size>readPos` доводят остаток очереди до записи `0x1F`.
Отрицательные ветви при ненулевом счётчике, ненулевом gate или
`size<=readPos` не создают record и не вызывают `0x6F54CC10`.
Отдельно `0xFE` и неизвестный слот дают разные байты (`0xFF` и `0`).

Размер копирования приводится к 16 битам перед `memcpy` в локальный
буфер; этот статический разбор не доказывает безопасность произвольной
длины или поведение при переполнении. В оригинале локальный
`CDataStore` использует vtable точной сборки, поэтому C++-реконструкции
недостаточно одной компиляции для безопасного исполнения этой ветви.
Следующая проверка — проследить `0x6F54CC10` до адресации получателя и
сопоставить двухигроковый input, локальную запись и фактическую выдачу.

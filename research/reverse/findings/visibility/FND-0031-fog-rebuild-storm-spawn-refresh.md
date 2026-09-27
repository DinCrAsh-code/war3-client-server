# FND-0031 — условный выход fog rebuild обновляет Storm- и CSpawn-флаги

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, rebuild, presentation, spawn, terrain, query, network-boundary |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [rebuild `0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json), [Storm option `0x6F00FAC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F00FAC0.json), [option gate `0x6F765240`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F765240.json), [refresh `0x6F016CD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F016CD0.json), [Storm latch `0x6F740940`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F740940.json), [пять групп `0x6F012640`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F012640.json), [group refresh `0x6F012350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F012350.json), [CSpawn accessor `0x6F015930`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F015930.json), [CSpawn refresh `0x6F012210`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F012210.json), [координаты `0x6F408170`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F408170.json), [fog query `0x6F00E830`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F00E830.json) |

## Вывод

После обязательного локального terrain-выхода
[FND-0029](FND-0029-fog-rebuild-presentation-output.md) полный rebuild
`0x6F40A8F0` выполняет **ещё одну ветвь только при**
`(dword_6FAB6A34 & 3) == 0`. Та же проверка раньше управляла
`memset(fogTable+0x30)` — [FND-0024](FND-0024-alliance-fog-rebuild.md).
При ненулевых младших битах global функция возвращается после terrain
выхода, минуя обе функции `0x6F00FAC0` и `0x6F016CD0`.

При прохождении gate выбор аргумента для `0x6F00FAC0` задают поля
таблицы: `1` тогда и только тогда, когда `(fogTable+0x24 & 1) == 0` и
хотя бы одно из `+0x10/+0x14` ненулевое; иначе `0`. Эта функция
соответственно выставляет или снимает бит `0x200` в поле Storm singleton
`+0x970`, затем вызывает `0x6F765240(0x200)`. В прочитанном теле
последний вызов с `0x200` попадает в ветвь немедленного возврата:
отдельные действия предусмотрены для `0x2`, `0x4`, `0x80` и `0x1000`.
Само изменение
`+0x970` при этом сохраняется. Бит `0x200` нельзя смешивать с
независимым битом `0x2000`, которым ниже разрешён обход пяти групп.

Затем `0x6F016CD0(fogTable)` передаёт указатель на таблицу в Storm
singleton через `0x6F740940`: поле singleton `+0x99C` получает этот
указатель, а каждому существующему элементу массива `+0x104`
(счётчик `+0x100`, шаг `0x94`) устанавливается бит `0x8` в слове
`element+0x4C`. Это dirty-пометка существующих записей, без копирования
плоскостей fog в сетевой пакет.

Если `fogTable` ненулевой, `0x6F016CD0` вызывает два дальнейших пути:

1. `0x6F012640` **только при** `Storm+0x970 & 0x2000` проходит пять
   объектов `0x6F012430` и вызывает `0x6F012350(group, fogTable)`.
   Для допущенных элементов массива группы (`+0x48` count, `+0x4C`
   pointer, шаг `0x50`) `0x6F012350` выставляет или снимает бит `0x1`
   в записи по результату `0x6F00E830(fogTable, cell, mask)`:
   результат `4` выставляет бит, другой результат снимает. Маска —
   `word(fogTable+0x3C)`. Допуск включает ненулевые старшие биты поля
   `element+0x4C` и бит `0x2` начального поля элемента.
2. `0x6F0159A0` получает ленивый объект с vtable `CSpawn` через
   `0x6F015930` и вызывает `0x6F012210(CSpawn, fogTable)`.
   Для элементов `CSpawn+0x0C` (count `+0x08`, шаг `0x3C`) с ненулевым
   первым полем и битом `0x1` в `element+0x38` координаты из
   `element+0x28/+0x2C` переводятся в клетку через `0x6F408170`.
   `0x6F00E830` читает **игровые** fog-плоскости `+0x2C/+0x30` и
   возвращает код ответа [FND-0018](FND-0018-fog-grid-result-table.md).
   Код `4` выставляет бит `0x4` в `element+0x38`, другой — снимает.

Это два статически прослеженных обновления **локальных записей** после
rebuild, а не отправка информации клиенту. Ни одна из показанных функций
не сериализует сетевое сообщение и не вызывает транспорт напрямую.

## Основание и контроли

Положительный статический контроль: при global low bits `0`, ненулевом
`fogTable`, ненулевом счётчике `CSpawn` и элементе, прошедшем фильтры,
`0x6F012210` вызывает `0x6F00E830` и обновляет `element+0x38 & 0x4`
по сравнению результата с `4`. Для пяти групп нужен отдельный
положительный gate `Storm+0x970 & 0x2000`; установленный ранее бит
`0x200` его не заменяет.

Отрицательные контроли: global low bits `!=0` обходят весь этот выход,
даже если terrain-выход уже выполнен. Нулевой указатель `fogTable` в
`0x6F016CD0` ограничивает путь latch/dirty и не вызывает оба обхода.
При `fogTable+0x24 & 1` оба клеточных обновителя ставят соответствующий
бит `0x1` или `0x4` **без fog query** для допущенных записей. При
нулевом количестве CSpawn-записей `0x6F012210` ничего в них не меняет;
при выключенном `Storm+0x970 & 0x2000` не вызывается обход пяти групп.

## Границы для клиент-серверного проекта

Это только чтение закреплённых IDA-тел, без исполнения DLL и без
наблюдения сети. Содержимое CSpawn-записей, смысл high bits у пяти
групп, момент экранной перерисовки и возможные последующие потребители
установленных флагов остаются открытыми. Нельзя вывести из этих локальных
флагов право на выдачу скрытого состояния: после отзыва обзора нужно
отдельно проверить изменение плоскостей, результат query, фактические
данные у удалённого игрока и порядок фаз. Адреса не переносимы на
runtime без fingerprint DLL и независимой проверки.

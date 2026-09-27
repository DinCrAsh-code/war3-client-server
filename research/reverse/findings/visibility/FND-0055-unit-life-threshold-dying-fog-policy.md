# FND-0055 — порог жизни переводит юнит в dying-состояние, которое fog допускает к расчёту

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, life, death, listener, fog, reveal-radius |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен. Адреса относятся только к этой выгрузке, DLL не запускалась. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [CUnit `0x6F277370`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F277370.json), [Float vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F877BA8.json), [listener creation `0x6F477550`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F477550.json), [listener init `0x6F480FB0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F480FB0.json), [listener bind `0x6F480E90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F480E90.json), [relation constructor `0x6F48B420`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F48B420.json), [threshold relation vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F952B6C.json), [range recipient `0x6F497510`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F497510.json), [threshold notify `0x6F4A9D80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A9D80.json), [crossing `0x6F4A9CF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4A9CF0.json), [endpoint router `0x6F47FD00`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F47FD00.json), [agent dispatcher `0x6F472AE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F472AE0.json), [relation event handler `0x6F472040`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F472040.json), [observer invoke `0x6F62A5D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A5D0.json), [CUnit dispatcher `0x6F2A7E60`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A7E60.json), [dying handler `0x6F2A7D80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A7D80.json), [fog rebuild `0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json), [radius calculation `0x6F29F040`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F29F040.json) |

## Регистрируемый маршрут жизни

При инициализации CUnit `0x6F277370` только если `unit+0xA8 == 0`
создаёт слушатель для tracked life ref `unit+0x98`: граница
`flt_6FAAE4C4`, направление `0`, message ID `0xD019F`, target — тот
же CUnit, mode `0`. Возвращённый counted `FloatListener` сохраняется
в `unit+0xA8`. Числовое значение границы S11 здесь не раскрывает;
название `g_itemValueFloor` в S10 не заменяет чтение DLL data.

У `Float` vtable `0x6F877BA8` слот `+0x14` —
[`0x6F478820`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F478820.json): он
берёт owner из разрешённого life range `+0x30` через `0x6F478900`,
затем agent `owner+0x54`. `0x6F477550` передаёт полученный subject,
life ref и параметры в `0x6F480FB0`. Тот через `0x6F480E90`
создаёт relation по тегам `0x5E6C6973`/`0x6072746C`, пишет ID `0xD019F` в
`relation+0x48`, прикрепляет `CPoReThresholdLis` к life range через
`0x6F47AD60 → 0x6F4A6BC0 → 0x6F4A5BE0` и задаёт границу через
`0x6F480F80`. Вставка использует `range+0x60` как голову списка;
первый node оказывается в `range+0x64`. Конструктор
`0x6F48B420` кладёт собственный указатель в `relation+0x3C`,
поэтому `0x6F497510` при обходе `range+0x64` действительно вызывает
relation vtable `+0x20` (`0x6F4A9D80`).

Независимо от прикрепления к range, `0x6F480FB0` регистрирует
observer: [`0x6F471A30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F471A30.json)
перебрасывает вызов в embedded CObserver `subject+0x14`, slot
`+0x08` (`0x6F62A9A0 →`
[`0x6F62A820`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A820.json)). Его
три аргумента — handle relation, ID `0xD019F` и target CUnit.
Так замыкается ранее неизвестный конкретный получатель life range
из [FND-0036](FND-0036-killunit-life-notification-death-boundary.md).

## Пороговое событие и CUnit

При life update из FND-0036 событие с тегами
`0x5E70726F/0x60726C64` попадает в
`CPoReThresholdLis::Notify` `0x6F4A9D80`. Только если объект range
разрешён, `range+0x4F & 1 == 0`, тег события совпал и сравнение
`0x6F4A9940` признало crossing, `0x6F4A9CF0` формирует
событие с тегами `0x5E6C6973/0x6072746C` и вызывает endpoint A
slot `+0x20`. Для `CAgentBaseAbs` этот слот в
[S11 vtable `0x6F9520D4`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F9520D4.json) —
`0x6F47FD00`: он допускает код `0x6072746C` и вызывает agent из
`endpointA+0x54`, slot `+0x18` (`0x6F472AE0`). Его ветвь
`0x6072746C` сначала получает **исходный relation pointer** из
event context `+0x0C` через `0x6F4A5EC0`, затем передаёт его в
`0x6F472040`. Последний считывает `relation+0x48`,
публикует event через `0x6F471FD0` и embedded CObserver.
В `0x6F62A5D0` совпавшая запись по relation handle кладёт
зарегистрированный ID `0xD019F` в event `+0x08` и вызывает
target slot `+0x0C`. В CUnit vtable это dispatcher `0x6F2A7E60`;
его ветвь `0xD019F` вызывает `0x6F2A7D80`. Этот маршрут требует
сохранённых relation/observer registrations и конкретного
`CAgentBaseAbs` endpoint; непрослеженный иного типа endpoint нельзя
считать тем же маршрутом.

`0x6F2A7D80` сначала выходит, если `unit+0x5C & 0x100` уже стоит.
Если `0x6F076950` возвращает неноль, он снимает бит `0x2000`,
сбрасывает `unit+0x250/+0x254` в `-1` и выходит, не ставя dying-биты.
При нуле он выполняет подготовку, ставит `unit+0x5C |= 0x100`,
вызывает CUnit vtable `+0xA0` (`0x6F285BC0`) с нулём, затем ставит
`unit+0x5C |= 0x20`, меняет связанные presentation/life поля и
отправляет события другим получателям. Повторная запись life=0 внутри
обработчика не превращает этот путь в независимый отзыв fog.

## Следствие для fog и контроли

Положительный контроль: при валидном unit, life range и listener,
несупрессированном пересечении порога, разрешённом endpoint A и
нулевом ответе `0x6F076950` цепь достигает установки обоих битов
`0x100|0x20`. В полном rebuild `0x6F40A8F0` юнит с битом `0x100`
исключается **только если** бит `0x20` отсутствует; пара из dying
handler проходит этот gate при соблюдении остальных фильтров.
Инкрементальный `0x6F40A650` использует тот же gate. Вычисление
радиуса `0x6F29F040` при бите `0x20` дополнительно читает
`Misc/DyingRevealRadius` и может изменить радиус. Поэтому смерть не
означает обязательное исчезновение источника обзора: движок имеет
отдельную политику обзора умирающего юнита.

Отрицательные контроли: `unit+0xA8 != 0` пропускает повторную
регистрацию; suppress flag range, несовпадающий event tag,
непересечённый threshold, отсутствие endpoint/observer записи не
достигают `0xD019F` по этому маршруту; уже установленный `0x100`
пропускает повторное dying-преобразование; ненулевой `0x6F076950`
не ставит пару dying-битов. Даже после установки битов другие фильтры
rebuild и итоговое значение `DyingRevealRadius` могут исключить
нанесение обзора. Очистка plane `fogTable+0x30` при rebuild отдельно
зависит от `dword_6FAB6A34 & 3 == 0` — [FND-0026](FND-0026-unit-fog-rebuild-trigger.md).

В прямом теле `0x6F2A7D80` нет вызова `RefreshUnitFog`
`0x6F40A650`, writer `0x6F409E00` или rebuild `0x6F40A8F0`.
Его virtual `+0xA0`, последующие helper calls и отправленные события
оставляют возможные косвенные effects; статический просмотр не
устанавливает, когда новое dying-правило попадает в fog plane и
локальный запрос раскрытия. `KillUnit` задаёт life=0, но range clamp
и threshold comparison не гарантируют crossing для любого исходного
состояния. S11 статусы ключевых звеньев смешаны (`THUNK`, `TODO`,
`DIFFERS`, `EXACT`); это проверка IDA-body, не выполнение Game.dll.
Следующий опыт: одиночный источник обзора с известным life range и
`DyingRevealRadius`; снять listener/observer registration, life до и
после `KillUnit`, флаги `+0x5C`, grid `+0x30`, результат fog query и
таймер `0x80269`. Повторить с супрессией listener, уже dying unit и
вторым независимым источником обзора.

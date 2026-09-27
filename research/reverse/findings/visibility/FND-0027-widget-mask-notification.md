# FND-0027 — снятие битов виджета условно уведомляет GameUI

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | widget, player-mask, ui, presentation, revocation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [vtable CWidget](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F92E7BC.json), [vtable CUnit](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [вход `+0x98`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD7E0.json), [снятие `+0x110`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AF6A0.json), [GameUI gate](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F333520.json), [sink](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F39A3F0.json), [обработка агента](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F38EA30.json) |

## Вывод

У CWidget и CUnit виртуальный вход `+0x98` ведёт в `0x6F2AD7E0`.
Он берёт 16-битные маски существующих игроков из таблицы игроков
`+0x2C`, виджета `+0x2C` и виджета `+0x2E`. После проверки
пересечения двух масок виджета он передаёт `live = existing & widget+0x2C`
и `notify = live & widget+0x2E` в виртуальный фильтр `+0xF8`.
При положительном ответе и непустом `notify` виртуальный `+0xEC`
возвращает индекс владельца; если он неотрицателен, бит владельца
исключается. Только при оставшейся ненулевой маске вызывается
виртуальный `+0x110(mask, 0)`; в обеих названных vtable это
`0x6F2AF6A0`.

`0x6F2AF6A0` создаёт агент уведомления типа `hgw+`, передаёт его
виртуальному `+0x60` виджет, мировую позицию и выбранную маску,
затем выполняет **16-битное** `widget+0x2C &= ~mask`.
Виджет `+0x2E` эта запись не меняет. После записи вызывается
`0x6F333520(agent)`: `GetGameUI(0, 0)` не создаёт отсутствующий UI;
при нулевом результате путь уведомления здесь заканчивается, но
`widget+0x2C` уже изменён. При наличии GameUI берётся sink из
`GameUI+0x3BC` и вызывается `0x6F39A3F0`.

Sink выделяет slot в **своём** growable array `+0x650` через
[`0x6F2AEE80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AEE80.json)
и передаёт агента в type-checked
[`Assign` `0x6F085B50`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F085B50.json),
который при отказе может сохранить в slot ноль. Затем sink синхронно
вызывает `0x6F38EA30`. Последний пересчитывает по
позиции агента две 16-битные маски через `0x6F3A1460` и массив
игроков `sink+0x1F8` (алгоритм — [FND-0022](FND-0022-rebuild-player-masks.md)).
Результат второго выхода он записывает в агент `+0x4A` через
[`0x6F2C6210`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C6210.json);
результат первого выхода **добавляет OR**, а не присваивает, в агент
`+0x4C` через
[`0x6F2C6230`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C6230.json).
После этого [`0x6F2C6220`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2C6220.json)
читает `+0x4C`: если у `new(+0x4A) & ~accumulated(+0x4C)`
нет младших 16 бит, вызывается виртуальный метод агента `+0x5C`.
Иначе путь продолжается в условную обработку представления через
`0x6F2C64D0` и другие helpers. Назначение `+0x5C` для всех
конкретных типов агента и конечный экранный эффект здесь не установлены.

## Основание и контроли

В S11 видны реальные виртуальные caller'ы `+0x98`: установка модели
по handle [`0x6F26B9E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26B9E0.json)
вызывает его лишь при ненулевых handle и аргументе `notify`;
[`0x6F26BA90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26BA90.json)
и [`0x6F26BAC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26BAC0.json)
при ненулевом `notify` анимации; CUnit
[`0x6F291790`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F291790.json)
вызывает его без такой проверки перед обновлением pending state.
Это положительный контроль достижимости виртуального входа из
конкретных тел S11, но не доказательство, что его вызывает отзыв
fog или detection.

Положительный контроль снятия битов: непустое пересечение трёх масок,
положительный `+0xF8` и оставшийся после исключения владельца бит
доводят его до `and [widget+0x2C], ~mask`; при существующем GameUI
тот же вызов доходит до записи масок агента уведомления. Отрицательные
контроли: пустое пересечение, отказ `+0xF8` либо единственный бит
владельца не вызывают `+0x110`; отсутствующий GameUI пропускает sink,
но не отменяет снятие `widget+0x2C`.

Отдельная проверка против ложной связи со спрайтом: [локальный predicate
`0x6F2AD5D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD5D0.json)
проверяет бит `0x8000` **другого** слова `widget+0x2E`.
[`CUnit::RefreshSpriteVisibility` `0x6F26E350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26E350.json)
читает этот predicate и ещё учитывает флаги image/юнита.
В показанной цепи `+0x110 → GameUI sink` прямого вызова
`RefreshSpriteVisibility` нет. Следовательно, снятие бита `+0x2C`
само по себе не доказывает немедленное скрытие спрайта.

## Пределы и значение для проекта

Статусы S11: `0x6F2AD7E0`, `0x6F2AF6A0`, `0x6F39A3F0` и
`0x6F26E350` — `DIFFERS`; `0x6F333520`, `0x6F2C6210/6220/6230`
и `0x6F2AD5D0` — `EXACT`; `0x6F38EA30` — `THUNK` в реконструкции,
хотя его IDA-тело присутствует. Собственного исполнения, проверки
изображения и сравнения с исходной DLL не было. Не прослежены все
виртуальные caller'ы `+0x98`, все конкретные агенты и конечные
действия UI helpers. Прямая связь этого уведомления с изменением
счётчиков детекта, fog grid или с сетевой выдачей не установлена.

Для клиент-серверной архитектуры это отдельная граница публикации:
слово виджета, сохранённый агент и экранный predicate живут на
разных путях. Следующий связанный опыт — вызвать штатное изменение
маски для двух игроков с одним владельцем и одним сторонним
наблюдателем, сравнить `widget+0x2C/+0x2E`, агент `+0x4A/+0x4C`,
наличие GameUI и фактический спрайт; отрицательный контроль —
отсутствующий GameUI и маска только владельца.

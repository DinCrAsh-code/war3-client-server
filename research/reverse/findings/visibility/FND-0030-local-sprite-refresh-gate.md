# FND-0030 — рескан fog обновляет спрайт юнита при смене локального бита

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, unit, destructable, widget, sprite, presentation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [писатель `0x6F2AB710`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AB710.json), [fog setter `+0x2C`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F407DD0.json), [fog setter `+0x30`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F407E60.json), [рескан `0x6F392E70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F392E70.json), [destructable callback](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F38C5E0.json), [unit callback](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F392B70.json), [grid query](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A1520.json), [CUnit vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F931934.json), [CDestructable vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F92EAF4.json) |

## Вывод

Прямой статический маршрут локального обновления после **изменения
режима fog** найден. `0x6F407DD0` записывает `fogTable+0x10` и при
отдельном условии очищает плоскость `+0x2C`; `0x6F407E60` записывает
`fogTable+0x14` и при нулевом значении очищает плоскость `+0x30`.
Оба вызывают terrain output `0x6F406CC0`, затем получают GameUI через
`GetGameUI(1, 0)` и передают `word(playerTable+0x28)` в
`0x6F392E70` у sink `GameUI+0x3BC`. Этот вызов следует за записью
fog-поля и не зависит от поздних проверок `fogTable+0x24`.

`0x6F392E70` сначала пересчитывает массив масок `sink+0x1F8` через
`0x6F38DEA0`, потом обходит widget agents. Если его аргумент **не**
равен sentinel `0x10`, он дополнительно обходит два разных типа:

| Тип обхода | Источник type id | Callback | Виджетный слот `+0xF4` |
|---|---|---|---|
| Destructables | [`0x6F266160`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F266160.json) | `0x6F38C5E0` | Для CDestructable `0x6F266300` |
| Units | [`0x6F26C1C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26C1C0.json) | `0x6F392B70` | Для CUnit `0x6F26E350` |

Оба callback берут позицию объекта и через `0x6F3A1520` читают
fog grid `+0x2C/+0x30` с учётом массива `sink+0x1F8`. Первый выход
сохраняется через OR в 16-битном `widget+0x2C`; **второй** подаётся
в `0x6F2AB710` вместе с **другим** словом `sink+0x1F4`. Последнее
служит аргументом маски локального игрока для этого вычисления;
его нельзя смешивать ни с двумя выходами grid query, ни с
`widget+0x2E`.

`0x6F2AB710` сохраняет старое 16-битное `widget+0x2E`. Пусть `Q` —
второй выход `0x6F3A1520`, `L = word(sink+0x1F4)`. Новое слово
вычисляется статически как
`(Q & 0x0FFF) | word_6FA73A94[((Q & 0x0FFF) & L) != 0]`.
Значения **двух слов таблицы `word_6FA73A94[0/1]` в S11 не выгружены**,
поэтому этот вывод не подменяет их предполагаемыми `0/0x8000`.
После записи функция проверяет `(old ^ new) & 0x8000`.
**Только при смене старшего бита** она вызывает виртуальный `+0xF4`,
затем `+0xB4`, и возвращает `1`; иначе возвращает `0` без этих вызовов.
Для динамического типа CUnit `+0xF4` —
[`CUnit::RefreshSpriteVisibility` `0x6F26E350`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26E350.json).
Для CDestructable это **другой** override `0x6F266300`, поэтому
destructable callback не доказывает вызов CUnit-метода.
Отдельный путь снятия `widget+0x2C` и уведомления через GameUI описан
в [FND-0027](FND-0027-widget-mask-notification.md); его нельзя
подменять этим писателем `widget+0x2E`.

Unit callback `0x6F392B70` имеет ещё один отдельный вызов `+0xF4`
после `0x6F2AB710`, если
[`IsGameModeOne` `0x6F53F160`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F53F160.json)
вернул истину. В этом режиме локальный refresh возможен **даже без
смены** бита `0x8000`. Сам `0x6F26E350` далее читает predicate по
`widget+0x2E`, флаг принудительной видимости `image+0x24` и другие
условия перед вызовом setter спрайта
[`0x6F26B820`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F26B820.json).
Вызов refresh сам по себе не равен скрытию спрайта.

## Контроли и отвергнутые короткие выводы

Положительный статический контроль: при аргументе рескана, отличном
от `0x10`, и юните, дошедшем до callback, путь
`fog setter → 0x6F392E70 → unit type enumeration → 0x6F392B70 →
0x6F3A1520 → 0x6F2AB710` установлен прямыми вызовами и указателем
callback. Если табличное новое слово меняет `0x8000`, CUnit vtable
доводит этот же путь до `0x6F26E350`.

Отрицательные контроли: аргумент `0x10` пропускает оба typed обхода;
`0x6F38C5E0` относится к destructables, а не units; изменение только
младших битов `widget+0x2E` не вызывает `+0xF4` внутри
`0x6F2AB710`. В игровом режиме 1 есть дополнительный вызов, поэтому
последнее правило относится только к **внутренней ветви писателя**.
Даже после входа в `CUnit::RefreshSpriteVisibility` forced-visible
флаг может сохранить спрайт видимым.
Кроме того, [`CWidget::Load` `0x6F2AC810`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AC810.json)
передаёт адрес `widget+0x2E` читателю сохранённого слова без вызова
`+0xF4` в собственном теле; значит не всякая запись этого поля
немедленно проходит через показанный refresh gate.

Полный fog rebuild [`0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json)
вызывает [`0x6F39B6E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F39B6E0.json)
с аргументом **0**. В `0x6F39B6E0` ветка обхода списка
`sink+0x600` через `0x6F39B160 → 0x6F2AB710` запускается лишь
при ненулевом аргументе; при нуле возвращается сам список.
В [`0x6F3A2920`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A2920.json)
есть вызов `0x6F39B6E0(1)`, но он стоит **перед** вызовом полного
rebuild в том же стартовом маршруте. Ни один из этих фактов не
доказывает rescan списка после `0x6F40A8F0`; другие его выходы
остаются отдельной границей.

## Пределы и значение для проекта

Это чтение IDA-тел без исполнения. `0x6F2AB710`, fog setters,
`0x6F392E70`, callbacks и `0x6F3A1520` имеют статус S11 `TODO`;
`CUnit::RefreshSpriteVisibility` — `DIFFERS`, sprite setter
`0x6F26B820` — `MISMATCH` в реконструкции. Точный DLL fingerprint,
содержимое таблицы `word_6FA73A94`, все косвенные caller'ы `+0xF4`,
штатные значения sentinel и конечное изображение не проверены.
Изменения detection/alliance могут влиять на входные mask и grid,
но прямой переход от их конкретного отзыва к этому рескану пока не
прослежен. Приписывать им немедленное скрытие CUnit нельзя.

С точки зрения клиент-серверной системы право выдать
скрытые сведения остаётся отдельным от локального sprite refresh.
Следующий опыт — в двухигроковой сцене отдельно переключить оба fog
mode setter и источник детекта, фиксируя `grid+0x2C/+0x30`,
`sink+0x1F4/+0x1F8`, `unit+0x2C/+0x2E`, вызовы `+0xF4` и спрайт.
Для отрицательного контроля нужен юнит с изменением младших битов
`+0x2E` без изменения `0x8000` и отдельный вызов с player sentinel
`0x10`. Происхождение входного mask для detection/alliance проверять
отдельно от этого presentation-пути.

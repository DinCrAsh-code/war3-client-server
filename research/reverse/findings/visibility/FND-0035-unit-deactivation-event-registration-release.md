# FND-0035 — `0xD01A1` при деактивации снимает регистрацию, а не вызывает обработчик

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, deactivation, observer, event, refcount, fog |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен; адреса — координаты S11. Сопоставлен S10 C++, но DLL и поведение не воспроизводились. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [CUnit deactivation `0x6F282920`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F282920.json), [event selector `0x6F2AB1E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AB1E0.json), [observer wrapper `0x6F62A570`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A570.json), [registration removal `0x6F62A000`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A000.json), [registration insert `0x6F62A820`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A820.json), [observer delivery `0x6F62A5D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A5D0.json), [S10 event selector](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Widget/widget_postagentevent.cpp), [S10 registration table](https://github.com/DinCrAsh-code/war3-client-server/blob/2fc76c8035554912dd66fc0a06a39eda376a806c/src/Agent/observereventreg.cpp) |

## Вывод

`CUnit::Deactivate` (`0x6F282920`) вызывает `0x6F2AB1E0(unit, 0)`.
При втором аргументе `0` функция кладёт `unit` и ключ `0xD01A1`
в аргументы `0x6F62A570`. Если у observer нет таблицы `+0x08`,
обёртка сразу возвращается; иначе она передаёт ключ и `unit`
в `0x6F62A000`. Эта функция хеширует ключ, идёт по соответствующему
bucket и ищет запись с тем же ключом и ресурсом, равным `unit`.
Совпавшая запись **снимается из таблицы**, а её counted reference
освобождается. Она не вызывает message handler ресурса на этом пути.

Названия `PostAgentEvent`, `PostEvent` и `Dispatch` в S10 и фраза
«delivery» в комментариях сами по себе не доказывают уведомление
слушателя. S11 различает операции по телам: при ненулевом втором
аргументе `0x6F2AB1E0` вызывает vtable `+0x08` CUnit, то есть
`0x6F62A9A0 → 0x6F62A820`; этот путь **создаёт или обновляет**
регистрацию `(key, resource)` и увеличивает счётчик ссылок ресурса.
Отдельный путь настоящей доставки сообщения — CUnit vtable `+0x14`
`0x6F62A7B0 → 0x6F62A5D0`: он вызывает у зарегистрированного
ресурса vtable `+0x0C`. `CUnit::Deactivate` в показанной цепи
к `0x6F62A5D0` не обращается.

Таким образом, поиск обработчиков, сравнивающих message ID `0xD01A1`,
не связывает их с этим конкретным вызовом при деактивации. В частности,
CUnit slot `+0x0C` (`0x6F2A7E60`) имеет case `0xD01A1`, который
переходит к `0x6F29DBC0`, но до этого slot путь
`0x6F282920 → 0x6F2AB1E0(unit,0) → 0x6F62A570 → 0x6F62A000`
не доходит. В прочитанных прямых ветвях снятия регистрации нет вызова
fog writer `0x6F409E00`, инкрементального refresh `0x6F40A650`
или полного rebuild `0x6F40A8F0`.

## Основание и контроли

Положительный контроль снятия: `0x6F62A000` сравнивает
`entry+0x04` с переданным ключом (`0x6F62A0CA`) и, если target
не ноль, `entry+0x08` с переданным `unit` (`0x6F62A0D7`).
При совпадении и нулевом флаге storage вызывает unlink
`0x6F629830` (`0x6F62A11B`), уменьшает refcount ресурса
и освобождает entry через `0x6F4C1B50`. При ненулевом storage
тоже уменьшает refcount, но обнуляет `entry+0x08` (`0x6F62A16D`),
оставляя саму запись в таблице. После чистого выхода возможен
`0x6F629750` (`0x6F62A1B0`), который уменьшает счётчик entries.

Отрицательные контроли: нулевой observer `+0x08` в `0x6F62A570`
не входит в таблицу; несовпадающий ключ или ненулевой target,
не равный `entry+0x08`, не снимают эту запись. `0x6F62A000`
не содержит перехода на vtable `+0x0C` ресурса. Напротив,
`0x6F62A5D0` явно вызывает `resource->vtable+0x0C`
(`0x6F62A6F5–0x6F62A6FB`). Это разные операции на одной таблице.

Есть важное исключение к выводу о прямых вызовах: освобождение
ресурса при переходе его refcount к нулю делает виртуальный вызов
`resource->vtable+0x00` (`0x6F62A12C–0x6F62A130` либо
`0x6F62A15F–0x6F62A163`). Для CUnit vtable slot 0 в S11 указывает
на `0x6F001F70` (`ReleaseSelf`). Из этих статических тел неизвестно,
может ли данный release быть последней ссылкой в реальном сценарии
и какие дальнейшие эффекты возникнут. Это не callback сообщения
`0xD01A1` и не доказанный fog revoke.

## Граница и следующий эксперимент

FND-0034 устанавливает spatial teardown в `Deactivate`, но данная
ветвь `0xD01A1` не добавляет доказанного снятия старого fog radius.
Это **точная граница одного маршрута**, а не доказательство отсутствия
fog cleanup при удалении юнита: остаются handle notifications,
`ReleaseSelf`, другие callers `+0x14` и timer/rebuild [FND-0026](FND-0026-unit-fog-rebuild-trigger.md).
Следующая проверка — в контролируемом матче пометить единственный
источник обзора, проследить `Deactivate` и refcount перед
`0x6F62A000`, затем сопоставить изменение старых fog-клеток
`+0x30`, вызовы `0x6F40A650`/`0x6F40A8F0` и маску
`dword_6FAB6A34 & 3`. Нужен также конкретный вход JASS `RemoveUnit`:
его связь с `Deactivate` пока не доказана.

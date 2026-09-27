# FND-0082 — target scanners не ограничивают число candidate rows

| Поле | Значение |
|---|---|
| Subsystem | simulation |
| Tags | order, target, selection, candidate, capacity, memory-safety |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; два callback `0x6F2CADC0/0x6F2CB190`, их caller `0x6F2CD4E0/0x6F2CDA40` и общий enumerator `0x6F421AF0`. Доказан локальный инкремент без проверки ёмкости, но достижимость 13 допущенных кандидатов и эффект в игре не установлены. Fingerprint DLL неизвестен. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0068](FND-0068-point-target-fogged-order-family.md); [C++ PR #54](https://github.com/FilippTheBestDev/claudecraft/pull/54) как вторичная реконструкция |

## Граница записи

Входящие `0xA0012/0xA0013` создают на стеке контекст с двенадцатью
строками по `0x24` байта. Для direct-target строки начинаются в
контексте с `+0x3C`, счётчик находится в `+0x38`; для two-target —
с `+0x44` при счётчике в `+0x40`. Это видно в caller
[`0x6F2CD4E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CD4E0.json)
и [`0x6F2CDA40`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CDA40.json).

Оба scanner callback —
[`0x6F2CADC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CADC0.json)
и [`0x6F2CB190`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2CB190.json)
— после прохождения sender/order gates читают счётчик, увеличивают его
на единицу и вычисляют строку как `base + oldCount*0x24`. Проверки
`oldCount < 12` между gate и записью нет. В обоих callback return равен
`1` и на принятой, и на отклонённой ветви.

Общий [`0x6F421AF0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F421AF0.json)
перебирает разрешённые identity из selection list, вызывает callback
для объекта с нужным tag и нулевым `entry+0x20`, прекращает обход при
нулевом ответе callback или конце списка. Число итераций берётся из
построенного списка; локальной проверки против 12 в этом enumerator
не видно. Ограничение возможно раньше при формировании selection и
должно быть проверено отдельно.

## Контроли и предел вывода

Положительный **условный** статический контроль: если enumerator передаст
тринадцать юнитов, которые проходят gates callback, запись для индекса
`12` окажется за объявленными двенадцатью строками. Отрицательные
ветви: отклонённый юнит не увеличивает счётчик; пустой selection list,
неразрешённая identity и callback с нулевым ответом прерывают или
пропускают путь. Показанные два callback всегда отвечают `1`, поэтому
последняя остановка для них сама по себе не ограничивает число строк.

Это не доказанный эксплойт и не результат исполнения. Не установлены
внешний предел selection, достижимость тринадцати проходящих юнитов,
содержимое switch table `0x6F2C9010`, эффект записи за массивом и
поведение иных order variants. Для нового авторитетного приёмника
ёмкость результата и допустимую selection нужно проверять явно,
сверяя при этом совместимость с обычными картами и командами.

# FND-0043 — GetLocalPlayer выбирает слот по режиму и возвращает handle token

| Поле | Значение |
|---|---|
| Subsystem | jass |
| Tags | local-context, player, game-mode, handles, compatibility |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | compatibility, correctness, security |
| Scope | S11, адреса IDA предполагаемой Game.dll 1.26a/build 6401 x86; fingerprint образа и поведение запущенной DLL не проверены |
| Sources | [S11](../../SOURCES.md#s11-адресный-корпус-ida-коллеги), commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10](../../SOURCES.md#s10), commit `2fc76c8035554912dd66fc0a06a39eda376a806c` |

## Вывод

В [адресной записи GetLocalPlayer](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3BBB60.json)
натив не возвращает напрямую индекс игрока. При отсутствии таблицы
`dword_6FAB65F4` он возвращает нуль. Иначе вызывает `0x6F53F160`,
выбирает слово таблицы `+0x2A` при результате 1 или `+0x28` при 0,
получает соответствующий объект из массива `+0x58` через
`0x6F3A1650`, затем передаёт объект с аргументом 0 в `0x6F430C80`
через результат `0x6F3A8060`. Возвращаемое значение последнего вызова
становится результатом JASS native. Его регистрация как
`GetLocalPlayer` с сигнатурой `()Hplayer;` есть в
[S10](../../../../src/Jass/jassregisterallnatives.cpp).

Это даёт проверяемую границу локальной идентичности: два поля выбора
игрока и преобразование объекта в handle token, причём выбор зависит от
режима игры. Значение `+0x2A` нельзя без опыта объявлять тем же
«локальным игроком» во всех режимах.

## Основание и контроли

- [Режим](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F53F160.json)
  сравнивает слово `+0x610` блока из TLS slot 13 с 1; S10
  [IsGameModeOne](../../../../src/Game/gamemode.cpp) восстанавливает эту
  узкую проверку. Другое значение режима ведёт к полю `+0x28`.
- [Индексный доступ](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A1650.json)
  читает указатель по `table + 0x58 + index*4`. S10
  [playercolor](../../../../src/Widget/playercolor.cpp) использует тот же
  accessor для игрока. S10
  [локальная проверка владельца](../../../../src/Widget/cunit_agent3_islocallyowned.cpp)
  отдельно сравнивает владельца юнита с полем `+0x28`.
- [Реестр](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F430C80.json)
  в S10 восстановлен как [Register](../../../../src/Agent/agentregistry.cpp):
  его результат — slot token, при отказе нуль. Тело `0x6F3A8060`
  [в S11](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3A8060.json)
  лениво получает или создаёт объект реестра по полю таблицы `+0x1C`;
  точный lifetime здесь не прослежен.
- Статический отрицательный контроль: `GetLocalPlayer` не вызывает
  `GetGameUI`; [GetCameraField](../../../../src/Jass/jassnatives_camera.cpp)
  получает данные через UI и камеру. Наличие обоих нативов не означает,
  что их локальные значения одинаково сохраняются после sleep.

## Ограничения и отвергнутые гипотезы

S11 помечает `GetLocalPlayer` как `TODO`; здесь восстановлен маршрут
инструкций, а не готовое matching C++-тело или исполненный эксперимент.
Соседние helper-записи имеют статусы `EXACT`/`DIFFERS`, которые не
повышают evidence всего маршрута. Класс объекта из `+0x58`, семантика
режима 1, значение `+0x2A`, стоимость/повторяемость регистрации token и
связь с JASS instance не подтверждены. Динамических положительных и
отрицательных контролей нет. Прямое отождествление JASS result с
числовым player index отвергается формой вызовов; сетевой ID из него
не следует.

## Значение для проекта

При разделении общего исполнения карты и персонального представления
нужно сохранить наблюдаемое поведение `GetLocalPlayer`, включая режимную
ветку и identity/lifetime handle. Следующий опыт: два клиента с разными
локальными слотами, режимы по обе стороны `IsGameModeOne`, сравнение
JASS handle identity до и после sleep, отдельно отказ без таблицы.
Связанные границы: [FND-0005](FND-0005-instance-context.md) и
[FND-0009](FND-0009-local-input-continuations.md). Это исследование не
выбирает схему исполнения JASS и не разрешает повторять общие эффекты.

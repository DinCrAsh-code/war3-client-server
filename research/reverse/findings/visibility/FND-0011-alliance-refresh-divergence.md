# FND-0011 — C++ `SetPlayerAlliance` расширяет ветку обновления маски

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | jass, alliance, shared-vision, reconstruction |
| Kind | negative-result |
| Evidence | static |
| Verification | source-reviewed |
| State | open |
| Impact | security, compatibility, correctness |
| Scope | S11, заявленная Game.dll 1.26a/build 6401, x86; `JASS_SetPlayerAlliance` `0x6F3C1050`; fingerprint DLL не установлен |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [IDA-запись](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C1050.json), [C++-тело](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/pipeline/src/Jass/jasssetplayeralliance.cpp), [описание closure](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/pipeline/docs/targets/JASS_PlayerStateNatives2.md) |

## Вывод

В исходной IDA-выгрузке тип отношения, отличный от `5` и `9`, перепрыгивает
оба уведомления **и** проверку активного мира с обновлением мировой маски.
В опубликованном C++ этой ревизии условие `allianceType == 5 || 9`
охватывает только уведомления, а `RefreshPlayerWorldVisibilityMasks` и
`TailCleanup` стоят после блока и при активном мире вызываются для любого
типа. Это различие в наборе вызовов, а не только в регистрах/порядке
инструкций. Статус функции в адресном реестре — `DIFFERS`, score
`0.431372549`; он не удостоверяет поведенческую эквивалентность.

## Основание и контроли

Положительный статический путь: при значении `5` или `9` поток проходит
`NotifyPlayerControllerChanged` (`0x6F416700`), `CPlayerWar3FinishLeaveNotify`
(`0x6F41B4C0`), затем при ненулевом поле мира `+0x3E0` вызывает
`RefreshPlayerWorldVisibilityMasks` (`0x6F408070`) и tail cleanup
(`0x6F016CD0`). Отрицательный путь: после сравнений с `5` и `9` инструкция
`0x6F3C10A6` (`jnz 0x6F3C10D4`) перепрыгивает всю эту часть при другом
типе. Значение `value` выбирает `EnableAllianceFlag`/`DisableAllianceFlag`
раньше и само по себе этот переход не отменяет.

В C++ блок закрывается после `CPlayerWar3FinishLeaveNotify`; отдельный
`if (world+0x3E0)` расположен ниже. При ненулевом `+0x3E0` и типе,
отличном от `5`/`9`, он вызывает функции, которые IDA-поток не вызывает.
Это проверяемый контрпример к заявлению, что расхождение объясняется
только распределением регистров.

## Ограничения и следующий шаг

Собственный запуск, компиляция изменённого C++, verifier-gate и проверка
состояния мира не выполнялись. Возможно, дополнительные вызовы не меняют
итог при части входов, но изменение трассы уже установлено статически.
Названия значений `5`/`9` и побочные эффекты `TailCleanup` ещё требуют
проверки; одно имя «shared vision» в комментарии не определяет все правила.

Исправить область условия в matching-реконструкции, сравнить инструкции
`quick.py`/`verify.py` и проверить трассы для `5`, `9` и другого типа при
активном мире. Затем проверить изменение `+0x3C`/`+0x3E`/`+0x40` у
`SWorldVisibilityMaskState` и ответы [FND-0010](FND-0010-unit-submit-gates.md).
Пока исправление и контроль не прошли, эту C++-ветку нельзя использовать
как точный контракт для серверной политики видимости.

Следующий статический проход установил смысл вызываемого для игрока B
`CPlayerWar3FinishLeaveNotify` в этой цепочке: он пересчитывает `B+0x2E0`
по направленным полям `5/9` других игроков. См.
[FND-0015](FND-0015-directed-alliance-mask.md). Это не устраняет
расхождение ветвей в опубликованном C++.

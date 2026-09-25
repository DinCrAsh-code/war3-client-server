# FND-0007 — видимость точки, юнита и детект имеют разные маршруты

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, unit, detection, player, jass |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | C++-реконструкция S10, заявленная сборка 6401; fingerprint экспорта не установлен |
| Sources | [S10](../../SOURCES.md#s10), ревизия 2fc76c8035554912dd66fc0a06a39eda376a806c |

## Вывод и основание

В просмотренном исходнике запрос видимости точки не заменяет запрос о юните.

| Запрос | Наблюдаемый путь в исходнике | Граница |
|---|---|---|
| `JASS_IsVisibleToPlayer` | Player handle → индекс/маска → `IsPointVisible` → `Prepare`/`Submit` | Чтение fog-кодов точки, не полей объекта |
| `JASS_IsUnitVisible` | Unit/player handles → `CUnit::SubmitToPlayerTable(index, 0, 4)` | Внутри `SPlayerTable::SubmitUnit` ещё переходит в оригинал; есть дополнительная проверка бита юнита |
| `JASS_IsUnitDetected` | Unit/player handles → `CUnit::QueryDetection(index, -1)` | Через handle читаются две группы записей детекта; писатели здесь не восстановлены в связную цепочку |

Проверены тела [point natives](../../../../src/Jass/jassnatives_xyvisibility.cpp),
[unit natives](../../../../src/Jass/jassnatives_unitquery.cpp),
[unit query](../../../../src/Unit/unit_playertable.cpp),
[detection](../../../../src/Unit/unit_querydetection.cpp),
[Prepare/PointToCell](../../../../src/Widget/playertablesubmitprepare.cpp),
[point visibility](../../../../src/Widget/playertablepointvisible.cpp),
[Submit](../../../../src/Widget/playertablesubmitparts.cpp) и макрос/запись
`SPlayerTable::SubmitUnit` в
[unreconstructed_thunks](../../../../src/Misc/unreconstructed_thunks.cpp).
Последняя запись генерирует переход в оригинальный образ.

## Ограничения и контроли

Это статическое чтение реконструкции, не независимое сравнение с Game.dll.
Положительные и отрицательные динамические контроли не запускались.
Полнота правил невидимости, общего обзора, spectator/reconnect и прав на
отдельные поля/события не доказана. Имена неизвестных битов не переименованы
в права доступа. Старые комментарии о готовности могут отставать от тела:
например, `PointToCell` уже имеет C++-реализацию вопреки верхнему комментарию.

## Значение и следующая проверка

Приоритет P0: разобрать `SPlayerTable::SubmitUnit` и writers его входов,
затем сравнить ответы на сценариях тумана, общего обзора, невидимости и
детекта для двух игроков. Это уточняет, но не отменяет ограничение
[FND-0006](FND-0006-fog-provider-scope.md): одна сетка тумана не доказывает
готовый безопасный срез. Обновление тумана имеет отдельную UI-зависимость —
[FND-0008](FND-0008-fog-ui-dependency.md).

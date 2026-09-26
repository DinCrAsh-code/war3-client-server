# FND-0008 — обновление тумана обращается к локальному world frame

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | fog, presentation, selection, player-relations, lifetime |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | Тело RefreshUnitFog в S10, заявленная сборка 6401; fingerprint экспорта не установлен |
| Sources | [S10](../../SOURCES.md#s10), ревизия 2fc76c8035554912dd66fc0a06a39eda376a806c |

## Вывод и основание

В [SPlayerTableSubmitParts::RefreshUnitFog](../../../../src/Widget/playertableunitfogrefresh.cpp)
после проверок юнита и флагов находится следующий порядок:

1. Получить `GameUI` и вызвать `world frame → AddSelectedUnitIfNotAlready`.
2. Использовать переданную маску либо вычислить её из отношений владельца,
   специальных масок юнита и дополнительного vision word.
3. Вызвать `RefreshField10C` и `RefreshRelationMaskFields`.
4. Передать юнит и маску в `ApplyUnitFogRadius`.

В том же файле `ApplyUnitFogRadius` реализован переходом в оригинальную DLL.
[AddSelectedUnitIfNotAlready](../../../../src/GameUI/worldframeaddselectedunit.cpp)
имеет C++-тело с проверкой флагов и добавлением ссылки в массив world frame.
Его имя само по себе не доказывает все условия уникальности элементов.

Это конкретная связь writer тумана с состоянием UI в опубликованной
реконструкции. Она не объясняет, необходим ли этот побочный эффект для
симуляции или обслуживает локальное представление.

## Ограничения и контроли

Игра не запускалась; сравнения с оригинальным образом и динамических
положительных/отрицательных контролей нет. Корректность маски, полнота
вызовов writer, назначение списка world frame и последствия пропуска вызова
не установлены. Наличие этой связи не доказывает невозможность отделения UI.

## Значение и следующая проверка

Приоритет P0: установить владельца и потребителей списка, порядок изменения
масок и чтения [visibility queries](FND-0007-player-visibility-routes.md).
После этого сравнить поведение минимального сценария с сохранённым локальным
эффектом и с изолированным обслуживанием этого эффекта. Не удалять вызов UI
по названию функции. Эта находка конкретизирует одну из зависимостей для
[исследования startup/headless](../startup/FND-0001-headless-boundary.md),
но не назначает серверный runtime.

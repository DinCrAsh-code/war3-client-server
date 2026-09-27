# Индекс находок по движку

Обновлено: 2026-09-27. Все перечисленные результаты — **прочитанные
исследовательские контракты**, не новые прогоны в этом проекте.
Классификаторы: [README](README.md). Ревизия и ссылки: [SOURCES](SOURCES.md).
Реконструкция коллеги: [приоритетные маршруты](SRC_MAP.md) и
[каталог всех исходников](catalog/README.md).

## Найденные контракты и границы

| ID и вывод | Subsystem / теги | Kind | Evidence | Verification | State |
|---|---|---|---|---|---|
| [FND-0001: готовой headless-границы нет](findings/startup/FND-0001-headless-boundary.md) | startup; presentation, lifetime | boundary | static | source-reviewed | open |
| [FND-0002: порядок выделения и приказов влияет на мир](findings/simulation/FND-0002-command-order.md) | simulation; selection, identity, network | contract | connected | source-reviewed | bounded |
| [FND-0003: ввод времени связан с фазами мира](findings/simulation/FND-0003-world-phases.md) | simulation; time, session, lifetime | contract | connected | source-reviewed | bounded |
| [FND-0004: startup seed и JASS reseed различаются](findings/simulation/FND-0004-rng-startup.md) | simulation; rng, startup, jass | contract | connected | source-reviewed | bounded |
| [FND-0005: JASS привязан к контексту и lifetime handles](findings/jass/FND-0005-instance-context.md) | jass; handles, callbacks | contract | isolated | source-reviewed | bounded |
| [FND-0006: подготовка тумана не равна видимости игрока](findings/visibility/FND-0006-fog-provider-scope.md) | visibility; terrain, pathing | boundary | isolated | source-reviewed | bounded |
| [FND-0007: точка, юнит и детект идут разными путями](findings/visibility/FND-0007-player-visibility-routes.md) | visibility; unit, detection, jass | boundary | static | source-reviewed | bounded |
| [FND-0008: fog writer обращается к world frame](findings/visibility/FND-0008-fog-ui-dependency.md) | visibility; presentation, lifetime | boundary | static | source-reviewed | bounded |
| [FND-0009: локальные входы и состояние ожидания JASS](findings/jass/FND-0009-local-input-continuations.md) | jass; camera, tls, continuation | boundary | static | source-reviewed | bounded |
| [FND-0040: натив ожидания выводит JASS instance с отдельным статусом](findings/jass/FND-0040-jass-vm-yield-status.md) | jass; tls, native-dispatch, sleep, continuation | contract | static | source-reviewed | bounded |
| [FND-0041: trigger action удерживает instance и продолжает исполнение](findings/jass/FND-0041-trigger-action-continuation.md) | jass; trigger, sleep, event, continuation, handles | contract | static | source-reviewed | bounded |
| [FND-0042: sync trigger использует маску игроков и ready-команду](findings/jass/FND-0042-trigger-sync-barrier.md) | jass; trigger, sync, player-mask, network-command | contract | static | source-reviewed | bounded |
| [FND-0043: GetLocalPlayer выбирает слот и возвращает handle token](findings/jass/FND-0043-local-player-handle-selection.md) | jass; local-context, player, handles | boundary | static | source-reviewed | bounded |
| [FND-0044: команды trigger различаются token и sender](findings/jass/FND-0044-trigger-continuation-command-identity.md) | jass; trigger, network-command, handle, token, authority | boundary | static | source-reviewed | bounded |
| [FND-0045: таймер trigger передаёт событие владельцу](findings/jass/FND-0045-trigger-timer-observer-bridge.md) | jass; trigger, timer, observer, sleep, continuation | contract | static | source-reviewed | bounded |
| [FND-0046: продолжение повторно читает JASS TLS и local-player slot](findings/jass/FND-0046-local-player-context-after-trigger-sleep.md) | jass; local-context, player, tls, sleep, continuation | boundary | static | source-reviewed | bounded |
| [FND-0047: resume dispatch читает TLS текущего event pump](findings/jass/FND-0047-resume-dispatch-tls-boundary.md) | jass; continuation, network-command, event-pump, tls | boundary | static | source-reviewed | bounded |
| [FND-0048: event worker привязывает TLS array контекста](findings/jass/FND-0048-event-context-tls-rebinding.md) | jass; continuation, event-context, event-pump, tls | boundary | static | source-reviewed | bounded |
| [FND-0049: GetLocalPlayer зависит от активной сетевой записи](findings/jass/FND-0049-local-player-slot-writers.md) | jass; local-context, player, net-session, tls, continuation | boundary | static | source-reviewed | bounded |
| [FND-0051: sender команды определяется сетевой записью](findings/jass/FND-0051-resume-sender-resolution.md) | jass; continuation, network-command, sender, player-slot, authority | boundary | static | source-reviewed | bounded |
| [FND-0053: ready снимает бит без локальной проверки membership](findings/jass/FND-0053-trigger-ready-membership-boundary.md) | jass; trigger, continuation, sender, sync-mask, token, authority | boundary | static | source-reviewed | bounded |
| [FND-0054: общий native dispatch ведёт к UI и миру](findings/jass/FND-0054-cross-domain-native-dispatch.md) | jass; vm, native-dispatch, local-context, camera, simulation, compatibility | boundary | static | source-reviewed | bounded |
| [FND-0056: сброс GameUI привязан к lifecycle мира](findings/jass/FND-0056-gameui-world-reset-boundary.md) | jass; camera, presentation, gameui, world-lifecycle, tls | boundary | static | source-reviewed | bounded |
| [FND-0058: продолжение trigger может записать состояние юнита](findings/jass/FND-0058-resumed-trigger-unit-state-write.md) | jass; trigger, continuation, native-dispatch, unit, world-state, handles | boundary | static | source-reviewed | bounded |

Основной evidence относится только к выводу карточки. Например, связанный
компонентный запуск не доказывает обычный запуск всей игры, а проверка
маски тумана не доказывает `IsUnitVisible` для каждого игрока.

## Карта открытых областей

| Подсистема | Что известно / где вход | Следующий существенный вопрос |
|---|---|---|
| Startup | FND-0001, FND-0004 | Обычный запуск, обязательные владельцы и полный teardown |
| Simulation | FND-0002–0004 | Все игровые действия, pathfinding/коллизии, полный матч и порядок RNG |
| JASS | FND-0005, FND-0009, FND-0040–0049, FND-0051/0053/0054/0056/0058; TLS/VM continuation, sender gates, GameUI lifecycle и условный world setter | Проверить peer/trigger authority и фактическое число local/UI и world эффектов после ожидания на двух игроках |
| Visibility | FND-0006–0008; чтение точки/юнита/детекта и fog writer | Завершить маршрут SubmitUnit и writers; оракул, общий обзор, права на поля/события |
| Presentation | S01/S03, FND-0008/0009; локальные входы и UI-зависимость тумана | Поле за полем проверить потребителей мира; клиентский runtime ещё не выбран |
| Resources | Компонентный маршрут архивов/собственной карты в S01/S09 | Произвольные карты и обязательный серверу ресурсный состав |
| Network | FND-0002/0003; action/turn dispatch в S10, см. SRC_MAP | Связать полный ввод игрока с миром; новый wire-протокол ещё проектируется |
| Map extensions | Общего подтверждённого контракта не перенесено | Версии расширений, зависимости от UI, server mode и отдельная приёмка |

Отсутствующая карточка означает пробел, а не отсутствие подсистемы.
Результаты одной карты или расширения не распространяются на весь движок.
Новый протокол, адаптеры и продуктовые решения находятся в
[архитектуре](../../docs/ARCHITECTURE.md) и [ADR](../../docs/decisions/README.md).

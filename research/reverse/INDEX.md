# Индекс находок по движку

Обновлено: 2026-09-25. Все перечисленные результаты — **прочитанные
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

Основной evidence относится только к выводу карточки. Например, связанный
компонентный запуск не доказывает обычный запуск всей игры, а проверка
маски тумана не доказывает `IsUnitVisible` для каждого игрока.

## Карта открытых областей

| Подсистема | Что известно / где вход | Следующий существенный вопрос |
|---|---|---|
| Startup | FND-0001, FND-0004 | Обычный запуск, обязательные владельцы и полный teardown |
| Simulation | FND-0002–0004 | Все игровые действия, pathfinding/коллизии, полный матч и порядок RNG |
| JASS | FND-0005, FND-0009; C++-обвязка и исходные redirects в S10 | Локальные контексты, ожидания, события и синхронизация без изменения карты |
| Visibility | FND-0006–0008; чтение точки/юнита/детекта и fog writer | Завершить маршрут SubmitUnit и writers; оракул, общий обзор, права на поля/события |
| Presentation | S01/S03, FND-0008/0009; локальные входы и UI-зависимость тумана | Поле за полем проверить потребителей мира; клиентский runtime ещё не выбран |
| Resources | Компонентный маршрут архивов/собственной карты в S01/S09 | Произвольные карты и обязательный серверу ресурсный состав |
| Network | FND-0002/0003; action/turn dispatch в S10, см. SRC_MAP | Связать полный ввод игрока с миром; новый wire-протокол ещё проектируется |
| Map extensions | Общего подтверждённого контракта не перенесено | Версии расширений, зависимости от UI, server mode и отдельная приёмка |

Отсутствующая карточка означает пробел, а не отсутствие подсистемы.
Результаты одной карты или расширения не распространяются на весь движок.
Новый протокол, адаптеры и продуктовые решения находятся в
[архитектуре](../../docs/ARCHITECTURE.md) и [ADR](../../docs/decisions/README.md).

# Индекс находок по движку

Обновлено: 2026-09-25. Все перечисленные результаты — **прочитанные
исследовательские контракты**, не новые прогоны в этом проекте.
Классификаторы: [README](README.md). Ревизия и ссылки: [SOURCES](SOURCES.md).

## Найденные контракты и границы

| ID и вывод | Subsystem / теги | Kind | Evidence | Verification | State |
|---|---|---|---|---|---|
| [FND-0001: готовой headless-границы нет](findings/startup/FND-0001-headless-boundary.md) | startup; presentation, lifetime | boundary | static | source-reviewed | open |
| [FND-0002: порядок выделения и приказов влияет на мир](findings/simulation/FND-0002-command-order.md) | simulation; selection, identity, network | contract | connected | source-reviewed | bounded |
| [FND-0003: ввод времени связан с фазами мира](findings/simulation/FND-0003-world-phases.md) | simulation; time, session, lifetime | contract | connected | source-reviewed | bounded |
| [FND-0004: startup seed и JASS reseed различаются](findings/simulation/FND-0004-rng-startup.md) | simulation; rng, startup, jass | contract | connected | source-reviewed | bounded |
| [FND-0005: JASS привязан к контексту и lifetime handles](findings/jass/FND-0005-instance-context.md) | jass; handles, callbacks | contract | isolated | source-reviewed | bounded |
| [FND-0006: подготовка тумана не равна видимости игрока](findings/visibility/FND-0006-fog-provider-scope.md) | visibility; terrain, pathing | boundary | isolated | source-reviewed | bounded |

Основной evidence относится только к выводу карточки. Например, связанный
компонентный запуск не доказывает обычный запуск всей игры, а проверка
маски тумана не доказывает `IsUnitVisible` для каждого игрока.

## Карта открытых областей

| Подсистема | Что известно / где вход | Следующий существенный вопрос |
|---|---|---|
| Startup | FND-0001, FND-0004 | Обычный запуск, обязательные владельцы и полный teardown |
| Simulation | FND-0002–0004 | Все игровые действия, pathfinding/коллизии, полный матч и порядок RNG |
| JASS | FND-0005 | Локальные контексты, ожидания, события и синхронизация без изменения карты |
| Visibility | FND-0006 | Оракул «игрок видит объект», детект, общий обзор, права на события |
| Presentation | Встреченные зависимости в S01/S03 | Зависимости UI от мира и варианты их разделения; клиентский runtime ещё не выбран |
| Resources | Компонентный маршрут архивов/собственной карты в S01/S09 | Произвольные карты и обязательный серверу ресурсный состав |
| Network | Ограниченный вход сессии в FND-0002/0003 | Полный ввод игрока и запись; новый wire-протокол ещё проектируется |
| Map extensions | Общего подтверждённого контракта не перенесено | Версии расширений, зависимости от UI, server mode и отдельная приёмка |

Отсутствующая карточка означает пробел, а не отсутствие подсистемы.
Результаты одной карты или расширения не распространяются на весь движок.
Новый протокол, адаптеры и продуктовые решения находятся в
[архитектуре](../../docs/ARCHITECTURE.md) и [ADR](../../docs/decisions/README.md).

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
| [FND-0010: запрос о юните имеет ранние выходы и маску ответа](findings/visibility/FND-0010-unit-submit-gates.md) | visibility; unit, relations, fog, jass | contract | static | source-reviewed | bounded |
| [FND-0011: C++ SetPlayerAlliance расширяет обновление маски](findings/visibility/FND-0011-alliance-refresh-divergence.md) | visibility; jass, alliance, reconstruction | negative-result | static | source-reviewed | open |
| [FND-0012: детект юнита учитывает два канала со счётчиками](findings/visibility/FND-0012-detection-refcounts.md) | visibility; unit, detection, buffs, abilities | contract | static | source-reviewed | bounded |
| [FND-0013: writer тумана юнита меняет две плоскости](findings/visibility/FND-0013-unit-fog-writer-planes.md) | visibility; unit, fog, grid, masks | boundary | static | source-reviewed | bounded |
| [FND-0014: повторная проверка SubmitUnit берёт первый канал детекта](findings/visibility/FND-0014-submit-detection-gate.md) | visibility; unit, detection, relations, jass | boundary | static | source-reviewed | bounded |
| [FND-0015: alliance 5/9 наполняют маску получателя](findings/visibility/FND-0015-directed-alliance-mask.md) | visibility; alliance, player-relations, unit, jass | contract | static | source-reviewed | bounded |
| [FND-0016: JASS-запись тумана выбирает маску адресатов](findings/visibility/FND-0016-map-fog-shared-mask.md) | visibility; jass, fog, alliance, grid | contract | static | source-reviewed | bounded |
| [FND-0017: локальная публикация юнита читает две маски](findings/visibility/FND-0017-local-unit-publication-masks.md) | visibility; unit, player-relations, local-context, fog | boundary | static | source-reviewed | bounded |
| [FND-0018: две плоскости fog-клетки дают общий ответ](findings/visibility/FND-0018-fog-grid-result-table.md) | visibility; fog, grid, unit, jass | contract | static | source-reviewed | bounded |
| [FND-0019: unit fog writer не даёт универсального отзыва обзора](findings/visibility/FND-0019-unit-fog-writer-modes.md) | visibility; unit, fog, grid, writer, revocation | boundary | static | source-reviewed | bounded |
| [FND-0020: смена адресата эффекта переносит два блока состояния](findings/visibility/FND-0020-detection-revocation.md) | visibility; unit, detection, buffs, shared-vision, revocation | contract | static | source-reviewed | bounded |
| [FND-0021: режимы map fog writer меняют обе плоскости](findings/visibility/FND-0021-map-fog-writer-modes.md) | visibility; jass, fog, grid, map-compatibility | contract | static | source-reviewed | bounded |
| [FND-0022: 12 слотов агрегируют ответ клетки в маски объекта](findings/visibility/FND-0022-rebuild-player-masks.md) | visibility; fog, grid, player-mask, item, presentation | boundary | static | source-reviewed | bounded |
| [FND-0023: прямой писатель маски обходит счётчик](findings/visibility/FND-0023-direct-detection-override.md) | visibility; unit, detection, player-mask, override, revocation | boundary | static | source-reviewed | bounded |
| [FND-0024: смена отношений условно пересобирает fog](findings/visibility/FND-0024-alliance-fog-rebuild.md) | visibility; alliance, fog, grid, unit, rebuild | boundary | static | source-reviewed | bounded |
| [FND-0025: fog modifier пишет при создании и Stop исключает replay](findings/visibility/FND-0025-fog-modifier-lifecycle.md) | visibility; jass, fog, grid, modifier, lifecycle | boundary | static | source-reviewed | bounded |
| [FND-0026: fog rebuild имеет таймерный вход с условной очисткой](findings/visibility/FND-0026-unit-fog-rebuild-trigger.md) | visibility; fog, grid, unit, timer, lifecycle | boundary | static | source-reviewed | bounded |
| [FND-0027: снятие битов виджета условно уведомляет GameUI](findings/visibility/FND-0027-widget-mask-notification.md) | visibility; widget, player-mask, ui, presentation, revocation | boundary | static | source-reviewed | bounded |
| [FND-0028: создание юнита наносит fog двумя путями](findings/visibility/FND-0028-unit-fog-invalidation.md) | visibility; unit, creation, movement, fog, invalidation | boundary | static | source-reviewed | bounded |
| [FND-0029: rebuild передаёт производную fog-сетку в terrain](findings/visibility/FND-0029-fog-rebuild-presentation-output.md) | visibility; fog, grid, rebuild, presentation, terrain | boundary | static | source-reviewed | bounded |
| [FND-0030: fog рескан условно обновляет спрайт юнита](findings/visibility/FND-0030-local-sprite-refresh-gate.md) | visibility; fog, unit, destructable, widget, sprite | boundary | static | source-reviewed | bounded |
| [FND-0031: условный выход rebuild обновляет Storm и CSpawn](findings/visibility/FND-0031-fog-rebuild-storm-spawn-refresh.md) | visibility; fog, rebuild, presentation, spawn, terrain | boundary | static | source-reviewed | bounded |
| [FND-0032: движение юнита обновляет spatial grid, fog-отзыв открыт](findings/visibility/FND-0032-unit-reposition-spatial-observer-boundary.md) | visibility; fog, unit, movement, pathfinding, observer | boundary | static | source-reviewed | bounded |
| [FND-0033: spatial observer зависит от runtime-класса](findings/visibility/FND-0033-spatial-observer-concrete-targets.md) | visibility; fog, unit, spatial-grid, observer, teardown | boundary | static | source-reviewed | bounded |
| [FND-0034: деактивация юнита снимает pathing footprint](findings/visibility/FND-0034-unit-deactivation-fog-revocation-boundary.md) | visibility; unit, removal, deactivation, fog, pathfinding | boundary | static | source-reviewed | bounded |
| [FND-0035: деактивация снимает observer-регистрацию](findings/visibility/FND-0035-unit-deactivation-event-registration-release.md) | visibility; unit, deactivation, observer, event, refcount, fog | boundary | static | source-reviewed | bounded |
| [FND-0036: KillUnit задаёт life=0 и открывает два канала уведомлений](findings/visibility/FND-0036-killunit-life-notification-death-boundary.md) | visibility; unit, KillUnit, life, death, observer, fog | boundary | static | source-reviewed | bounded |
| [FND-0037: relation teardown условно вызывает CUnit Deactivate](findings/visibility/FND-0037-relation-teardown-unit-deactivation-boundary.md) | visibility; unit, lifecycle, relation, deactivation, observer, fog | boundary | static | source-reviewed | bounded |
| [FND-0038: ответ публикации юнита допускает локальный MISS-текст](findings/visibility/FND-0038-unit-publication-miss-text-gate.md) | visibility; unit, SubmitUnit, fog, combat, GameUI, text-tag | boundary | static | source-reviewed | bounded |
| [FND-0039: событие D01D4 условно снимает локальный выбор](findings/visibility/FND-0039-unit-d01d4-selection-publication-gate.md) | visibility; unit, observer, SubmitUnit, selection, GameUI | boundary | static | source-reviewed | bounded |
| [FND-0050: сетевой выбор юнита передаёт команду](findings/visibility/FND-0050-unit-selection-command-outbound-boundary.md) | visibility; unit, selection, network, command, disclosure-boundary | boundary | static | source-reviewed | bounded |
| [FND-0052: SetUnitOwner условно снимает биты виджета](findings/visibility/FND-0052-setunitowner-widget-mask-revocation.md) | visibility; unit, ownership, player-mask, selection, GameUI, revocation | contract | static | source-reviewed | bounded |
| [FND-0055: dying-юнит может сохранять радиус обзора](findings/visibility/FND-0055-unit-life-threshold-dying-fog-policy.md) | visibility; unit, life, death, listener, fog, reveal-radius | contract | static | source-reviewed | bounded |
| [FND-0057: selection deltas отправляются перед basic order](findings/simulation/FND-0057-selection-before-basic-order-queue.md) | simulation; selection, order, network-command, identity, queue | contract | static | source-reviewed | bounded |
| [FND-0059: death-event bridge не показывает отзыв fog](findings/visibility/FND-0059-dying-death-event-dispatch-fog-boundary.md) | visibility; unit, death, event, JASS, fog, disclosure | boundary | static | source-reviewed | bounded |
| [FND-0062: basic order применяют к выбранным допущенным юнитам](findings/simulation/FND-0062-inbound-selection-basic-order-application.md) | simulation; selection, basic-order, observer, sender, unit, authority | boundary | static | source-reviewed | bounded |
| [FND-0064: basic order проходит разные gates очереди и задачи](findings/simulation/FND-0064-basic-order-queue-and-task-gates.md) | simulation; order, queue, task, unit, lifecycle, stop | contract | static | source-reviewed | bounded |
| [FND-0068: point, target и fogged order расходятся по payload](findings/simulation/FND-0068-point-target-fogged-order-family.md) | simulation; order, point, target, fogged, selection, identity, disclosure | boundary | static | source-reviewed | bounded |
| [FND-0069: исходящие builders ставят выбор перед приказом](findings/simulation/FND-0069-outbound-point-target-fogged-order-builders.md) | simulation; order, point, target, fogged, flags, selection, turn-store | contract | static | source-reviewed | bounded |
| [FND-0070: локальный бит цели выбирает fogged отдельно от selection](findings/simulation/FND-0070-point-target-route-selected-unit-visibility.md) | simulation; order, point, target, fogged, selection, visibility, disclosure | boundary | static | source-reviewed | bounded |
| [FND-0071: fogged order переносит снимок полей цели](findings/simulation/FND-0071-fogged-target-descriptor-provenance.md) | simulation; order, target, fogged, descriptor, identity, disclosure | boundary | static | source-reviewed | bounded |
| [FND-0072: режимы приказа выбирают point, target и fogged](findings/simulation/FND-0072-multimode-order-variant-gates.md) | simulation; order, point, target, fogged, selection, visibility, result | boundary | static | source-reviewed | bounded |
| [FND-0073: варианты приказа записывают поля цепью writer](findings/simulation/FND-0073-order-payload-writer-chain.md) | simulation; order, target, fogged, serialization, disclosure | contract | static | source-reviewed | bounded |
| [FND-0074: command store проверяет слот и ёмкость](findings/simulation/FND-0074-order-command-store-admission.md) | simulation; order, command-store, sender, queue, admission | boundary | static | source-reviewed | bounded |
| [FND-0075: special-session flush пишет запись 0x1F](findings/simulation/FND-0075-special-order-queue-record.md) | simulation; order, command-store, special-session, sender-slot, record | boundary | static | source-reviewed | bounded |

Основной evidence относится только к выводу карточки. Например, связанный
компонентный запуск не доказывает обычный запуск всей игры, а проверка
маски тумана не доказывает `IsUnitVisible` для каждого игрока.

## Карта открытых областей

| Подсистема | Что известно / где вход | Следующий существенный вопрос |
|---|---|---|
| Startup | FND-0001, FND-0004 | Обычный запуск, обязательные владельцы и полный teardown |
| Simulation | FND-0002–0004, FND-0057/0062/0064/0068–0075; selection → basic/point/target/fogged order → CUnit queue/task gates → local command store/`0x1F` record | Проверить sender ↔ peer, фактический эффект move/stop/target, недействительные ссылки; далее pathfinding, RNG и полный матч |
| JASS | FND-0005, FND-0009; C++-обвязка и исходные redirects в S10 | Локальные контексты, ожидания, события и синхронизация без изменения карты |
| Visibility | FND-0006–0008, FND-0010–0039, FND-0050/0052/0055/0059; точка/юнит/детект, writers, owner change, dying и death events | Установить тайминг записи/отзыва fog при death/RemoveUnit и границу выдачи полей юнита; затем двухигроковый оракул |
| Presentation | S01/S03, FND-0008/0009; локальные входы и UI-зависимость тумана | Поле за полем проверить потребителей мира; клиентский runtime ещё не выбран |
| Resources | Компонентный маршрут архивов/собственной карты в S01/S09 | Произвольные карты и обязательный серверу ресурсный состав |
| Network | FND-0002/0003; action/turn dispatch в S10, см. SRC_MAP | Связать полный ввод игрока с миром; новый wire-протокол ещё проектируется |
| Map extensions | Общего подтверждённого контракта не перенесено | Версии расширений, зависимости от UI, server mode и отдельная приёмка |

Отсутствующая карточка означает пробел, а не отсутствие подсистемы.
Результаты одной карты или расширения не распространяются на весь движок.
Новый протокол, адаптеры и продуктовые решения находятся в
[архитектуре](../../docs/ARCHITECTURE.md) и [ADR](../../docs/decisions/README.md).

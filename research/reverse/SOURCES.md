# Источники знаний о движке

Срез проверки: **2026-09-25**. Основной источник —
[war3-engine-atlas](https://github.com/DinCrAsh-code/war3-engine-atlas),
ветка `agent/sim-research`, commit
`7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8`.
Изучена локальная копия; commit совпал с удалённой веткой на момент проверки.
Источник закрытый: для перехода по ссылкам нужен отдельный доступ.
Это не утверждение о содержимом его `main` или последующих ревизий.

В проект перенесено краткое описание контрактов движка и их ограничений.
Код, история, сырые отчёты и продуктовые компоненты других приложений не
копировались. Исходные динамические эксперименты здесь **не повторялись**:
все начальные карточки имеют `verification: source-reviewed`.

## Первичные документы

| ID | Источник на закреплённой ревизии | Использование |
|---|---|---|
| S01 | [Карта подсистем](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/ENGINE_MAP.md) | Навигация, связные и открытые маршруты |
| S02 | [Текущий исследовательский индекс](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/notes/RESEARCH_INDEX.md) | Актуальная граница cases 049–052 |
| S03 | [Граница headless и display](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/notes/PASS_028_HEADLESS_BOUNDARY.md) | FND-0001 |
| S04 | [Юниты и команды](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/UNIT_COMMANDS.md) | FND-0002 |
| S05 | [Ввод сессии и фазы мира](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/WORLD_PHASES.md) | FND-0003 |
| S06 | [Начальные данные и RNG](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/STARTUP_STATE.md) | FND-0004 |
| S07 | [Привязки экземпляра JASS](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/JASS_INSTANCE_BINDINGS.md) | FND-0005 |
| S08 | [Провайдеры подготовки тумана](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/FOG_PROVIDER_QUERIES.md) | FND-0006 |
| S09 | [Активация мира](https://github.com/DinCrAsh-code/war3-engine-atlas/blob/7fa4797ee5f6ed052d2e4e9dd8a5583b3acb43e8/cartography/WORLD_ACTIVATION.md) | FND-0001, FND-0005; ограниченный компонентный маршрут |

Ссылки на компактные evidence и их хеши находятся в соответствующих
исходных контрактах. Числа в карточках — результаты источника, не локальные
прогоны этой задачи. Build scope: Warcraft III 1.26a, Game.dll 1.26.0.6401;
точный fingerprint источника:
`6d21bd9a0f9fbc8446f455c9e89ac994fed68174426fe608fcb9baefd4dec53c`.

## Пробелы и отбор

Каталог `docs/client-server/`, классификатор нативов и численные оценки
нагрузки, на которые ссылался исходный бриф, в изученной ревизии Атласа
не найдены. Они не используются как подтверждение архитектуры или скорости.
Сведения о реализации сторонних продуктов, интерфейсах анализа и расследованиях
конкретных читов исключены из этой базы: они не определяют контракт движка.

Перепроверка ограничивалась чтением документов и сравнением ревизий.
Бинарники, карты, гостевые сценарии, полные отчёты и серверная нагрузка в этой
задаче не исполнялись и не измерялись. Следующие агенты начинают с конкретной
находки и её источника, а не принимают пересказ за независимое доказательство.

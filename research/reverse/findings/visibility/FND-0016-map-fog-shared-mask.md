# FND-0016 — JASS-запись тумана выбирает маску адресатов до изменения сетки

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | jass, fog, alliance, map-compatibility, grid |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [регистрация JASS S10](../../../../src/Jass/jassregisterallnatives.cpp), [`SetFogStateRect`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C1A30.json), [`SetFogStateRadius`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C1AB0.json), [`SetFogStateRadiusLoc`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C1B20.json), [выбор маски `0x6F408110`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F408110.json), [writer прямоугольника `0x6F3B0E90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3B0E90.json) |

## Вывод

Три зарегистрированных натива `SetFogStateRect`, `SetFogStateRadius` и
`SetFogStateRadiusLoc` передают индекс указанного игрока и последний
булев аргумент в общий выборщик `0x6F408110`. При нуле он возвращает
`(uint16_t)(1 << playerIndex)`; при ненуле читает слово записи **этого же**
игрока `+0x2E0` и возвращает его 16 младших бит. Полученный `fogWord`
передаётся вместе с режимом `fogstate` и областью в writer сетки.
Регистрация подтверждает положение булева аргумента; имя
`useSharedVision` взято из реконструкции S10, а не доказано по одному биту.

Таким образом, карта может адресовать изменение тумана либо одному слоту,
либо набору из направленной маски указанного игрока, описанной в
[FND-0015](FND-0015-directed-alliance-mask.md). Смена альянса способна
изменить **будущий набор адресатов** того же JASS-вызова. Это не означает,
что изменение альянса само повторно выполняет предыдущую запись карты.

## Статический маршрут и контроли

| Узел S11 | Наблюдение | Статус S11 |
|---|---|---|
| `0x6F3C1A30` / `0x6F3C1AB0` / `0x6F3C1B20` | Последний аргумент и индекс player передаются `0x6F408110`; результат поступает в writer прямоугольника либо радиуса | DIFFERS / MISMATCH / MATCH-PARTIAL |
| `0x6F408110` | При нулевом аргументе один бит, при ненулевом `record+0x2E0`; результат усечён до слова | MISMATCH |
| `0x6F3B76E0` → `0x6F3B0E90` | Прямоугольник переводится в клетки, затем `fogWord` используется как битовая маска записи | EXACT → MATCH-PARTIAL |
| `0x6F3BA480` | Writer радиуса получает тот же `fogWord`, но его полное тело здесь не исследовано | THUNK |

Положительный статический контроль: все три native вызывают один выборщик
перед записью; прямоугольный writer использует переданное слово в
битовых `or`/`and` двух плоскостей сетки. Отрицательные: нулевой булев
аргумент не читает `+0x2E0` после получения записи игрока; неверный
player handle завершает native до writer. Для `Rect` неверный rect handle,
а для `RadiusLoc` неверный location handle тоже обходят writer. В пути
`Radius` отсутствует проверка handle области, поскольку область задана
числовыми координатами и радиусом.

## Пределы и следующий опыт

Это чтение IDA-текста и опубликованного C++, без запуска. Статусы
`MISMATCH`/`MATCH-PARTIAL` не разрешают считать соответствующие C++-тела
проверенным эквивалентом. Точный смысл режимов `fogstate`, округление
границы радиуса, порядок обновления UI и старых fog-записей, а также
неявные ограничения индекса игрока здесь не установлены. Маска сетки
не является сама по себе разрешением передать объект клиенту.

Двухигроковый опыт: при неизменных координатах и режиме карты выполнить
каждую из трёх форм с булевым 0 и 1; между повторениями направленно
включить и снять типы альянса 5/9. Сравнить изменения клеток для A и B,
`IsPointVisible` и `IsUnitVisible`, а также локальный UI. Контроль:
обратное отношение не задавать; после снятия альянса проверить, меняется
ли ранее записанная клетка без нового JASS-вызова. До опыта это гипотеза,
а не установленное свойство.

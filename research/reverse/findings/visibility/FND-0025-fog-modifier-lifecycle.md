# FND-0025 — fog modifier пишет сетку при создании, а Stop исключает повторное наложение

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | jass, fog, grid, modifier, lifecycle, revocation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, compatibility, correctness |
| Scope | IDA-выгрузка S11, заявленная Game.dll 1.26a/build 6401, x86; fingerprint исходной DLL не установлен; адреса — координаты S11 |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [регистрация JASS S10](../../../../src/Jass/jassregisterallnatives.cpp), [CreateRect `0x6F3D0E70`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3D0E70.json), [CreateRadius `0x6F3D0F90`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3D0F90.json), [CreateRadiusLoc `0x6F3D10F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3D10F0.json), [Start `0x6F3C1BC0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C1BC0.json), [Stop `0x6F3C1BE0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3C1BE0.json), [применение `0x6F40A7B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A7B0.json), [пересчёт `0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json) |

## Вывод

Три JASS `CreateFogModifier*` проверяют игрока и, где нужен handle формы,
прямоугольник или location. После создания `CFogModifier` они вызывают
его виртуальный конфигуратор `+0x60` (rect), `+0x64` (radius) или `+0x68`
(radiusLoc). В [RTTI-записи `CFogModifier`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F94C3DC.json)
это соответственно `0x6F3E6F80`, `0x6F3E7010`, `0x6F3E7090`.
Все три сохраняют режим в младших битах `modifier+0x20`, однобитную
маску игрока в `+0x24` и геометрию, затем **без проверки бита запуска**
вызывают `0x6F40A7B0(modifier, 0xFFFF)`. Create-нативы также
передают объект в `0x6F430C80` для регистрации. Следовательно, статический
путь создания уже достигает writer сетки; нельзя считать, что запись
начинается только в `FogModifierStart`.

`0x6F40A7B0` при аргументе `0xFFFF` расширяет маску `modifier+0x24`
через relation-поля игроков `+0x88/+0xC8`. Затем по форме
`modifier+0x20 & 0x38` выбирает прямоугольник
[`0x6F3B76E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3B76E0.json)
→ `0x6F3B0E90` или радиус `0x6F3BA480`. Эти маршруты доходят до
двух плоскостей grid `+0x2C/+0x30`, как разобрано в
[FND-0021](FND-0021-map-fog-writer-modes.md). Но JASS
`SetFogState*` делает разовую запись непосредственно через свои writer,
без объекта `CFogModifier` и его дальнейшего участия в пересчёте.

`FogModifierStart` (`0x6F3C1BC0`) только устанавливает бит `0x40` в
`modifier+0x20`, `FogModifierStop` (`0x6F3C1BE0`) только снимает его.
Оба не вызывают grid writer. Другой путь,
[`0x6F40A8F0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40A8F0.json),
перестраивает сетку: при условии `(global 0x6FAB6A34 & 3) == 0`
обнуляет массив `grid+0x30`, затем повторно собирает список объектов
типа `+fgm` через callback `0x6F408EB0`. Первый проход вызывает
`0x6F40A7B0` для объектов с битом `0x40` и без бита `0x100` в `+0x20`;
после прохода юнитов второй вызывает его для объектов с **обоими** битами.
`0x100` задаёт разные фазы применения, а не отменяет модификатор.
Поэтому после Stop и **такого пересчёта с очисткой** вклад остановленного
модификатора в текущую плоскость `+0x30` не накладывается заново.
Массив `grid+0x2C` этой общей очисткой не затронут; его прошлое
состояние и другие источники нужно проверять отдельно.

## Контроли и пределы

Положительный статический контроль: у конфигураторов `+0x60/+0x64/+0x68`
есть вызов `0x6F40A7B0` после сохранения формы, а `0x6F40A7B0`
доходит до тех же writer двух плоскостей, что и [FND-0021](FND-0021-map-fog-writer-modes.md).
На пересчёте объект с битом `0x40` проходит к повторному применению в одном
из двух проходов в зависимости от `0x100`.
Отрицательные: при неразрешённом игроке create-нативы уходят до
конфигуратора; `Stop` не пишет grid; при снятом `0x40` оба прохода
пропускают модификатор; при ненулевых двух младших
битах глобального `0x6FAB6A34` показанная очистка `+0x30` пропускается.

`DestroyFogModifier` (`0x6F3C1BA0`, `EXACT`) только разрешает handle и
переходит через виртуальный слот `+0x5C`. У `CFogModifier` и
`CFogModifierTimed` этот слот указывает на
[`0x6F472990`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F472990.json),
который разрешает agent-ref и при выполнении проверок передаёт его в
`0x6F4A6970`. В просмотренных телах нет немедленной очистки fog-grid;
момент исчезновения уничтоженного объекта из списка пересчёта ещё не
доказан. Нельзя обещать немедленный отзыв после `Stop` или `Destroy`:
само создание вызывает writer без проверки `0x40`, а очистка на пересчёте
условна. Также другие модификаторы, юниты или карта могут сохранять обзор.

Статусы S11: три create-натива, конфигураторы, `0x6F40A7B0` и
`0x6F40A8F0` — `TODO`; Start/Stop/Destroy и `0x6F3B76E0` — `EXACT`;
`0x6F3BA480` — `THUNK`; прямоугольный диапазонный writer
`0x6F3B0E90` — `MATCH-PARTIAL`. Это статический маршрут, без запуска
Game.dll, точного fingerprint, измерения момента пересчёта и проверки
клиентских данных. Следующий опыт: в изолированной области без юнитов
создать один modifier режима `4`, вызвать Start, Stop и Destroy по одному,
на каждом шаге наблюдать обе grid-плоскости, пересчёт, три JASS-запроса
точки и UI. Контрольному игроку обзор не давать. Отдельной серией вызвать
`SetFogState*` с тем же режимом, чтобы отличить разовую запись карты от
жизненного цикла модификатора.

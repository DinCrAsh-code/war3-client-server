# FND-0076 — запись `0x1F` попадает в локальную очередь событий

| Поле | Значение |
|---|---|
| Subsystem | network |
| Tags | order, event, queue, special-session, peer-boundary |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; путь `0x6F54CC10` → `0x6F54CAD0` → `0x6F54C490` / `0x6F5491B0`. Установлен локальный enqueue, но не сетевой peer, ownership payload и фактическое исполнение. Fingerprint DLL не известен. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), pinned commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [FND-0075](../simulation/FND-0075-special-order-queue-record.md); [FND-0066 в PR #3](https://github.com/DinCrAsh-code/war3-client-server/blob/83bc16bee5252f67c8ae22d913777d30407f2f10/research/reverse/findings/network/FND-0066-online-turn-store-flush-boundary.md) |

## Локальная цепь

[FND-0075](../simulation/FND-0075-special-order-queue-record.md) заканчивается
вызовом [`0x6F54CC10`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54CC10.json)
с id `0x1F` и локальным store. `0x6F54CC10` получает через vtable
store `+0x28` указатель, размер и дополнительное поле, затем передаёт
их вместе с id и флагом в
[`0x6F54CAD0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54CAD0.json).
Если store отсутствует или второе выходное поле нулевое, эта оболочка
сбрасывает только указатель; она сама не проверяет получателя.

`0x6F54CAD0` вызывает
[`0x6F54C490`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F54C490.json).
Тот берёт заготовку события через `0x6F549B80` из контейнера
`netData+0x2290`, проверяет id `<0xFF`, размер `<0xFFFFF` и
дополнительное поле `<0xFFFFF`. При успехе он записывает в 24-байтный
event: `+0x08` исходный указатель, `+0x0C` размер, `+0x10`
дополнительное поле, `+0x14` id, `+0x15` флаг. В этом теле нет
копирования payload bytes. `0x6F54CAD0` передаёт event в
[`0x6F5491B0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F5491B0.json),
который вызывает `0x6F548FF0` с параметрами `2, 0` для локального
включения в event container. В этой цепи нет аргумента peer или
проверки authenticated sender.

Для id `0x1E`/`0x1F` после enqueue создаётся временный reader поверх
тех же указателя и размера. Он пытается прочитать первое 16-битное
слово через `0x6F6516C0` → `0x6F4C2C70`. При успешной проверке границы
reader увеличивает `netData+0x225C` на прочитанное слово. При нехватке
двух байтов `0x6F4C2B30` ставит курсор за пределом; сравнение курсора
с размером в `0x6F54CAD0` не допускает этого прибавления. Смысл
счётчика `+0x225C` за пределами показанного тела не установлен.

## Контроли и границы

Положительный статический контроль: id `0x1F` с обеими величинами ниже
`0xFFFFF` проходит к `0x6F5491B0`; минимум два байта payload
дополнительно позволяют обновить `+0x225C`. Отрицательные ветви для
id `0xFF`, размера или дополнительного поля `>=0xFFFFF` вызывают
`0x6F54B810` для удаления заготовки и не вызывают enqueue. Payload
короче двух байтов не обновляет счётчик, хотя event уже добавлен.

Отвергнута гипотеза, что вызов `0x6F54CC10` сам отправляет запись
`0x1F` сетевому peer: показанная цепь создаёт и ставит локальный event.
Это не доказывает, что событие никогда не передаётся дальше. Здесь не
проверены lifetime указателя `+0x08`, потребитель очереди, peer mapping
и dynamic type outbound net-client sink из [FND-0066](https://github.com/DinCrAsh-code/war3-client-server/blob/83bc16bee5252f67c8ae22d913777d30407f2f10/research/reverse/findings/network/FND-0066-online-turn-store-flush-boundary.md).
Следующий шаг — пройти потребление события из контейнера и связать его
с конкретным session/connection и входным player admission; затем
сверить трассу двух игроков с отрицательным контролем чужого отправителя.

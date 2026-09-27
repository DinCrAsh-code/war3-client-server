# FND-0040 — натив ожидания выводит текущий JASS instance из интерпретатора с отдельным статусом

| Поле | Значение |
|---|---|
| Subsystem | jass |
| Tags | tls, native-dispatch, sleep, sync, continuation |
| Kind | contract |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | compatibility, correctness, security |
| Scope | S11, адреса IDA предполагаемой Game.dll 1.26a/build 6401 x86; fingerprint образа и исполнение DLL не проверены |
| Sources | [S11](../../SOURCES.md#s11-адресный-корпус-ida-коллеги), commit `10950d496aa7357a6180c1956def50c2a3e8c1a9`; [S10](../../SOURCES.md#s10), commit `2fc76c8035554912dd66fc0a06a39eda376a806c` |

## Вывод

В прочитанном потоке инструкций `TriggerSleepAction` меняет запись **того же** JASS instance, который исполняет opcode stream. После вызова натива интерпретатор проверяет признак ожидания, снимает instance с верхушки TLS-стека и возвращает код `3`, сохраняя курсор исполнения в instance. `TriggerSyncStart` использует тот же стек, но выставляет отдельный признак и возвращает код `4`. Эти коды различаются с обычным завершением (`2`) и должны разбираться вызывающим кодом по отдельности.

## Основание и контроли

1. [Вход `ExecuteOpcodeStream`, 0x6F45E9D0](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F45E9D0.json) кладёт свой `this` в TLS slot 5 через [0x6F44EA20](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F44EA20.json) → [0x6F44DAA0](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F44DAA0.json). Последний добавляет указатель в массив TLS `+0x0C` и увеличивает счётчик `+0x14`. Интерпретатор записывает входной cursor в instance `+0x20`, сбрасывает флаги `+0x34/+0x38/+0x3C` и продвигает `+0x20` во время исполнения.
2. [`TriggerSleepAction`, 0x6F3B2DB0](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3B2DB0.json) передаёт `kind=0` в [`JassThreadSleep`, 0x6F44B2F0](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F44B2F0.json). Тот берёт последний элемент массива TLS slot 5 и записывает instance `+0x34=1`, `+0x38=kind`, `+0x40=*seconds`. В S10 это [C++-тело](../../../../src/Jass/jassthreadstate.cpp), однако статус S11 для `0x6F44B2F0` — `DIFFERS`; вывод здесь основан на указанной последовательности записей S11, а не на заявлении о byte match.
3. После dispatch натива `ExecuteOpcodeStream` проверяет `+0x34` в `0x6F45EFC7`. При ненуле он идёт к `0x6F45F7D4`: [`JassThreadPop`, 0x6F449CE0](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F449CE0.json) уменьшает TLS-счётчик `+0x14`, затем VM возвращает `3`. Это положительный статический контроль связки native → текущий instance → yield.
4. Отрицательный контроль: [`TriggerSyncStart`, 0x6F3B2DC0](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3B2DC0.json) вызывает [`0x6F44B3A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F44B3A0.json) и меняет `+0x3C`, но не `+0x34/+0x40`. VM проверяет `+0x3C` лишь после отрицательной проверки `+0x34`; этот выход снимает TLS-верхушку и возвращает `4` (`ebp=4` в данном блоке). При исчерпании opcode stream отдельный выход возвращает `2`.

## Ограничения и отвергнутые гипотезы

Это статическая причинная связь, не собственное воспроизведение DLL. `TLS slot 5` здесь выбирает текущую запись интерпретатора; он не доказывает отдельную VM на игрока. Возврат `3` сам по себе не доказывает таймер, фактическое возобновление, момент исполнения или семантику общего/персонального эффекта. Эти следующие звенья рассматриваются в [FND-0041](FND-0041-trigger-action-continuation.md). Указанные адреса — координаты источника S11, не runtime-контракт без fingerprint.

## Значение для проекта

Сон скрипта является сохранением состояния исполнения, а не простым ожиданием внутри вызова натива. При проверке архитектуры нужно сохранять идентичность instance, курсор и порядок эффектов по обе стороны yield. [FND-0009](FND-0009-local-input-continuations.md) уже установила локальный вход и запись флагов; здесь дополнена отсутствовавшая причинная связь с выходом VM.

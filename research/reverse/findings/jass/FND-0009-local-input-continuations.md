# FND-0009 — JASS читает локальную камеру и хранит состояние ожидания в потоке

| Поле | Значение |
|---|---|
| Subsystem | jass |
| Tags | local-context, camera, presentation, tls, sleep, continuation |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | compatibility, correctness, security |
| Scope | Указанные тела и регистрации S10, заявленная сборка 6401; fingerprint экспорта не установлен |
| Sources | [S10](../../SOURCES.md#s10), ревизия 2fc76c8035554912dd66fc0a06a39eda376a806c |

## Вывод и основание

В [jassnatives_camera](../../../../src/Jass/jassnatives_camera.cpp)
`JASS_GetCameraField` и getters позиции получают камеру через `GetGameUI`
и возвращают её данные. Локальное представление служит входом скрипта.

В [jassthreadstate](../../../../src/Jass/jassthreadstate.cpp)
`JassThreadSleep` получает текущий JASS-поток через TLS и записывает признаки
ожидания, его вид и длительность; `JassThreadSyncStart` меняет флаг того же
текущего потока. Обе функции имеют C++-тела. Это отдельное наблюдение о
контексте исполнения; здесь не прослежена динамическая цепочка от камеры
через ожидание до возобновления одного скрипта.

Восстановлены не все нижележащие звенья:

- [Construct/CreateChildInstance](../../../../src/Jass/jassinstancebigthunks.cpp)
  и [ExecuteOpcodeStream](../../../../src/Jass/jassexecutestream.cpp) — переходы
  в оригинал. Обёртка `RunOpcodeStream` имеет C++-тело.
- [ExecuteFunc](../../../../src/Jass/jassruntimeexecutefunc.cpp) содержит
  C++-обвязку, но `JassExecuteFuncFrame::Invoke` — переход в оригинал.
- В [регистрации нативов](../../../../src/Jass/jassregisterallnatives.cpp)
  присутствует `GetLocalPlayer`; сама запись не раскрывает его тело.

## Ограничения и контроли

Статическое чтение не доказывает matching, полный scheduler, сохранение
локальных контекстов после sleep или возможность нескольких независимых
VM. Положительные/отрицательные динамические контроли не выполнялись.
Наличие регистраций нативов не означает восстановление всех этих функций.

## Значение и следующая проверка

Приоритет P0: связать локальный ввод, общие/персональные эффекты, handles и
продолжение исполнения. Предлагаемый контроль — два разных локальных состояния
камеры, ожидание и последующее чтение/изменение состояния; проверить, что
общий эффект не дублируется и локальные входы не смешиваются. Дополнить
[FND-0005](FND-0005-instance-context.md) живым маршрутом, затем сравнивать
варианты разделения JASS. Повтор локальных веток для всех игроков решением
не принят.

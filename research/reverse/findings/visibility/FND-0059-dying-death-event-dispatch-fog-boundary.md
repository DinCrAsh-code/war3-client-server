# FND-0059 — dying-состояние предшествует двум death-событиям, а trigger-мост не очищает fog

| Поле | Значение |
|---|---|
| Subsystem | visibility |
| Tags | unit, death, event, JASS, fog, disclosure |
| Kind | boundary |
| Evidence | static |
| Verification | source-reviewed |
| State | bounded |
| Impact | security, correctness, compatibility |
| Scope | S11, заявленная Game.dll 1.26a/build 6401 x86; fingerprint DLL не установлен. Адреса относятся только к этой выгрузке; DLL не запускалась. |
| Sources | [S11](../../SOURCES.md#s11-адресный-реестр-и-matching-pipeline-коллеги), [dying handler `0x6F2A7D80`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2A7D80.json), [death dispatch `0x6F2AD4D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AD4D0.json), [event post `0x6F40B5A0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F40B5A0.json), [`TriggerRegisterDeathEvent` `0x6F3D22C0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F3D22C0.json), [registration `0x6F4364E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F4364E0.json), [widget registration `0x6F2AB3E0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F2AB3E0.json), [CDeathEventReg vtable](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/classes/0x6F94FF84.json), [death-event handler `0x6F43B670`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F43B670.json), [observer delivery `0x6F62A5D0`](https://github.com/FilippTheBestDev/claudecraft/blob/10950d496aa7357a6180c1956def50c2a3e8c1a9/agent_worktrees/funcs/0x6F62A5D0.json) |

## Порядок при переходе в dying

После положительной ветви life threshold из [FND-0055](FND-0055-unit-life-threshold-dying-fog-policy.md)
`CUnit::HandleDying` `0x6F2A7D80` выставляет `unit+0x5C & 0x100`
на `0x6F2A7DB9` и `& 0x20` на `0x6F2A7DCE`. После изменения
life, target и других полей он вызывает `0x6F2AD4D0` на
`0x6F2A7E03`. Значит оба бита уже стоят при входе в этот dispatch.
Если исходный `0x100` уже стоял либо `0x6F076950` вернул неноль,
handler выходит до этого dispatch.

`0x6F2AD4D0` сначала выбирает объект через индекс
`word ptr [dword_6FAB65F4+0x28]` и `0x6F3A1650`, передаёт ему
указатель на unit через `0x6F40B5A0`. Та функция создаёт `CEvent`
с ID `0x80259` и subject=unit, затем вызывает у выбранного объекта
vtable `+0x10`. **После возврата** `0x6F2AD4D0` создаёт второй
`CEvent`, ID `0xD01A0` и subject=unit, и отправляет его через
CUnit vtable `+0x10`. Порядок этих двух вызовов виден в одном теле;
смысл глобального индекса `+0x28` и состав подписчиков первого
объекта здесь не доказаны.

## Конкретный зарегистрированный получатель unit-события

Native `TriggerRegisterDeathEvent` `0x6F3D22C0` сперва разрешает
trigger и widget; при нуле любого из них возвращает ноль. При успехе
он строит event registration и вызывает `0x6F4364E0` с этим
registration, trigger и widget. `0x6F4364E0` привязывает registration
к widget: `0x6F2AB3E0(widget, registration, 1)` вызывает widget
vtable `+0x08` с ключом `0xD01A0` и resource=registration. Затем
вызывает у самого registration vtable `+0x08`, регистрируя trigger
на ключ `0x80259`. Это **две последовательные observer-регистрации**,
а не вызов fog writer.

Для concrete `CDeathEventReg` vtable `0x6F94FF84` slot `+0x0C`
равен `0x6F43B670`. Поэтому при доставке widget-события `0xD01A0`
через общий observer `0x6F62A5D0` совпавший resource может попасть
в этот handler. Он сравнивает ID входного события с
`dword_6F92EDA8`; значение этой data-константы S11 не раскрывает,
поэтому прохождение сравнения для `0xD01A0` остаётся условным.
В успешной ветви handler извлекает subject из входного события,
строит `CScriptEvent` с ID `0x80259` и публикует его через собственный
vtable `+0x10`; регистрация trigger на этом ключе задаёт следующий
получатель. Исполнение кода карты после доставки не прослежено.

## Контроли и предел fog-вывода

Положительный контроль: при несупрессированном dying-переходе
`0x6F2AD4D0` публикует первое событие до второго; регистрация
`0x6F4364E0` использует ровно `0xD01A0` на widget и `0x80259`
на `CDeathEventReg`. `0x6F62A5D0` вызывает resource vtable
`+0x0C` только для записи с совпавшим ключом. Отрицательные
контроли: уже установленный `0x100` или ненулевой `0x6F076950`
пропускают оба события по этому входу; неразрешённый trigger/widget
не создаёт registration; отсутствие observer-записи либо несовпадение
с `dword_6F92EDA8` не приводит к публикации `CScriptEvent` через
показанный handler.

В непосредственных `0x6F2AD4D0`, `0x6F40B5A0`, `0x6F2AB3E0`,
`0x6F4364E0` и `0x6F43B670` нет прямого вызова fog writer
`0x6F409E00`, `RefreshUnitFog` `0x6F40A650` или полного rebuild
`0x6F40A8F0`. Гипотеза «событие `0x80259` само отзывает радиус»
этим маршрутом **не подтверждается**. События доставляют
runtime-зависимым подписчикам, включая код карты; их последствия
и другие death helpers не проверены. Наличие trigger-моста не
устанавливает момент очистки plane `fogTable+0x30` или ответ
локального disclosure query.

Для серверной выдачи это означает необходимость отдельного контракта
на порядок death-событий и пересчёт visibility: умирающий unit
может сохранять `DyingRevealRadius` по FND-0055, а сам dispatch
не даёт доказательства отзыва. Следующая проверка: при единственном
источнике обзора сопоставить оба event delivery с вызовами
`0x6F40A650`/`0x6F40A8F0`, grid `+0x30` и результатом fog query;
повторить без death trigger и с ним, контролируя уже dying unit.
`RemoveUnit` `loc_6F3C8060` остаётся отдельным входом без IDA-тела
в S11; этот event-путь не заменяет его анализ.

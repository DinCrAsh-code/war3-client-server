; Map trigger script for the mini RTS, assembled by script_asm.py.
;
; globals: 0 = creep wave counter, 1 = game over flag
;
; Constants used below (must match the C++ enums):
;   player 4 = neutral hostile, unit type 40 = first creep type,
;   order 5 = attack-move, item types 0..13, map center = 1536.0

.globals 2

; ---------------------------------------------------------------------------
; main: runs once at map init and registers the triggers.
.func main 0 0
    pushi 0
    stg 0
    pushi 0
    stg 1
    pushf 90.0
    pushi @creep_wave
    native RegisterTimerTrigger 2
    pop
    pushf 2.0
    pushi @check_victory
    native RegisterTimerTrigger 2
    pop
    pushi @on_death
    native RegisterDeathTrigger 1
    pop
    ret
.endfunc

; ---------------------------------------------------------------------------
; clamp_int(v, lo, hi)
.func clamp_int 3 0
    ldl 0
    ldl 1
    lt
    jz not_low
    ldl 1
    retv
not_low:
    ldl 0
    ldl 2
    gt
    jz in_range
    ldl 2
    retv
in_range:
    ldl 0
    retv
.endfunc

; ---------------------------------------------------------------------------
; pick_target_player(): random player that is still alive, or -1.
; locals: 0 = tries, 1 = candidate
.func pick_target_player 0 2
    pushi 0
    stl 0
again:
    pushi 0
    native GetPlayerCount 0
    pushi 1
    sub
    native GetRandomInt 2
    stl 1
    ldl 1
    native IsPlayerDefeated 1
    jnz retry
    ldl 1
    retv
retry:
    ldl 0
    pushi 1
    add
    dup
    stl 0
    pushi 6
    lt
    jnz again
    pushi -1
    retv
.endfunc

; ---------------------------------------------------------------------------
; creep_wave: every 90s after the 5 minute mark, spawn a growing group of
; hostile creeps in the middle and send them at a random player's base.
; locals: 0 = count, 1 = i, 2 = target player, 3 = unit, 4 = max type offset
.func creep_wave 0 5
    ldg 1
    jz running
    ret
running:
    native GetGameTime 0
    pushf 300.0
    lt
    jz late_enough
    ret
late_enough:
    ldg 0
    pushi 1
    add
    stg 0
    pushi 2
    ldg 0
    pushi 2
    div
    add
    pushi 2
    pushi 8
    call clamp_int
    stl 0
    call pick_target_player
    stl 2
    ldl 2
    pushi 0
    lt
    jz have_target
    ret
have_target:
    ldg 0
    pushi 3
    div
    pushi 0
    pushi 3
    call clamp_int
    stl 4
    pushi 0
    stl 1
spawn:
    ldl 1
    ldl 0
    lt
    jz spawned
    pushi 4
    pushi 40
    pushi 0
    ldl 4
    native GetRandomInt 2
    add
    pushf 1536.0
    pushf -200.0
    pushf 200.0
    native GetRandomReal 2
    add
    pushf 1536.0
    pushf -200.0
    pushf 200.0
    native GetRandomReal 2
    add
    native CreateUnit 4
    stl 3
    ldl 3
    pushi 5
    ldl 2
    native GetStartX 1
    ldl 2
    native GetStartY 1
    native IssuePointOrder 4
    pop
    ldl 1
    pushi 1
    add
    stl 1
    jmp spawn
spawned:
    ret
.endfunc

; ---------------------------------------------------------------------------
; on_death: extra bounty and item drops for creep kills, gold and lumber
; for hero kills.
; locals: 0 = dying unit, 1 = killer, 2 = level, 3 = killer's player
.func on_death 0 4
    native GetTriggerUnit 0
    stl 0
    native GetKillingUnit 0
    stl 1
    ldl 1
    pushi 0
    eq
    jz has_killer
    ret
has_killer:
    ldl 1
    native GetOwningPlayer 1
    stl 3
    ldl 3
    pushi 0
    ge
    ldl 3
    pushi 4
    lt
    and
    jnz real_player
    ret
real_player:
    ldl 0
    native GetOwningPlayer 1
    pushi 4
    eq
    jz check_hero
    ldl 0
    native GetUnitLevel 1
    stl 2
    ldl 3
    ldl 2
    pushi 3
    mul
    native AddPlayerGold 2
    pop
    ldl 2
    pushi 3
    ge
    jz check_hero
    pushi 0
    pushi 99
    native GetRandomInt 2
    pushi 10
    ldl 2
    pushi 4
    mul
    add
    lt
    jz check_hero
    pushi 0
    pushi 13
    native GetRandomInt 2
    ldl 0
    native GetUnitX 1
    ldl 0
    native GetUnitY 1
    native CreateItem 3
    pop
check_hero:
    ldl 0
    native IsUnitHero 1
    jz done
    ldl 3
    pushi 50
    native AddPlayerGold 2
    pop
    ldl 3
    pushi 10
    native AddPlayerLumber 2
    pop
done:
    ret
.endfunc

; ---------------------------------------------------------------------------
; check_victory: a player with no structures left is defeated; the last
; player standing wins.
; locals: 0 = p, 1 = alive count, 2 = last alive, 3 = player count
.func check_victory 0 4
    ldg 1
    jz running
    ret
running:
    pushi 0
    stl 1
    pushi -1
    stl 2
    native GetPlayerCount 0
    stl 3
    pushi 0
    stl 0
loop:
    ldl 0
    ldl 3
    lt
    jz tally
    ldl 0
    native IsPlayerDefeated 1
    jnz next
    ldl 0
    native CountPlayerStructures 1
    pushi 0
    eq
    jz alive
    ldl 0
    native SetPlayerDefeated 1
    pop
    jmp next
alive:
    ldl 1
    pushi 1
    add
    stl 1
    ldl 0
    stl 2
next:
    ldl 0
    pushi 1
    add
    stl 0
    jmp loop
tally:
    ldl 1
    pushi 1
    le
    jz done
    pushi 1
    stg 1
    ldl 2
    native EndGame 1
    pop
done:
    ret
.endfunc

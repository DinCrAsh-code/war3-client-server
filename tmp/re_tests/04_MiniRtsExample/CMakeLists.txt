add_executable(04_MiniRtsExample
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/AI.cpp
    src/Creeps.cpp
    src/Commands.cpp
    src/Replay.cpp
    src/Script.cpp
    src/MapScript.cpp
    src/Unit.cpp
    src/Movement.cpp
    src/Combat.cpp
    src/Orders.cpp
    src/Hero.cpp
    src/Building.cpp
    src/Widget.cpp
    src/Missile.cpp
    src/Ability.cpp
    src/AbilityWorker.cpp
    src/Buff.cpp
    src/Terrain.cpp
    src/Pathing.cpp
    src/SpatialGrid.cpp
    src/Fog.cpp
    src/Data.cpp
    src/MathUtil.cpp
    src/Memory.cpp
    src/Random.cpp
    src/Bits.cpp
    src/Compress.cpp
    src/Timer.cpp
    src/Events.cpp
)

# No RTTI: with /GR every polymorphic class gets a type descriptor holding
# its decorated name (".?AVCUnit@@" ...), which would hand the class names
# straight to the reverser. The code never uses dynamic_cast/typeid.
if(MSVC)
    target_compile_options(04_MiniRtsExample PRIVATE /GR-)
else()
    target_compile_options(04_MiniRtsExample PRIVATE -fno-rtti)
endif()

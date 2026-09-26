// Mini real-time strategy simulation: 2-4 computer players on a generated
// map, fully deterministic for a given command line. The process exit code
// is a digest of the final game state and replay.

#include "Game.h"

int main(int argc, char** argv)
{
    u32 seed = 0x3C6EF372u;
    for (int i = 1; i < argc; ++i)
        seed = HashBytes(argv[i], (u32)strlen(argv[i]), seed);
    seed ^= (u32)argc * 0x9E3779B9u;

    MemInit();
    InitMathTables();
    InitCrcTable();

    CGame* game = new CGame;
    g_game = game;

    u32 result = 0;
    int players = 2 + (int)(seed % 3u);
    if (game->Init(seed, players))
        result = game->Run(kGameMaxTicks);

    game->Shutdown();
    delete game;
    g_game = 0;
    MemShutdown();

    return (int)(result & 0x7FFFFFFFu);
}

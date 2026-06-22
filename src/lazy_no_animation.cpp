#include <iostream>
#include <cstdlib>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

#include "constants.h"
#include "tetris.h"
#include "solver.h"

int main(void) { 
    // third party setup
    srand(time(0));
    InitWindow(GRID_FRAME_WIDTH + 75, GRID_FRAME_HEIGHT, "Tetris");
    SetTargetFPS(60); // my 2014 macbook gets too warm at 60 fps
    
    // core game logic classes
    GameState state;
    state.playerControlled = false;
    FrameDrawer frameDrawer;

    EvaluationWeights weights = {
        .totalLinesCleared = 1.0,
        .totalLockHeight = 12.885008263218383,
        .totalWellCells = 15.842707182438396,
        .totalColumnHoles = 26.894496507795950,
        .totalColumnTransitions = 27.616914062397015,
        .totalRowTransitions = 30.185110719279040
    };

    Tetrimino tetriminoToPlace = solveForOptimalTetrimino(state.getGrid(), state.getCurrentTetrimino(), state.getNextTetrimino(), weights);

    // main gameplay loop
    while (!WindowShouldClose() and !state.gameOver) {
        state.currentTetrimino = tetriminoToPlace;
        state.moveTetrimino(down);

        if (state.isLineClearInProgress()) {
            state.clearFullLines();
            std::cout << state.linesCleared << std::endl;
        }

        state.initNewTetrimino();
        tetriminoToPlace = solveForOptimalTetrimino(state.getGrid(), state.getCurrentTetrimino(), state.getNextTetrimino(), weights);

        frameDrawer.drawFrame(state, false);
    }

    std::cout << "Game Over" << std::endl;
    
    CloseWindow();
    return 0;
}
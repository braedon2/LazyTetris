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
    InitWindow(GRID_FRAME_WIDTH + 75, GRID_FRAME_HEIGHT, "Tetris");
    SetTargetFPS(60); 
    
    // core game logic classes
    GameState state;
    state.playerControlled = false;
    FrameDrawer frameDrawer;

    EvaluationWeights weights = {
        .totalLinesCleared = 1.67348,
        .totalLockHeight = 4.34036,
        .totalWellCells = -4.774,
        .totalColumnHoles = 11.6506,
        .totalColumnTransitions = 9.23035,
        .totalRowTransitions = 8.49966,
        .totalDeepWells = -23.6849
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
#include <vector>
#include <iostream>
#include <cstdlib>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

#include "util.h"
#include "constants.h"
#include "tetris.h"

int main(void) { 
    // third party setup
    srand(time(0));
    InitWindow(GRID_FRAME_WIDTH + 75, GRID_FRAME_HEIGHT, "Tetris");
    SetTargetFPS(30); // my 2014 macbook gets too warm at 60 fps
    
    // core game logic classes
    GameState state;
    FrameCounter frameCounter;
    FrameDrawer frameDrawer;
    
    bool disableKeyDown = false;

    // main gameplay loop
    while (!WindowShouldClose() and !state.gameOver) {
        frameCounter.nextFrame();

        if (IsKeyPressed(KEY_RIGHT)) {
            state.moveTetronimo(right);
        }
        if (IsKeyPressed(KEY_LEFT)) {
            state.moveTetronimo(left);
        }
        if (IsKeyPressed(KEY_Z)) {
            state.rotateTetronimo(counterClockwise);
        } 
        if (IsKeyPressed(KEY_X)) {
            state.rotateTetronimo(clockwise);
        }
        if (IsKeyDown(KEY_DOWN) and frameCounter.framesPerSoftDropCounter >= FRAMES_PER_SOFT_DROP and not disableKeyDown) {
            state.moveTetronimo(down);
            frameCounter.resetCounters();
        }
        if (IsKeyDown(KEY_DOWN) and state.isCurrentTetrominoPlaced()) {
            disableKeyDown = true;
        }
        if (IsKeyReleased(KEY_DOWN) and disableKeyDown) {
            disableKeyDown = false;
        }

        if (frameCounter.framesPerGridCellCounter >= FRAMES_PER_GRID_CELL) {
            state.moveTetronimo(down);
            frameCounter.resetCounters();
        }

        if (state.isLineClearInProgress()) {
            if (frameCounter.framesPerLineClearCounter >= FRAMES_PER_LINE_CLEAR) {
                state.nextLineClearStep();
                frameCounter.resetCounters();
            }
            if (not state.isLineClearInProgress()) {
                std::cout << state.linesCleared << std::endl;
            }
        }

        if (state.isCurrentTetrominoPlaced() and frameCounter.framesPerTetronimoResetCounter >= FRAMES_PER_TETRONIMO_RESET) {
            state.initNewTetronimo();
            frameCounter.resetCounters();
        }

        frameDrawer.drawFrame(state);
    }

    std::cout << "Game Over" << std::endl;

    // game over animation
    while (!WindowShouldClose()) {
        frameCounter.nextFrame();
        if (frameCounter.framesPerGameOverStepCounter >= FRAMES_PER_GAME_OVER_STEP) {
            frameDrawer.nextGameOverStep();
            frameCounter.resetCounters();
        }

        frameDrawer.drawFrame(state);
    }
    
    CloseWindow();
    return 0;
}

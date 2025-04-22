#include <iostream>
#include <cstdlib>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

#include "constants.h"
#include "tetris.h"

int main(void) { 
    // third party setup
    srand(time(0));
    InitWindow(GRID_FRAME_WIDTH + 75, GRID_FRAME_HEIGHT, "Tetris");
    SetTargetFPS(60); // my 2014 macbook gets too warm at 60 fps
    
    // core game logic classes
    GameState state;
    FrameDrawer frameDrawer;
    
    bool disableKeyDown = false;
    int frameCounter = 0;

    // main gameplay loop
    while (!WindowShouldClose() and !state.gameOver) {
        frameCounter++;

        if (IsKeyPressed(KEY_RIGHT)) {
            state.moveTetrimino(right);
        }
        if (IsKeyPressed(KEY_LEFT)) {
            state.moveTetrimino(left);
        }
        if (IsKeyPressed(KEY_Z)) {
            state.rotateTetrimino(counterClockwise);
        } 
        if (IsKeyPressed(KEY_X)) {
            state.rotateTetrimino(clockwise);
        }
        if (IsKeyDown(KEY_DOWN) and frameCounter >= FRAMES_PER_SOFT_DROP and not disableKeyDown) {
            state.moveTetrimino(down);
            frameCounter = 0;
        }
        if (IsKeyDown(KEY_DOWN) and state.isCurrentTetrominoPlaced()) {
            disableKeyDown = true;
        }
        if (IsKeyReleased(KEY_DOWN) and disableKeyDown) {
            disableKeyDown = false;
        }

        if (frameCounter >= state.fallSpeed()) {
            if (not state.isCurrentTetrominoPlaced()) {
                state.moveTetrimino(down);
                frameCounter = 0;
            }
        }

        if (state.isLineClearInProgress()) {
            if (frameCounter >= FRAMES_PER_LINE_CLEAR) {
                state.nextLineClearStep();
                frameCounter = 0;
            }
            if (not state.isLineClearInProgress()) {
                std::cout << state.linesCleared << std::endl;
            }
        }

        if (state.isCurrentTetrominoPlaced() and frameCounter >= FRAMES_PER_TETRONIMO_RESET) {
            state.initNewTetrimino();
            frameCounter = 0;
        }

        frameDrawer.drawFrame(state);
    }

    std::cout << "Game Over" << std::endl;

    // game over animation
    while (!WindowShouldClose()) {
        frameCounter++;
        if (frameCounter >= FRAMES_PER_GAME_OVER_STEP) {
            frameDrawer.nextGameOverStep();
            frameCounter = 0;
        }

        frameDrawer.drawFrame(state);
    }
    
    CloseWindow();
    return 0;
}

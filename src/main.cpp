#include <vector>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "util.h"
#include "constants.h"
#include "tetris.h"

void drawFrame(GameState state);

int main(void) { 
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris");
    SetTargetFPS(30); // my 2014 macbook gets too warm at 60 fps
    
    GameState state;
    FrameCounter frameCounter;
    bool disableKeyDown = false;

    while (!WindowShouldClose()) {
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
        }

        drawFrame(state);

        if (state.isCurrentTetrominoPlaced() and frameCounter.framesPerTetronimoResetCounter >= FRAMES_PER_TETRONIMO_RESET) {
            state.initNewTetronimo();
            frameCounter.resetCounters();
        }
    }
    
    CloseWindow();
    return 0;
}


void drawFrame(GameState state) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        if (not state.isCurrentTetrominoPlaced()) {
            for (auto pos : state.getCurrentTetronimo().getPositions()) {
                DrawRectangle(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, RED);
                DrawRectangleLines(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
            }
        }

        GameGrid grid = state.getGrid();
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                Position p(x, y);
                if (!grid.isEmpty(p)) {
                    DrawRectangle(BLOCK_SIZE * x, BLOCK_SIZE * y, BLOCK_SIZE, BLOCK_SIZE, grid.getColor(p));
                    DrawRectangleLines(BLOCK_SIZE * x, BLOCK_SIZE * y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
                }
            }
        }
    EndDrawing();
}
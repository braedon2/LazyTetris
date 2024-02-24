#include <vector>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "util.h"
#include "constants.h"
#include "tetris.h"

int main(void) { 
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris");
    SetTargetFPS(30); // my 2014 macbook gets too warm at 60 fps
    
    GameState state;
    bool disableKeyDown = false;

    int framesPerGridCellCounter = 0;
    int framesPerGridCell = 24; // from https://tetris.fandom.com/wiki/Tetris_(NES,_Nintendo)

    int framesPerSoftDropCounter = 0;
    int framesPerSoftDop = 1;

    int framesPerTetronimoResetCounter = 0;
    int framesPerTetronimoReset = 10;

    int framesPerLineClearCounter = 0;
    int framesPerLineClear = 2;

    while (!WindowShouldClose()) {
        framesPerGridCellCounter++;
        framesPerSoftDropCounter++;
        framesPerTetronimoResetCounter++;

        if (IsKeyPressed(KEY_RIGHT)) {
            state.moveTetronimo(right);
        }
        if (IsKeyPressed(KEY_LEFT)) {
            state.moveTetronimo(left);
        }
        if (IsKeyDown(KEY_DOWN) and framesPerSoftDropCounter >= framesPerSoftDop and not disableKeyDown) {
            state.moveTetronimo(down);
            framesPerGridCellCounter = 0;
            framesPerSoftDropCounter = 0;
        }
        if (IsKeyDown(KEY_DOWN) and state.isCurrentTetrominoPlaced()) {
            disableKeyDown = true;
        }
        if (IsKeyReleased(KEY_DOWN) and disableKeyDown) {
            disableKeyDown = false;
        }

        if (framesPerGridCellCounter >= framesPerGridCell) {
            state.moveTetronimo(down);
            framesPerGridCellCounter = 0;
            framesPerTetronimoResetCounter = 0;
        }

        if (state.isLineClearInProgress()) {
            framesPerLineClearCounter++;

            if (framesPerLineClearCounter >= framesPerLineClear) {
                state.nextLineClearStep();
                framesPerLineClearCounter = 0;
            }

            if (not state.isLineClearInProgress()) {
                framesPerLineClearCounter = framesPerLineClear;
            }
        }


        BeginDrawing();
            Position pos = state.getCurrentTetronimo();
            ClearBackground(RAYWHITE);
            if (not state.isCurrentTetrominoPlaced()) {
                DrawRectangle(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, RED);
                DrawRectangleLines(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
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

        if (state.isCurrentTetrominoPlaced() and framesPerTetronimoResetCounter >= framesPerTetronimoReset) {
            state.initNewTetronimo();
            framesPerGridCellCounter = 0;
        }
    }
    
    CloseWindow();
    return 0;
}
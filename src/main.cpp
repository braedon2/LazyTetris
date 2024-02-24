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

    Timer timer;
    Timer speedLimiter;
    GameState state;
    bool disableKeyDown = false;
    std::vector<int> rowClearsToAnimate;

    int framesPerGridCellCounter = 0;
    int framesPerGridCell = 24; // from https://tetris.fandom.com/wiki/Tetris_(NES,_Nintendo)

    int framesPerSoftDropCounter = 0;
    int framesPerSoftDop = 1;

    int framesPerTetronimoResetCounter = 0;
    int framesPerTetronimoReset = 10;

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

            // check for rows to clear
            if (state.isCurrentTetrominoPlaced()) {
                rowClearsToAnimate = state.clearFullRows();
                std::cout << "here" << std::endl;
            }
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

            // check for rows to clear
            if (state.isCurrentTetrominoPlaced()) {
                rowClearsToAnimate = state.clearFullRows();
                std::cout << "here" << std::endl;
            }
        }

        // if (not rowClearsToAnimate.empty()) {
        //     for (int left_i = LEFT_MIDDLE_INDEX, right_i = RIGHT_MIDDLE_INDEX; right_i < GRID_WIDTH; left_i--, right_i++) {
        //         BeginDrawing();
        //             for (int row_i : rowClearsToAnimate) {
        //                 DrawRectangle(BLOCK_SIZE * left_i, BLOCK_SIZE * row_i, BLOCK_SIZE, BLOCK_SIZE, BLANK);
        //                 DrawRectangle(BLOCK_SIZE * right_i, BLOCK_SIZE * row_i, BLOCK_SIZE, BLOCK_SIZE, BLANK);
        //             }
        //         EndDrawing();
        //     }
        // }

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
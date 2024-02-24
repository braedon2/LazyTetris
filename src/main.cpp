#include <vector>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "util.h"
#include "constants.h"
#include "tetris.h"

int main(void) { 
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris");
    SetTargetFPS(30);

    Timer timer;
    Timer speedLimiter;
    GameState state;
    bool disableKeyDown = false;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT)) {
            state.moveTetronimo(right);
        }
        if (IsKeyPressed(KEY_LEFT)) {
            state.moveTetronimo(left);
        }
        if (IsKeyDown(KEY_DOWN) and speedLimiter.getElapsed() > 0.05 and not disableKeyDown) {
            state.moveTetronimo(down);
            timer.start();
            speedLimiter.start();
        }
        if (IsKeyDown(KEY_DOWN) and state.isCurrentTetrominoPlaced()) {
            disableKeyDown = true;
        }
        if (IsKeyReleased(KEY_DOWN) and disableKeyDown) {
            disableKeyDown = false;
        }

        if (timer.getElapsed() > 1) {
            state.moveTetronimo(down);
            timer.start();
        }

        // check for rows to clear
        if (state.isCurrentTetrominoPlaced()) {
            std::cout << "checking rows to clear..." << std::endl;
            auto rows = state.getRowsToClear();
            if (not rows.empty()) {
                std::cout << "full rows: ";
                for (auto r : rows) {
                    std::cout << r << ' ';
                }
                std::cout << std::endl;
            }

            // animate clearing rows

            // remove rows and shift down
        }

        BeginDrawing();
            Position pos = state.getCurrentTetronimo();
            ClearBackground(RAYWHITE);
            DrawRectangle(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, RED);
            DrawRectangleLines(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, BLACK);

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

        if (state.isCurrentTetrominoPlaced() and timer.getElapsed() > 0.5) {
            state.initNewTetronimo();
            timer.start();
        }
    }
    
    CloseWindow();
    return 0;
}
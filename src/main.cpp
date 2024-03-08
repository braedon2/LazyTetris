#include <vector>
#include <iostream>
#include <cstdlib>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

#include "util.h"
#include "constants.h"
#include "tetris.h"

void drawFrame(GameState state, Texture2D sprite);

int main(void) { 
    srand(time(0));
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris");
    SetTargetFPS(30); // my 2014 macbook gets too warm at 60 fps
    Texture2D sprite = LoadTexture("assets/level1-1.png");
    
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
            if (not state.isLineClearInProgress()) {
                std::cout << state.linesCleared << std::endl;
            }
        }

        if (state.isCurrentTetrominoPlaced() and frameCounter.framesPerTetronimoResetCounter >= FRAMES_PER_TETRONIMO_RESET) {
            state.initNewTetronimo();
            frameCounter.resetCounters();
        }

        drawFrame(state, sprite);
    }
    
    CloseWindow();
    return 0;
}


void drawFrame(GameState state, Texture2D sprite) {
    BeginDrawing();
        ClearBackground(BLACK);
        if (not state.isCurrentTetrominoPlaced()) {
            for (auto pos : state.getCurrentTetronimo().getPositions()) {
                float x = (pos.x * BLOCK_SIZE) + (pos.x * GAP_SIZE) + GAP_SIZE;
                float y = (BLOCK_SIZE * pos.y) + (pos.y * GAP_SIZE) + GAP_SIZE;
                DrawTexturePro(
                    sprite,
                    { 0.0f, 0.0f, (float)sprite.width, (float)sprite.height },
                    { x, y, (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }
        }

        GameGrid grid = state.getGrid();
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                Position p(x, y);
                if (!grid.isEmpty(p)) {
                    DrawRectangle(
                        (BLOCK_SIZE * x) + (x * GAP_SIZE) + GAP_SIZE, 
                        (BLOCK_SIZE * y) + (y * GAP_SIZE) + GAP_SIZE, 
                        BLOCK_SIZE, 
                        BLOCK_SIZE, 
                        grid.getColor(p)
                    );
                }
            }
        }
    EndDrawing();
}
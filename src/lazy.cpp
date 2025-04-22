#include <iostream>
#include <cstdlib>
#include <variant>
#include <vector>
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

    Moves moves = solve(state.getGrid(), state.getCurrentTetrimino(), state.getNextTetrimino());
    std::vector<Move>::iterator currentMove = moves.begin();

    int frameCounter = 0;

    // main gameplay loop
    while (!WindowShouldClose() and !state.gameOver) {
        frameCounter++;

        if (IsKeyPressed(KEY_DOWN)) {
            // decrease speed
        }
        if (IsKeyPressed(KEY_UP)) {
            // increase speed
        }

        if (frameCounter >= FRAMES_PER_GRID_CELL and not state.isCurrentTetrominoPlaced()) {
            if (std::holds_alternative<Direction>(*currentMove)) {
                state.moveTetrimino(std::get<Direction>(*currentMove));
            } 
            else if (std::holds_alternative<Rotation>(*currentMove)) {
                state.rotateTetrimino(std::get<Rotation>(*currentMove));
            }            
            else if (currentMove == moves.end()) {
                state.moveTetrimino(down); // move down to lock in place
            }
            currentMove++;
            frameCounter = 0;
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
            moves = solve(state.getGrid(), state.getCurrentTetrimino(), state.getNextTetrimino());
            currentMove = moves.begin();
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
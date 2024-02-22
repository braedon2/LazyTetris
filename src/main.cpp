#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>

#include "util.h"

namespace {
    int GRID_WIDTH = 10;
    int GRID_HEIGHT = 20;
    int BLOCK_SIZE = 15;
    int WINDOW_WIDTH = GRID_WIDTH * BLOCK_SIZE;
    int WINDOW_HEIGHT = GRID_HEIGHT * BLOCK_SIZE;
}

class GridCell {
    public:
    Color color;
    bool isEmpty;

    GridCell(Color _color, bool _isEmpty): color(_color), isEmpty(_isEmpty) {}
};

class GameGrid {
    private:
    std::vector<std::vector<GridCell>> grid;
    
    public:
    GameGrid() {
        for (int i = 0; i < GRID_WIDTH; i++) {
            grid.push_back(std::vector<GridCell>(GRID_HEIGHT, GridCell(BLANK, true)));
        }
    }

    bool isEmpty(Position p) {
        return this->grid[p.x][p.y].isEmpty;
    }

    Color getColor(Position p) {
        return this->grid[p.x][p.y].color;
    }

    bool checkCollision(Position p) {
        if (p.x < 0 or p.x >= GRID_WIDTH) {
            return true;
        }
        if (p.y >= GRID_HEIGHT) {
            return true;
        }
        if (!this->isEmpty(p)) {
            return true;
        }
        return false;
    }

    void setCell(Position p, Color color) {
        this->grid[p.x][p.y] = GridCell(color, false);
    }

    void clearCell(Position p) {
        this->grid[p.x][p.y] = GridCell(BLANK, true);
    }
};

class GameState {
    private:
    GameGrid grid;
    Position currentTetronimo = Position(0, 0);

    public:
    void initNewTetronimo() { this->currentTetronimo = Position(0, 0); }

    GameGrid getGrid() { return this->grid; }

    Position getCurrentTetronimo() { return this->currentTetronimo; }

    // Moves the current tetromino one cell in the given direction so long as it does not cause a 
    // collision with the grid bounds or a placed tetrominoes.
    // Left/right movements are ignored if they would cause a collision
    // A down movement that results in a collision causes the current tetromino to be placed in the 
    // grid and a new tetromino to be initialized
    void moveTetronimo(Direction direction) {
        if (direction == down) {
            Position tmpTetronimo(this->currentTetronimo.x, this->currentTetronimo.y + 1);
            if (this->grid.checkCollision(tmpTetronimo)) {
                this->grid.setCell(this->currentTetronimo, RED);
                this->initNewTetronimo();
            }
            else {
                this->currentTetronimo = tmpTetronimo;
            }
        }
        else if (direction == right) {
            Position tmpTetronimo(this->currentTetronimo.x + 1, this->currentTetronimo.y);
            if (not this->grid.checkCollision(tmpTetronimo)) {
                this->currentTetronimo = tmpTetronimo;
            }
        }
        else if (direction == left) {
            Position tmpTetronimo(this->currentTetronimo.x - 1, this->currentTetronimo.y);
            if (not this->grid.checkCollision(tmpTetronimo)) {
                this->currentTetronimo = tmpTetronimo;
            }
        }
    }
};

int main(void) { 
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris");
    Timer timer;
    Timer speedLimiter;
    SetTargetFPS(60);
    GameState state;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT)) {
            state.moveTetronimo(right);
        }
        if (IsKeyPressed(KEY_LEFT)) {
            state.moveTetronimo(left);
        }
        if (IsKeyDown(KEY_DOWN) and speedLimiter.getElapsed() > 0.05) {
            state.moveTetronimo(down);
            timer.start();
            speedLimiter.start();
        }

        if (timer.getElapsed() > 1) {
            state.moveTetronimo(down);
            timer.start();
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
    }
    
    CloseWindow();
    
    return 0;
}
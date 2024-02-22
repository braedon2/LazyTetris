#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>

#define GRID_WIDTH 5
#define GRID_HEIGHT 10
#define BLOCK_SIZE 15

class Position {
    public:
        int x;
        int y;
        Position(int _x, int _y): x(_x), y(_y) {}
};

class Timer {
    private:
        double startTime;
    public:
        Timer() {
            this->startTime = GetTime();
        }

        void start() {
            this->startTime = GetTime();
        }

        double getElapsed() {
            return GetTime() - this->startTime;
        }
};

class GridBlock {
    public:
        Color color;
        bool isEmpty;

        GridBlock(Color _color, bool _isEmpty): color(_color), isEmpty(_isEmpty) {}
};

class GameGrid {
    private:
        std::vector<std::vector<GridBlock>> grid;
    
    public:
        GameGrid() {
            for (int i = 0; i < GRID_WIDTH; i++) {
                grid.push_back(std::vector<GridBlock>(GRID_HEIGHT, GridBlock(BLANK, true)));
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

        void setBlock(Position p, Color color) {
            this->grid[p.x][p.y] = GridBlock(color, false);
        }

        void clearBlock(Position p) {
            this->grid[p.x][p.y] = GridBlock(BLANK, true);
        }
};

int main(void) { 
    InitWindow(250, 250, "Template-4.0.0");
    Timer timer;
    Timer speedLimiter;
    SetTargetFPS(60);
    Position pos(0, 0);
    GameGrid grid;

    while (!WindowShouldClose()) {
        // player input
        if (IsKeyPressed(KEY_RIGHT)) {
            Position newPos(pos.x+1, pos.y);
            if (!grid.checkCollision(newPos)) {
                pos = newPos;
            }
        }
        if (IsKeyPressed(KEY_LEFT)) {
            Position newPos(pos.x-1, pos.y);
            if (!grid.checkCollision(newPos)) {
                pos = newPos;
            }
        }
        if (IsKeyDown(KEY_DOWN) and speedLimiter.getElapsed() > 0.05) {
            Position newPos(pos.x, pos.y + 1);
            if (not grid.checkCollision(newPos)) {
                pos = newPos;
                timer.start();
            }
            speedLimiter.start();
        }


        // moved piece down one postition after time elapsed
        if (timer.getElapsed() > 1) {
            Position newPos(pos.x, pos.y + 1);

            if (grid.checkCollision(pos)) {
                // game over
                break;
            }
            // active piece moving down would cause a position
            // place it at its current position and start a new active piece
            if (grid.checkCollision(newPos)) {
                grid.setBlock(pos, RED);
                pos = Position(0, 0);
            }
            // moving the active piece down does not cause a collision
            // so set its current position to the new position
            else {
                pos = newPos;
            }

            timer.start();
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangle(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, RED);
            DrawRectangleLines(pos.x * BLOCK_SIZE, BLOCK_SIZE * pos.y, BLOCK_SIZE, BLOCK_SIZE, BLACK);

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
#include <raylib.h>
#include <raymath.h>
#include <vector>

#define GRID_WIDTH 2
#define GRID_HEIGHT 10

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

class BoardCell {
    public:
        Color color;
        bool isEmpty;

        BoardCell(Color _color, bool _isEmpty): color(_color), isEmpty(_isEmpty) {}
};

int main(void) { 
    InitWindow(50, 250, "Template-4.0.0");
    Timer timer;
    SetTargetFPS(60);
    Position pos(0, 0);

    std::vector<std::vector<BoardCell>> grid;
    for (int i = 0; i < GRID_WIDTH; i++) {
        grid.push_back(std::vector<BoardCell>(GRID_HEIGHT, BoardCell(BLANK, true)));
    }

    while (!WindowShouldClose()) {
        if (timer.getElapsed() > 1) {
            if (pos.y == 9) {
                grid[pos.x][pos.y] = BoardCell(RED, false);
                pos.y = 0;
            }
            else if (grid[pos.x][pos.y + 1].isEmpty) {
                pos.y++;
            }
            else if (!grid[pos.x][pos.y].isEmpty) {
                // game over
            }
            else {
                grid[pos.x][pos.y] = BoardCell(RED, false);
                pos.y = 0;
            }

            timer.start();
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangle(pos.x * 25, 25 * pos.y, 25, 25, RED);
            DrawRectangleLines(pos.x * 25, 25 * pos.y, 25, 25, BLACK);

            for (int x = 0; x < GRID_WIDTH; x++) {
                for (int y = 0; y < GRID_HEIGHT; y++) {
                    BoardCell cell = grid[x][y];
                    if (!cell.isEmpty) {
                        DrawRectangle(25 * x, 25 * y, 25, 25, RED);
                        DrawRectangleLines(25 * x, 25 * y, 25, 25, BLACK);
                    }
                }
            }
        EndDrawing();
    }
    
    CloseWindow();
    
    return 0;
}